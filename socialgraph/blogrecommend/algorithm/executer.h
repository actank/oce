#ifndef _BLOG_EXECUTER_H_
#define _BLOG_EXECUTER_H_

#include "socialgraph/blogrecommend/algorithm/method/blogprocess.h"

namespace xce {
namespace socialgraph {

/*@brief blog处理类，里面的instance是处理实例*/
class Executer {
public:
	Executer() {}
	~Executer() {}

	/* @brief 设置不同的处理实例
   * @param instance 处理实例
   */
	void SetInstance(BlogProcessPtr instance) {
		instance_ = instance;
	}

	/* @brief 处理blog*/
	void Process(long blogid);

	/* @brief 展示*/
	void Display();

	/* @brief 获取结果*/
	void Get(Items& result);

private:
	long blogid_;
	BlogProcessPtr instance_;
};

}
}

#endif
