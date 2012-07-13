#ifndef SORT_BY_TAGWEIGHT_H_
#define SORT_BY_TAGWEIGHT_H_

#include "socialgraph/blogrecommend/update/cxx/sort_strategy.h"

namespace xce {
namespace socialgraph {

struct SortBlogData {
	SortBlogData() {
		blog_id_ = 0;
		weight_ = 0;
	}

	long blog_id_;
	long weight_;
};

//--------------------------------------------------------------------

struct BlogComp {
	bool operator() (const SortBlogData& s1, const SortBlogData& s2) {
		return s1.weight_ > s2.weight_;
	}
};

//--------------------------------------------------------------------

struct NewBlogComp {
	bool operator() (const Blog& s1, const Blog& s2) {
		return s1.weight_ > s2.weight_;
	}
};

//--------------------------------------------------------------------

static const int BLOGSIZE = 3;

//--------------------------------------------------------------------

class SortByTagweight : public SortStrategy {
public:
	SortByTagweight() {}
	virtual ~SortByTagweight() {}

/*@brief 按照tag对应weight值进行排序
  * @param blog_map blog数据文件
  * @param tag_map 要生成返回结果
  */   
  void Sort(const std::map<long, BlogData>& blog_map, 
            std::map<int, std::vector<long> >& tag_map);


	void NewSort(std::map<int, std::vector<Blog> >& t_map);

private:
	std::map<int, std::vector<SortBlogData> > sort_blog_map_;
};

}
}

#endif
