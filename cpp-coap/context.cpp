//
//  context.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include "context.h"
#include "exception.h"
#include <coap3/coap.h>
#include <mutex>

using namespace coap;

namespace {

static unsigned int init_counter = 0;

auto init_guard() {
	static auto mutex = std::mutex();
	return std::lock_guard(mutex);
}

}

context::context() {
	auto guard = init_guard();
	if(init_counter == 0) {
		::coap_startup();
//		::coap_set_log_level(LOG_DEBUG);
//		::coap_dtls_set_log_level(LOG_DEBUG);
	}
	m_context = ::coap_new_context(nullptr);
	if(m_context == nullptr) {
		::coap_cleanup();
		throw coap::exception("Context creation failed");
	}
	::coap_context_set_block_mode(m_context, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
	++init_counter;
}

context::~context() {
	::coap_free_context(m_context);
	auto guard = init_guard();
	if(--init_counter == 0) {
		::coap_cleanup();
	}
}
