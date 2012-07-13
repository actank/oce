#ifndef BLOG_PARTICIPLE_H_
#define BLOG_PARTICIPLE_H_

#include "socialgraph/blogrecommend/algorithm/method/blogprocess.h"

namespace xce {
namespace socialgraph {

class BlogParticiple: public BlogProcess {
public:
	BlogParticiple() {}
	virtual ~BlogParticiple() {}	

	/* @brief 处理blog*/
	void Create();

	/* @brief 获取处理后的结果 */
	void Get(Items& result);

	/* @brief 展示处理后的结果 */
	void Display();

private:

};

}
}

#endif
