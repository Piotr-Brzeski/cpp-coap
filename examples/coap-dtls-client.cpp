//
//  coap-dtls-client.cpp
//  coap-dtls-client
//
//  Created by Piotr Brzeski on 2023-01-05.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#include <cpp-coap/client.h>
#include <cpp-log/log.h>
#include <iostream>

#include "configuration.h"

int main(int argc, const char * argv[]) {
	try {
		auto log = logger::start(logger::cout());
		auto client = coap::client();
		auto session = client.create_session(configuration::ip, configuration::port, configuration::identity, configuration::key);
		auto response = session.get("15001/65640");
		std::cout << "Response: '" << response << "'" << std::endl;
		session.put("15001/65640", "{\"3311\":[{\"5850\":1}]}");
		return 0;
	}
	catch(std::exception& e) {
		std::cerr << "EXCEPTION: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "EXCEPTION!" << std::endl;
	}
	return 1;
}
