#include "socialgraph/friendfinderworker/algorithm/commonfactory.h"
#include "socialgraph/friendfinderworker/algorithm/method/impl/kmergesort.h"
#include "socialgraph/friendfinderworker/algorithm/method/impl/hashmapsort.h"

namespace xce {
namespace socialgraph {

static CommonFactory* cfp = NULL;
static void InitCommonFactory() {
	cfp = new CommonFactory();
}

CommonFactory& CommonFactory::GetInstance() {
	static XCE_DECLARE_ONCE(once_guard_);
	XceOnceInit(&once_guard_, &InitCommonFactory);
	return *cfp;
}

CommonFriendCalculatorPtr CommonFactory::Create(Method method) {
	switch (method) {
		case KMerge:
			return CommonFriendCalculatorPtr(new KMergeSort());

		case HashMap:
			return CommonFriendCalculatorPtr(new HashMapSort());
		
		default:
			std::cout << "[CommonFactory] no method found! method(" << method << ")" << std::endl;
			break;
	}

	return CommonFriendCalculatorPtr();
}

}
}
