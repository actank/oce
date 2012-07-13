#ifndef BLOG_RECOMMEND_COMMON_H_
#define BLOG_RECOMMEND_COMMON_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace xce {
namespace socialgraph {

static const std::string BLOG = "BLOG";
static const std::string BLOGTAG = "BLOGTAG";

struct BlogData {
	BlogData() {
		share_id_ = 0;
		blog_id_ = 0;
		host_id_ = 0;
		share_u_id_ = 0;
		time_ = 0;
		score_ = 0;
	}
	long share_id_;
	long blog_id_;
	int host_id_;
	long share_u_id_;
	long time_;
	int score_;
	std::vector< std::pair<int, int> > tag_list_;
};

struct Blog {
	Blog() {
		share_id_ = 0;
		blog_id_ = 0;
		host_id_ = 0;
		time_ = 0;
		count_ = 0;
		first_share_id_ = 0;
		weight_ = 0;
		title_ = "";
		content_ = "";
		weight_str_ = "";
	}

	long share_id_;		//分享id
	long blog_id_;		//blog id
	int host_id_;		//该blog的拥有者id
	long time_;		//blog的被分享时间
	int count_;		//blog被分享的次数
	int first_share_id_;		//第一次分享的用户
	float weight_;		//属于当前type的权重，临时值，同一个blog对不同type有不同的weight值
	std::string title_;		//blog的title
	std::string content_;			//blog的内容
	std::string weight_str_;		//blog的weight序列
};

}
}

#endif
