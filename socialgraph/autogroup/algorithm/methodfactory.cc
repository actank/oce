#include "socialgraph/autogroup/algorithm/methodfactory.h"

#include "socialgraph/autogroup/algorithm/method/impl/autogroupwithfree.h"
#include "socialgraph/autogroup/algorithm/method/impl/autogroupwithexistgroup.h"
#include "socialgraph/autogroup/algorithm/method/impl/autogroupwithlabel.h"
#include "socialgraph/autogroup/algorithm/method/impl/newautogroupwithlabel.h"

namespace xce {
namespace socialgraph {

static MethodFactory* mf = NULL;
static void InitMethodFactory() {
	mf = new MethodFactory();
}

MethodFactory& MethodFactory::GetInstance() {
	static XCE_DECLARE_ONCE(once_guard_);
	XceOnceInit(&once_guard_, &InitMethodFactory);
	return *mf;
}

//根据不同的method标签创建不同的分组方法
AutoGroupPtr MethodFactory::Create(Method method) {
	switch (method) {
		case GroupingWithFree:
			return AutoGroupPtr(new AutoGroupWithFree());

		case GroupingWithExistGroup:
			return AutoGroupPtr(new AutoGroupWithExistGroup());

		case GroupingWithLabel:
			return AutoGroupPtr(new autogroupwithlabel::AutoGroupWithLabel());

		case NewGroupingWithLabel:
			return AutoGroupPtr(new newautogroupwithlabel::NewAutoGroupWithLabel());

		default:
			std::cout << "[MethodFactory] no method found! method(" << method << ")" << std::endl;
			break;
	}

	return AutoGroupPtr();
}

}
}
