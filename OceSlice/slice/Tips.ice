#ifndef __TIPS_ICE__
#define __TIPS_ICE__

#include <Util.ice>

module xce {
module tps {

const int RENREN = 0;
const int XIAONEI = 1;
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
 *	提示适应年龄范围
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
 *	提示上线时间范围
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
 *  [真.提示信息.改] 加入通用的、可自定义扩展的过滤条件
 */
class Tips {
	int tipsid;		// 提示id
	string title;		// 提示标题
	string text;		// 正文
	int website;		// 域 0-人人; 1-校内; 2-guide.renren; -1-all the three 
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

	int weight;		// 提示权重
	bool persist;		// 是否点击后不消失
	int maxshow;		// 最多被显示的次数; -1为不限制

	bool online;		// 提示是否在线上
};
sequence<Tips> TipsSeq;


/**
 *	提示推荐及查看服务接口
 */
class TipsManager {

	/**
	 * 在一个域给一个用户推荐一条提示
  	*/
	Tips show(int uid, int website);


	/**
	 * 以提示id取出提示
 	*/
	Tips get(int tipsid);

	/**
	 * 从DB中更新提示列表
	 */
	void renew(bool rebuildIndex);

	/**
	 *	更新针对用户的cache
	 */
	void relocateUserCache(int uid);
	

	/**
	 *	强制更新一条提示的指定用户列表
	 */	
	void reloadTipsInterest(int tpsId, bool force);

	/**
	 * 显示所有提示[旧接口]  
	*/
	TipsSeq list();
	
	/** 
	 * 	用户点击或关闭一条提示
	 */
	void click(int uid, int tipsid, bool beClose);

};

/**
 *	提示操作服务接口
 */
class TipsAdmin {

	/**
	 *	添加提示
	 */
	void add(Tips tps);

	/**
	 *	删除提示
	 */
	void remove(int tipsid);

	/**
	 *	修改提示
	 */	
	void modify(int tipsid, Tips tps);

	/**
	 *	提示上线
	 */
	void online(int tipsid);

	/**
	 *	提示下线
	 */
	void offline(int tipsid);
};

};
};

#endif
