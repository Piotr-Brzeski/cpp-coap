//
//  server.h
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-11.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include "context.h"
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace coap {

class server: public context {
public:
	using callback = std::function<std::string()>;
	
	void add_endpoint(int port);
	void add_handler(std::string const& uri, callback callback);
	void run();
	
private:
	std::vector<std::unique_ptr<callback>> m_callbacks;
};

}

