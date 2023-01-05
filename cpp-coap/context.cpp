//
//  context.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#include "context.h"
#include "exception.h"
#include <coap3/coap.h>

using namespace coap;

namespace {

class init {
public:
	init() {
		::coap_startup();
		//::coap_set_log_level(LOG_DEBUG);
		//::coap_dtls_set_log_level(LOG_DEBUG);
	}
	~init() {
		::coap_cleanup();
	}
};

}

context::context() {
	static auto init_coap = init();
	m_context = ::coap_new_context(nullptr);
	if(m_context == nullptr) {
		throw coap::exception("Context creation failed");
	}
	coap_context_set_block_mode(m_context, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
}

context::~context() {
	::coap_free_context(m_context);
}

