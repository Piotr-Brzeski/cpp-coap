//
//  main.cpp
//  client
//
//  Created by Piotr Brzeski on 2022-12-06.
//

#include <coap3/coap.h>
#include <iostream>
#include <string>

#include <arpa/inet.h>
coap_address_t resolve_address(in_addr_t addr, in_port_t port) {
	coap_address_t address;
	coap_address_init(&address);
	constexpr uint8_t size = sizeof(address.addr.sin);
	address.size = size;
	address.addr.sin.sin_family      = AF_INET;
	address.addr.sin.sin_addr.s_addr = addr;
	address.addr.sin.sin_port        = port;
	return address;
}

const coap_dtls_cpsk_info_t* verify_ih_callback(coap_str_const_t*, coap_session_t*, void* arg) {
	coap_dtls_cpsk_info_t *psk_info = static_cast<coap_dtls_cpsk_info_t*>(arg);
	return psk_info;
}

bool have_response = false;

constexpr auto dtls = true;
coap_dtls_cpsk_t dtls_psk = {};
std::string const key = "EL1s1ckeogWykDpn";
std::string const user = "Client_name";
std::string const uri = "15004";

int main(int argc, const char * argv[]) {
	coap_startup();
	coap_context_t* ctx = nullptr;
	try {
		ctx = coap_new_context(nullptr);
		if(ctx == nullptr) throw false;
		coap_set_log_level(LOG_DEBUG);
		// Support large responses
		coap_context_set_block_mode(ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
		
		coap_address_t dst = resolve_address(inet_addr("172.17.10.90"), htons(5684));
		
		coap_session_t* session = nullptr;
		if(dtls) {
			dtls_psk.version = COAP_DTLS_CPSK_SETUP_VERSION;
			dtls_psk.validate_ih_call_back = verify_ih_callback;
			dtls_psk.ih_call_back_arg = &dtls_psk.psk_info;
			dtls_psk.psk_info.identity.s = reinterpret_cast<const uint8_t*>(user.data());
			dtls_psk.psk_info.identity.length = user.size();
			dtls_psk.psk_info.key.s = reinterpret_cast<const uint8_t*>(key.data());
			dtls_psk.psk_info.key.length = key.size();
			
			session = coap_new_client_session_psk2(ctx, NULL, &dst, COAP_PROTO_DTLS, &dtls_psk);
		}
		else {
			session = coap_new_client_session(ctx, nullptr, &dst, COAP_PROTO_UDP);
		}
		if(session == nullptr) throw false;
		
		// coap_register_response_handler(ctx, response_handler);
		coap_register_response_handler(ctx, [](auto, auto, const coap_pdu_t *received, auto) {
			have_response = true;
			coap_show_pdu(LOG_WARNING, received);
			return COAP_RESPONSE_OK;
		});
		
		// construct CoAP message
		coap_pdu_t *pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET,
		                                coap_new_message_id(session), coap_session_max_pdu_size(session));
		if(pdu == nullptr) throw false;
		
		// add a Uri-Path option
		coap_add_option(pdu, COAP_OPTION_URI_PATH, uri.size(), reinterpret_cast<const uint8_t*>(uri.data()));
		coap_show_pdu(LOG_WARNING, pdu);
		
		// add ?payload?
//		x = coap_add_data(pdu, payload.size(), reinterpret_cast<const uint8_t*>(payload.data()));
//		coap_show_pdu(LOG_WARNING, pdu);
		
		// send the PDU
		coap_send(session, pdu);
		
		while (have_response == 0) {
			coap_io_process(ctx, COAP_IO_WAIT);
		}
		
		coap_session_release(session);
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
