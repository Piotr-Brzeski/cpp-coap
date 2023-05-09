//
//  coap-client.cpp
//  coap-client
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#include <cpp-coap/client.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto client = coap::client();
		auto session = client.create_session("127.0.0.1", 5683);
		auto response = session.get("hello");
		std::cout << "Response: '" << response << "'" << std::endl;
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
