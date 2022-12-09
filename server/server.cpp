//
//  main.cpp
//  server
//
//  Created by Piotr Brzeski on 2022-12-06.
//

//#include "common.hh"
#include <coap3/coap.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

coap_address_t resolve_address() {
	coap_address_t address;
	coap_address_init(&address);
	address.addr.sin.sin_family      = AF_INET;
	address.addr.sin.sin_addr.s_addr = INADDR_ANY;
	address.addr.sin.sin_port        = htons(5683); //default port
	return address;
}


int main(void) {
	coap_startup(); // pair with: coap_cleanup()
	coap_context_t* ctx = nullptr;
	try {
		ctx = coap_new_context(nullptr);
		if(ctx == nullptr) throw false;
		coap_set_log_level(LOG_DEBUG);
		
		coap_address_t dst = resolve_address();
		coap_endpoint_t *endpoint = coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP); // pair with: coap_free_context(ctx);
		if(endpoint == nullptr) throw false;
		
		coap_str_const_t *ruri = coap_make_str_const("hello");
		coap_resource_t *resource = coap_resource_init(ruri, 0);
		if(resource == nullptr) throw false;
		
		coap_register_handler(resource, COAP_REQUEST_GET, [](auto, auto, const coap_pdu_t *request, auto, coap_pdu_t *response) {
//			coap_show_pdu(LOG_WARNING, request);
			coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
			coap_add_data(response, 5, (const uint8_t *)"world");
//			coap_show_pdu(LOG_WARNING, response);
		});
		
		coap_add_resource(ctx, resource);
		
		while (true) {
			coap_io_process(ctx, COAP_IO_WAIT);
		}
	}
	catch(...) {
		std::cerr << "EXCEPTION!\n";
	}
	
	if(ctx) {
		coap_free_context(ctx);
	}
	coap_cleanup();
	
	return 0;
}
