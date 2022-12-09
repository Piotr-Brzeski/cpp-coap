//
//  main.cpp
//  client
//
//  Created by Piotr Brzeski on 2022-12-06.
//

#include <coap3/coap.h>
#include <iostream>

#include <arpa/inet.h>
coap_address_t resolve_address(in_addr_t addr, in_port_t port) {
	coap_address_t address;
	coap_address_init(&address);
	constexpr uint8_t size = sizeof(address.addr.sin);
	address.size = size;
	address.addr.sin.sin_len         = size;
	address.addr.sin.sin_family      = AF_INET;
	address.addr.sin.sin_addr.s_addr = addr;
	address.addr.sin.sin_port        = port;
	return address;
}



#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
int resolve_address(const char *host, const char *service, coap_address_t *dst) {
	struct addrinfo *res, *ainfo;
	struct addrinfo hints;
	int error, len=-1;

	memset(&hints, 0, sizeof(hints));
	memset(dst, 0, sizeof(*dst));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_UNSPEC;

	error = getaddrinfo(host, service, &hints, &res);

	if (error != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		return error;
	}

	for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
		switch (ainfo->ai_family) {
		case AF_INET6:
		case AF_INET:
			len = dst->size = ainfo->ai_addrlen;
			memcpy(&dst->addr.sin6, ainfo->ai_addr, dst->size);
			goto finish;
		default:
			;
		}
	}

 finish:
	freeaddrinfo(res);
	return len;
}

bool have_response = false;

int main(int argc, const char * argv[]) {
	coap_startup();
	coap_context_t* ctx = nullptr;
	try {
		ctx = coap_new_context(nullptr);
		if(ctx == nullptr) throw false;
		coap_set_log_level(LOG_DEBUG);
		// Support large responses
		coap_context_set_block_mode(ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
		
		coap_address_t dst = resolve_address(inet_addr("127.0.0.1"), htons(5683));
//		coap_address_t dst1 = dst;
//		resolve_address("127.0.0.1", "5683", &dst);
		coap_session_t *session = coap_new_client_session(ctx, nullptr, &dst, COAP_PROTO_UDP);
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
		coap_add_option(pdu, COAP_OPTION_URI_PATH, 5, reinterpret_cast<const uint8_t *>("hello"));
		coap_show_pdu(LOG_WARNING, pdu);
		
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
	
	
	
	
	
/*
	auto src_addr = resolve_address(INADDR_ANY, htons(0));
	coap_context_t* ctx = coap_new_context(&src_addr);
	
	auto dst_addr = resolve_address(inet_addr("127.0.0.1"), htons(5683));
	
	static coap_uri_t uri;
	const char* server_uri = "coap://127.0.0.1/hello";
	coap_split_uri((uint8_t*)server_uri, strlen(server_uri), &uri);
	
	coap_pdu_t* request = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, );
	//request->hdr->type = COAP_MESSAGE_CON;
	request->hdr->id = coap_new_message_id(ctx);
//	request->hdr->code = get_method;
	coap_add_token(request, 4,"abcd");
	coap_add_option(request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s);

	
	
	fd_set            readfds;
//	unsigned char     get_method = 1;
	
	
	
	
	// Set the handler and send the request
	coap_register_response_handler(ctx, message_handler);
	coap_send_confirmed(ctx, ctx->endpoint, &dst_addr, request);
	while(!data_received)
	{
		FD_ZERO(&readfds);
		FD_SET( ctx->sockfd, &readfds );
		int result = select( FD_SETSIZE, &readfds, 0, 0, NULL );
		if ( result < 0 ) // socket error
		{
			exit(EXIT_FAILURE);
		}
		else if ( result > 0 && FD_ISSET( ctx->sockfd, &readfds )) //socket read
		{
				coap_read( ctx );
		}
	}
*/
	return 0;
}
