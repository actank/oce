#ifndef __BULLETIN_ICE__
#define __BULLETIN_ICE__

#include <Util.ice>

module xce {
module blt {

const int RENREN = 1;
const int KAIXIN = 0;
const int GUIDE = 2;
const int MALE = 1;
const int FEMALE = 0;
const int STARYES = 1;
const int STARNO = 0;
const int VIPYES = 1;
const int VIPNO = 0;
const int USERCONTACT = 0;
const int USERFAVOR = 1;
const int USERUNIV = 2;
const int USERCAREER = 3;
const int ALL = -1;
const bool REBUILDINDEX = true;
const bool NOREBUILDINDEX = false;

const string CTXUSERSTAGE = "userstage";
const int STAGENONE = 0;
const int STAGEMIDDLESCHOOL = 1;
const int STAGECOLLAGE = 2;
const int STAGEWHITECOLLAR = 3;


/**
 *	公告适应年龄范围
 **/
class AgeRange {
	int minAge;
	int maxAge;
};

/** 
 * 	一组允许不连续的年龄范围组合
 */
sequence<AgeRange> AgeScope;

/**
 *	公告上线时间范围
 */
class TimeRange {
	int beginTime;
	int endTime;
};

/**
 *	一组允许不连续的上线时间范围组合
 */
sequence<TimeRange> TimeScope;

/**
 *  [真.公告信息.改] 加入通用的、可自定义扩展的过滤条件
 */
class Bulletin {
	int bulletinid;		// 公告id
	string title;		// 公告标题
	string text;		// 正文
	int website;		// 发布网站:1-人人;0-开心;        2-guide.renren, -1-all the three 
	int star;		// 星级用户? 1-是;0-非;-1-皆可
	int vip;		// VIP？1-是;0-非;-1-皆可	
	MyUtil::IntSeq completes; 	// 已完整的资料项
	MyUtil::IntSeq uncompletes;	// 未完整的资料项
	int gender;		// 性别;1-男;0-女;-1-不限制
	AgeScope ages;		// 年龄范围
	MyUtil::IntSeq idmods;	// 用户ID尾号
	MyUtil::IntSeq idgiven;	// 专属给某个id(多用于测试/预览)
	TimeScope onlinetime;	// 上线的时间范围
	MyUtil::IntSeq appuse;	// 安装的app
	MyUtil::IntSeq appunuse;// 未安装的app
	MyUtil::Str2IntSeqMap ctxs; 	// 其他自定义过滤条件 

	int weight;		// 公告权重
	bool persist;		// 是否点击后不消失
	int maxshow;		// 最多被显示的次数; -1为不限制

	bool online;		// 公告否在线上
};
sequence<Bulletin> BulletinSeq;


/**
 *	公告推荐及查看服务接口
 */
class BulletinManager {

	/**
	 * 在一个网站给一个用户推荐一条公告
  	*/
	Bulletin show(int uid, int website);


	/**
	 * 以公告id取出公告
 	*/
	Bulletin get(int bulletinid);

	/**
	 * 从DB中更新公告列表
	 */
	void renew(bool rebuildIndex);

	/**
	 *	更新针对用户的cache
	 */
	void relocateUserCache(int uid);
	

	/**
	 *	强制更新一条公告的指定用户列表
	 */	
	void reloadBulletinInterest(int bltId, bool force);

	/**
	 * 显示所有公告[旧接口]  
	*/
	BulletinSeq list();
	
	/** 
	 * 	用户点击或关闭一条公告
	 */
	void click(int uid, int bulletinid, bool beClose);

};

/**
 *	公告操作服务接口
 */
class BulletinAdmin {

	/**
	 *	添加公告
	 */
	void add(Bulletin blt);

	/**
	 *	删除公告
	 */
	void remove(int bulletinid);

	/**
	 *	修改公告
	 */	
	void modify(int bulletinid, Bulletin blt);

	/**
	 *	公告上线
	 */
	void online(int bulletinid);

	/**
	 *	公告下线
	 */
	void offline(int bulletinid);
};

};
};

#endif
