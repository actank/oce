#ifndef _METHODFACTORY_H_
#define _METHODFACTORY_H_

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "base/once.h"
#include "socialgraph/autogroup/algorithm/method/autogroup.h"

namespace xce {
namespace socialgraph {

enum Method {
	GroupingWithFree, 
	GroupingWithExistGroup,
	GroupingWithLabel,
	NewGroupingWithLabel
};

class MethodFactory {
public:
	static MethodFactory& GetInstance();
	MethodFactory() {}
	virtual ~MethodFactory() {}

	AutoGroupPtr Create(Method method);
};

}
}

#endif
