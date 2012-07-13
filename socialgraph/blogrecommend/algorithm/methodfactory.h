#ifndef BLOG_METHODFACTORY_H_
#define BLOG_METHODFACTORY_H_

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "base/once.h"
#include "socialgraph/blogrecommend/algorithm/executer.h"
#include "socialgraph/blogrecommend/algorithm/method/blogprocess.h"

namespace xce {
namespace socialgraph {

/*@brief 用于blog分类的方法*/
enum Method {
	Participle
};

/*@brief blog分类方法的工厂类*/
class MethodFactory {
public:
	static MethodFactory& GetInstance();
	MethodFactory() {}
	virtual ~MethodFactory() {}
	
	/* @brief 创建处理blog实例 */
	BlogProcessPtr Create(Method method);
};

}
}

#endif
