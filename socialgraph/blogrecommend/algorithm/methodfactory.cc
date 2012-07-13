#include "socialgraph/blogrecommend/algorithm/methodfactory.h"

#include "base/logging.h"
#include "socialgraph/blogrecommend/algorithm/method/impl/blogparticiple.h"

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

//根据不同的method标签创建不同的处理方法
BlogProcessPtr MethodFactory::Create(Method method) {
	switch (method) {
		case Participle:
			return BlogProcessPtr(new BlogParticiple());

		default:
			LOG(WARNING)	<< "[MethodFactory] no method found! method(" << method << ")";
			break;
	}
	
	return BlogProcessPtr();
}

}
}
