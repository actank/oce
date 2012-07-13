#include "ActiveMotionFacadeAdapter.h"

using namespace MyUtil;
using namespace xce::activetrack;
using namespace xce::activetrack::adapter;

void ActiveMotionFacadeAdapter::motion(const MyUtil::StrSeq& params ) {
	getProxy(0)->motion(params);
}

