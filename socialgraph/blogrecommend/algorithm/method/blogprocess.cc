#include "socialgraph/blogrecommend/algorithm/method/blogprocess.h"

#include <iostream>
#include <sstream>
#include <string>

#include "base/logging.h"
#include "base/ptime.h"

namespace xce {
namespace socialgraph {

//主函数，需要子类实现其中的某些函数
void BlogProcess::Process(long blogid) {
	std::ostringstream oss;
	oss << "[BlogProcess] Process blogid:" << blogid;
	PTIME(pt, oss.str(), true, 0);	

	Initialize(blogid);

	if (Hook()) {
		Create();	//处理blog
	}
}

//初始化操作
void BlogProcess::Initialize(long blogid) {
	blogid_ = blogid;
}

}
}
