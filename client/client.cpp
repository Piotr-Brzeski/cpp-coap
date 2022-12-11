//
//  client.cpp
//  client
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include <cpp-coap/context.h>
#include <cpp-coap/session.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto context = coap::context();
		auto session = coap::session(context, "127.0.0.1", 5683);
		auto response = session.send("hello");
		std::cout << "Response: [" << response << "]" << std::endl;
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
