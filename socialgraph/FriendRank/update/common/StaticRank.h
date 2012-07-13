#ifndef _STATIC_RANK_H_
#define _STATIC_RANK_H_

#include "Common.h"

// class StaticRankHead {{{
class StaticRankHead
{
	public:
		StaticRankHead() {
			Init(0, 0);
		}
		~StaticRankHead() {}

		void Init(int uid, int num) {
			this->uid = uid;
			this->num = num;
		}
	public:
		int uid;
		int num;
};// }}}

class StaticRank
{
	public:
		StaticRank(USERID fid=0);
		~StaticRank();
		void Init(USERID fid);
		void Print() const;
		StaticRank& operator=(const StaticRank& other);

		//float GetScore() const;
		float GetScore(string& reason) const;
		USERID GetFid() const {
			return fid;
		}

	public:
		USERID  fid;             // friend id
		char    diffBirth;       // 年龄差别
		bool    diffGender;      // 是否异性

		short   friends[3];      // [我的好友数][她的好友数][共同好友数]
		float   commonInfors;    // 学校、公司相同个数
		float   scoreGroup;      // 用户分组（大学同学、挚交好友、家人亲戚）
		bool    isInvited;       // 是否被邀请
		bool    isSpecial;       // 是否特别好友
		bool    isGiftReceiver;  // 我是否送过礼物
		char    feedconfig;      // 新鲜事设置名单：+1白名单，-1黑名单，0不在黑白名单中
};

//struct Comp_ByStaticRank
//{
//	bool operator()(const StaticRank& p1, const StaticRank& p2) {
//		return p1.GetScore() > p2.GetScore();
//	}
//};

#endif

