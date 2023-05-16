//
//  session.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include "session.h"
#include "client.h"
#include "exception.h"
#include <coap3/coap.h>
#include <arpa/inet.h>
#include <optional>
#include <array>
#include <memory>

#ifdef __APPLE__
// On macOS OpenSSL is used and unsafe renegotiation is disabled by default
#include <openssl/ssl.h>
namespace {
void allow_openssl_unsafe_renegotiation(::coap_session_t* session) {
	auto tls_library_type = COAP_TLS_LIBRARY_NOTLS;
	auto tls = coap_session_get_tls(session, &tls_library_type);
	if(tls_library_type == COAP_TLS_LIBRARY_OPENSSL) {
		SSL_set_options(static_cast<SSL*>(tls), SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION);
	}
}
}
#else
// On Linux GNUTLS is used, and no reconfiguration is needed
namespace {
void allow_openssl_unsafe_renegotiation(::coap_session_t*) {
}
}
#endif

using namespace coap;

namespace {

coap_address_t resolve_address(const char* ip, int port) {
	auto net_addr = ::inet_addr(ip);
	auto net_port = htons(port);
	coap_address_t address;
	coap_address_init(&address);
	constexpr uint8_t size = sizeof(address.addr.sin);
	address.size = size;
	address.addr.sin.sin_family = AF_INET;
	address.addr.sin.sin_addr.s_addr = net_addr;
	address.addr.sin.sin_port = net_port;
	return address;
}

struct opt_list {
	~opt_list() {
		if(value) {
			coap_delete_optlist(value);
		}
	}
	coap_optlist_t* value = nullptr;
};

//coap_dtls_cpsk_info_t const* verify_ih_callback(coap_str_const_t*, coap_session_t*, void *arg) {
//	return static_cast<coap_dtls_cpsk_info_t*>(arg);
//}

}

session::session(client& client, const char* ip, int port)
	: m_client(client)
{
	coap_address_t address = resolve_address(ip, port);
	m_session = ::coap_new_client_session(m_client, nullptr, &address, COAP_PROTO_UDP);
	if(m_session == nullptr) {
		throw coap::exception("UDP Session creation failed");
	}
}

session::session(client& client, const char* ip, int port, std::string const& identity, std::string const& key)
	: m_client(client)
{
	if(coap_dtls_is_supported() != 1) { // || coap_dtls_psk_is_supported() != 1) {
		throw coap::exception("Session creation failed - DTLS is not supported.");
	}
	coap_address_t address = resolve_address(ip, port);
	m_session = ::coap_new_client_session_psk(m_client, nullptr, &address, COAP_PROTO_DTLS, identity.c_str(),
	                                          reinterpret_cast<std::uint8_t const*>(key.data()), static_cast<unsigned int>(key.size()));
	if(m_session == nullptr) {
		throw coap::exception("DTLS Session creation failed");
	}
	allow_openssl_unsafe_renegotiation(m_session);
}

session::~session() {
	if(m_session != nullptr) {
		::coap_session_release(m_session);
	}
}

session::session(session&& session)
	: m_client(session.m_client)
	, m_session(session.m_session)
{
	session.m_session = nullptr;
}

session& session::operator=(session &&session) {
	if(this != &session) {
		if(m_session != nullptr) {
			::coap_session_release(m_session);
		}
		m_session = session.m_session;
		session.m_session = nullptr;
	}
	return *this;
}

std::string session::get(std::string const& uri) {
	static const std::string empty_string;
	send(method::get, uri, empty_string);
	return process();
}

void session::put(std::string const& uri, std::string const& data) {
	send(method::put, uri, data);
	process();
}

void session::send(method method, std::string const& uri, std::string const& data) {
	auto coap_method = [method](){switch(method) {
		case method::get: return COAP_REQUEST_CODE_GET;
		case method::put: return COAP_REQUEST_CODE_PUT;
		default: throw coap::exception("Invalid coap method");
	}}();
	auto message_id = coap_new_message_id(m_session);
	auto max_pdu_size = coap_session_max_pdu_size(m_session);
	auto message = coap_pdu_init(COAP_MESSAGE_CON, coap_method, message_id, max_pdu_size);
	if(message == nullptr) {
		throw coap::exception("Message creation failed");
	}
	auto optlist = opt_list();
	if(!uri.empty()) {
		constexpr std::size_t static_buffer_size = 16;
		std::array<std::uint8_t, static_buffer_size> static_buffer;
		std::uint8_t* buffer = static_buffer.data();
		auto buffer_size = static_buffer_size;
		std::unique_ptr<std::uint8_t[]> dynamic_buffer;
		if(uri.size() > static_buffer_size - 2) {
			//TODO: Remove - leave for now to tune the static buffer size
			assert(false);
			buffer_size = uri.size() + 2;
			dynamic_buffer = std::make_unique<std::uint8_t[]>(buffer_size);
			buffer = dynamic_buffer.get();
		}
		auto number_of_uri_components = coap_split_path(reinterpret_cast<std::uint8_t const*>(uri.c_str()), uri.size(), buffer, &buffer_size);
		while(number_of_uri_components--) {
			auto value = coap_opt_value(buffer);
			if(value == nullptr) {
				throw coap::exception("Set message uri failed - value is null");
			}
			auto length = coap_opt_length(buffer);
			auto option = coap_new_optlist(COAP_OPTION_URI_PATH, length, value);
			if(option == nullptr) {
				throw coap::exception("Set message uri failed - optlist creation failed");
			}
			auto res = coap_insert_optlist(&optlist.value, option);
			if(res != 1) {
				throw coap::exception("Set message uri failed - optlist insert failed");
			}
			auto size = coap_opt_size(buffer);
			buffer += size;
		}
		auto res = coap_add_optlist_pdu(message, &optlist.value);
		if(res != 1) {
			throw coap::exception("Set message uri failed - optlist add failed");
		}
	}
	if(!data.empty()) {
		auto res = coap_add_data(message, data.size(), reinterpret_cast<std::uint8_t const*>(data.data()));
		if(res != 1) {
			throw coap::exception("Set message data failed");
		}
	}
	auto sent_message_id = ::coap_send(m_session, message);
	if(sent_message_id == COAP_INVALID_MID) {
		throw coap::exception("Message send failed");
	}
}

std::string session::process() {
	std::optional<coap::response> response;
	::coap_session_set_app_data(m_session, &response);
	m_client.process(response);
	::coap_session_set_app_data(m_session, nullptr);
	if(!response) {
		throw coap::exception("No response from server.");
	}
	if(response->content.empty()) {
		auto response_class = COAP_RESPONSE_CLASS(response->code);
		if(response_class != 2) {
			auto error_message = std::to_string(response_class*100 + (response->code & 31));
			auto code_phrase = coap_response_phrase(response->code);
			if(code_phrase != nullptr) {
				error_message += " ";
				error_message += code_phrase;
			}
			throw coap::exception(error_message);
		}
	}
	return response->content;
}
