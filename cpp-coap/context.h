//
//  context.h
//  cpp-coap
//
//  Created by Piotr Brzeski on 2022-12-10.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include <string>

struct coap_context_t;

namespace coap {

class context {
public:
	enum class type { UDP, DTLS };
	
	context();
	~context();
	
	operator ::coap_context_t*() {
		return m_context;
	}
	
protected:
	::coap_context_t* m_context;
};

}
