//
//  context.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include "context.h"
#include "exception.h"
#include <coap3/coap.h>
#include <map>
#include <optional>

using namespace coap;

namespace {

class init {
public:
	init() {
		::coap_startup();
//		::coap_set_log_level(LOG_DEBUG);
	}
	~init() {
		::coap_cleanup();
	}
};

std::map<int, std::string> responses;
void add_response(int message_id, std::string value) {
	responses.emplace(std::make_pair(message_id, std::move(value)));
}
std::optional<std::string> get_response(int message_id) {
	auto it = responses.find(message_id);
	if(it == responses.end()) {
		return std::nullopt;
	}
	auto value = std::optional<std::string>(std::move(it->second));
	responses.erase(it);
	return value;
}

}

context::context() {
	static auto init_coap = init();
	m_context = ::coap_new_context(nullptr);
	if(m_context == nullptr) {
		throw coap::exception("Context creation failed");
	}
	coap_context_set_block_mode(m_context, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
	::coap_register_response_handler(m_context, [](coap_session_t*, coap_pdu_t const*, coap_pdu_t const* received, coap_mid_t const message_id) {
		std::size_t response_len = 0;
		uint8_t const* response_data = nullptr;
		auto is_data = ::coap_get_data(received, &response_len, &response_data);
		if(is_data == 1) {
			auto response = std::string(reinterpret_cast<char const*>(response_data), response_len);
			add_response(message_id, response);
		}
		else {
			add_response(message_id, "");
		}
		return COAP_RESPONSE_OK;
	});
}

context::~context() {
	::coap_free_context(m_context);
}

std::string context::process(int message_id) {
	constexpr std::uint32_t timeout_ms = 1000;
	auto timeout = timeout_ms;
	while(timeout > 0) {
		auto time = ::coap_io_process(m_context, timeout);
		auto response = get_response(message_id);
		if(response) {
			return *response;
		}
		if(time < timeout) {
			timeout -= time;
		}
		else {
			timeout = 0;
		}
	}
	throw coap::exception("no response from server.");
}
