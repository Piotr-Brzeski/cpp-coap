//
//  session.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#include "session.h"
#include "client.h"
#include "exception.h"
#include <coap3/coap.h>
#include <arpa/inet.h>
#include <optional>

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
	if(!coap_dtls_is_supported()) {
		throw coap::exception("Session creation failed - DTLS is not supported.");
	}
	coap_address_t address = resolve_address(ip, port);
	m_session = ::coap_new_client_session_psk(m_client, nullptr, &address, COAP_PROTO_DTLS, identity.c_str(),
	                                          reinterpret_cast<std::uint8_t const*>(key.data()), static_cast<unsigned int>(key.size()));
	if(m_session == nullptr) {
		throw coap::exception("DTLS Session creation failed");
	}
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

std::string session::send(std::string uri) {
	auto message = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET,
	                             coap_new_message_id(m_session), coap_session_max_pdu_size(m_session));
	if(message == nullptr) {
		throw coap::exception("Message creation failed");
	}
	auto result = coap_add_option(message, COAP_OPTION_URI_PATH, uri.size(), reinterpret_cast<const uint8_t*>(uri.data()));
	if(result == 0) {
		throw coap::exception("Set message uri failed");
	}
	auto message_id = ::coap_send(m_session, message);
	if(message_id == COAP_INVALID_MID) {
		throw coap::exception("Message send failed");
	}
	std::optional<std::string> response;
	::coap_session_set_app_data(m_session, &response);
	m_client.process(response);
	::coap_session_set_app_data(m_session, nullptr);
	return *response;
}
