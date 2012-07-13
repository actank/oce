/*
 * key.h
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#ifndef KEY_H_
#define KEY_H_
#include <iostream>

namespace com {
namespace renren {
namespace tripod {

class Key {
public:
	static std::string GenerateKey(const std::string& name, int block, long id) {
		std::ostringstream oss;
		oss << name << "_" << block << "_" << id;
		return oss.str();
	}

	static std::string ParseName(const std::string& key) {

	}

	static int ParseBlock(const std::string& key) {

	}

	static int ParseId(const std::string& key) {

	}
};// end Key
}
}
}

#endif /* KEY_H_ */
