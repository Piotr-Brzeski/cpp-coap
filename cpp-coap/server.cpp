//
//  server.cpp
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-11.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include "server.h"
#include "exception.h"
#include <coap3/coap.h>

using namespace coap;

void server::add_endpoint(int port) {
	coap_address_t address;
	coap_address_init(&address);
	address.addr.sin.sin_family = AF_INET;
	address.addr.sin.sin_addr.s_addr = INADDR_ANY;
	address.addr.sin.sin_port = htons(port);
	coap_endpoint_t *endpoint = coap_new_endpoint(m_context, &address, COAP_PROTO_UDP);
	if(endpoint == nullptr) {
		throw coap::exception("endpoint creation failed.");
	}
}

void server::add_handler(std::string const& uri, callback callback) {
	::coap_str_const_t *ruri = coap_make_str_const(uri.c_str());
	::coap_resource_t *resource = coap_resource_init(ruri, 0);
	if(resource == nullptr) {
		throw coap::exception("uri resource creation failed.");
	}
	m_callbacks.emplace_back(std::make_unique<server::callback>(callback));
	::coap_resource_set_userdata(resource, m_callbacks.back().get());
	::coap_register_handler(resource, COAP_REQUEST_GET, [](coap_resource_t* resource, coap_session_t*, const coap_pdu_t*, coap_string_t const*, coap_pdu_t *response) {
		auto callback = static_cast<server::callback*>(coap_resource_get_userdata(resource));
		auto response_data = (*callback)();
		::coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
		::coap_add_data(response, response_data.size(), reinterpret_cast<uint8_t const*>(response_data.data()));
	});
	::coap_add_resource(m_context, resource);
}

void server::run() {
	while (true) {
		::coap_io_process(m_context, COAP_IO_WAIT);
	}
}
