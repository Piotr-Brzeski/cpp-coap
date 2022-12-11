//
//  main.cpp
//  server
//
//  Created by Piotr Brzeski on 2022-12-06.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include <cpp-coap/server.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto server = coap::server();
		server.add_endpoint(5683);
		server.add_handler("hello", [](){ return "world"; });
		server.run();
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
