#ifndef BLOGRECOMMEND_H_
#define BLOGRECOMMEND_H_

#include <string>

#include "socialgraph/socialgraphutil/calculator/calculator.h"

#include "item.pb.h"  //临时放这
#include "socialgraph/blogrecommend/update/cxx/common.h"

namespace xce {
namespace socialgraph {

const static std::string TAG = "BLOGTAG";
//----------------------------------------------

class BlogRecommend : public Calculator {
public:
	BlogRecommend(){}
	~BlogRecommend() {}

	virtual void Detail();

private:
	/*@brief 从文件加载blog数据进内存 
  * @param file blog数据文件
  */
	void LoadBlogData(const std::string& file);

	/*@brief 根据内存中的blog信息生成tag_map, 根据产品需求调整不同策略*/
	void CreateTagMap(const std::string& file);

	void SortTagMap();

	/*@brief SetBlogData*/
	void SetBlogData();

	/*@brief SetBlogTag*/
	void SetBlogTag();

	/*@brief FormatTime*/
	long FormatTime(const std::string& date, const std::string& clock);

	void Print(const ::xce::socialgraph::Items& target);
	void PrintTagMap(const std::map<int, std::vector<Blog> >& t_map);

	std::map<int, std::vector<Blog> > t_map_;
	//std::map<long, Blog> b_map_;		//在内存中存放blog数据 ？？不知道能不能放下 blog_id -> blog数据
	//std::map<int, std::vector<long> > t_map_; //在内存中存放blog类型对应该类型的全部blog_id type -> blog_id list

	std::map<long, BlogData> blog_map_; //在内存中存放blog数据 ？？不知道能不能放下 blog_id -> blog数据
	std::map<int, std::vector<long> > tag_map_; //在内存中存放blog类型对应该类型的全部blog_id type -> blog_id list
};

}
}

#endif
