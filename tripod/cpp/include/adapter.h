/*
 * adapter.h
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#ifndef ADAPTER_H_
#define ADAPTER_H_

#include "communicator.h"
#include "data.h"
#include <iostream>

namespace com {
namespace renren {
namespace tripod {

class Adapter {
public:
//	Adapter(const std::string& name, const std::string& registry_addr,
//			RegistryFactoryPtr registry_factory) :
//		name_(name), registry_addr_(registry_addr_), registry_factory_(
//				registry_factory) {
////		registry_ = registry_factory_ -> Create();
//	}
	~Adapter() {
	}

	void Initialize();

	DataPtr Read(const std::string& key);
	bool Write(const std::string& key, DataPtr data, bool exist = false,
			bool sync = true);
private:
	std::string name_;
	std::string registry_addr_;
//	RegistryFactoryPtr registry_factory_;
//	RegistryPtr registry_;
};// end Adapter
}
}
}

#endif /* ADAPTER_H_ */
