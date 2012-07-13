#include "socialgraph/blogrecommend/update/cxx/strategy/sort_by_tagweight.h"

namespace xce {
namespace socialgraph {

void SortByTagweight::Sort(const std::map<long, BlogData>& blog_map, 
          								 std::map<int, std::vector<long> >& tag_map) {
	LOG(INFO)	<< "[SortByTagweight] Sort blog_map size:" << blog_map.size();
	int count = 0;
	for (std::map<long, BlogData>::const_iterator iter = blog_map.begin();
			iter != blog_map.end(); ++iter) {
		if (++count % 1000 == 0) {
			LOG(INFO) << "[SortByTagweight] Sort count : " << count;
		}

		for (std::vector< std::pair<int, int> >::const_iterator it = iter->second.tag_list_.begin();
				it != iter->second.tag_list_.end(); ++it) {
			SortBlogData data;
			data.blog_id_ = iter->first;
			data.weight_ = it->second;

			sort_blog_map_[it->first].push_back(data);
		}
	}

	for (std::map<int, std::vector<SortBlogData> >::iterator iter = sort_blog_map_.begin();
			iter != sort_blog_map_.end(); ++iter) {
		std::sort(iter->second.begin(), iter->second.end(), BlogComp());
		int size = (int)iter->second.size() > THRESHOLD ? THRESHOLD : (int)iter->second.size();
		iter->second.resize(size);
	}
}

void SortByTagweight::NewSort(std::map<int, std::vector<Blog> >& t_map) {
	for (std::map<int, std::vector<Blog> >::iterator iter = t_map.begin();
			iter != t_map.end(); ++iter) {
		std::sort(iter->second.begin(), iter->second.end(), NewBlogComp());
	}

	for (std::map<int, std::vector<Blog> >::iterator iter = t_map.begin();
			iter != t_map.end(); ++iter) {
		iter->second.resize(BLOGSIZE);
	}
}

}
}
