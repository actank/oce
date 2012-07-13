#ifndef SORT_STRATEGY_H_
#define SORT_STRATEGY_H_

#include "base/logging.h"
#include "socialgraph/blogrecommend/update/cxx/common.h"

namespace xce {
namespace socialgraph {

const static int THRESHOLD = 100; //cache中保存数据的上限，blog推荐的上限

//---------------------------------------------------------------

class SortStrategy {
public:
	SortStrategy() {}
	virtual ~SortStrategy() {}

	/*@brief 提供排序策略接口,根据具体需求进行排序
  * @param blog_map blog数据文件
  * @param tag_map 要生成返回结果
  */
	virtual void Sort(const std::map<long, BlogData>& blog_map, 
							 		  std::map<int, std::vector<long> >& tag_map) = 0;

	virtual void NewSort(std::map<int, std::vector<Blog> >& t_map) = 0;
};

}
}

#endif
