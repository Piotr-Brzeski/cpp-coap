//
//  session.h
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#pragma once
#include <string>

struct coap_session_t;

namespace coap {

class client;

class session {
public:
	/// Create UDP session
	session(client& ctx, const char* ip, int port);
	/// Create DTLS session
	session(client& ctx, const char* ip, int port, std::string const& identity, std::string const& key);
	~session();
	
	session(session&& session);
	session(session const&) = delete;
	session& operator=(session const&) = delete;
	session& operator=(session&& session);

	std::string get(std::string const& uri);
	void put(std::string const& uri, std::string const& data);
	
private:
	enum class method { get, put };
	void send(method method, std::string const& uri, std::string const& data);
	std::string process();
	
	client&           m_client;
	::coap_session_t* m_session = nullptr;
};

}
