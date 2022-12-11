//
//  session.h
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once
#include <string>

struct coap_session_t;

namespace coap {

class client;

class session {
public:
//	enum class type { UDP, DTLS };
	
	session(client& ctx, const char* ip, int port);
	~session();
	
	session(session&& session);
	session(session const&) = delete;
	session& operator=(session const&) = delete;
	session& operator=(session&& session) = delete;
	
	std::string send(std::string uri);
	
private:
	client&           m_client;
	::coap_session_t* m_session;
};

}
