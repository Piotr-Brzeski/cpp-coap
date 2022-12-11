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

class context;

class session {
public:
//	enum class type { UDP, DTLS };
	
	session(context& ctx, const char* ip, int port);
	~session();
	
	std::string send(std::string uri);
	
private:
	context&          m_context;
	::coap_session_t* m_session;
};

}
