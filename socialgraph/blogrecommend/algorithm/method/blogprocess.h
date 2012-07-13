#ifndef BLOG_PROCESS_H_
#define BLOG_PROCESS_H_

#include <boost/shared_ptr.hpp>
#include "item.pb.h"

namespace xce {
namespace socialgraph {

/*@brief 处理blog的父类*/
class BlogProcess {
public:
	BlogProcess() {}
	virtual ~BlogProcess() {}

	//处理相应的blog
	void Process(long blogid);

	//由子类实现，用于展示结果
	virtual void Display() = 0;

	//由子类实现，用于获取result, Items用于规划统一结构
	virtual void Get(Items& result) = 0;

protected:
	//由子类实现的函数，用于处理相应的blog
	virtual void Create() = 0;

	virtual bool Hook() {
		return true;
	}

	long blogid_;

private:
	void Initialize(long blogid);
};

typedef boost::shared_ptr<BlogProcess> BlogProcessPtr;

}
}

#endif
