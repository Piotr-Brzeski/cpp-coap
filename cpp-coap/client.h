//
//  client.h
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-11.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include "context.h"
#include "session.h"
#include <string>
#include <optional>

namespace coap {

class client: public context {
public:
	client();
	
	session create_session(const char* ip, int port);
	
	void process(std::optional<std::string>& response);
};

}

