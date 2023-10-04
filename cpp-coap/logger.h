//
//  logger.h
//  cpp-coap
//
//  Created by Piotr Brzeski on 2023-09-28.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>

template<class T>
void log(T const& message) {
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto now_seconds = std::chrono::system_clock::from_time_t(time);
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - now_seconds);
	std::tm tm = *std::localtime(&time);
	std::cout << std::put_time(&tm, "%F %T") << '.';
	if(milliseconds.count() < 100) {
		std::cout << '0';
		if(milliseconds.count() < 10) {
			std::cout << '0';
		}
	}
	std::cout << milliseconds.count() << ' ';
	std::cout << message << std::endl;
}
