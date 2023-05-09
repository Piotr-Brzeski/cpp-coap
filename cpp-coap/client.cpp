//
//  client.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-11.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include "client.h"
#include "exception.h"
#include <coap3/coap.h>

using namespace coap;

client::client() {
	::coap_register_response_handler(m_context, [](coap_session_t* session, coap_pdu_t const*, coap_pdu_t const* received, coap_mid_t const message_id) {
		auto response = static_cast<std::optional<coap::response>*>(::coap_session_get_app_data(session));
		auto code = coap_pdu_get_code(received);
		*response = coap::response{code};
		if(code == COAP_RESPONSE_CODE_CONTENT) {
			std::size_t response_len = 0;
			uint8_t const* response_data = nullptr;
			auto is_data = ::coap_get_data(received, &response_len, &response_data);
			if(is_data == 1) {
				(*response)->content.assign(reinterpret_cast<char const*>(response_data), response_len);
			}
		}
		return COAP_RESPONSE_OK;
	});
}

/// Create UDP session
session client::create_session(const char *ip, int port) {
	return session(*this, ip, port);
}

/// Create DTLS session
session client::create_session(const char *ip, int port, std::string const& identity, std::string const& key) {
	return session(*this, ip, port, identity, key);
}

void client::process(std::optional<response>& response) {
	constexpr std::uint32_t timeout_ms = 10000;
	auto timeout = timeout_ms;
	while(timeout > 0) {
		auto time = ::coap_io_process(m_context, timeout);
		if(response) {
			return;
		}
		if(time < timeout) {
			timeout -= time;
		}
		else {
			timeout = 0;
		}
	}
}

