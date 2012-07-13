#ifndef COMMONDBHELPER_H_
#define COMMONDBHELPER_H_

#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"

namespace xce {
namespace socialgraph {

const static int EMPTY_ID = -999;		//如果要删除指点的推荐好友，则将该好友的friend_id设置为该值
const static int CF_SAVE_THRESHOLD_DB = 100;		//db中存储100条记录
const static size_t SAVE_DATA_LENGTH = 16;   //数据库shares字段长度为255,所以这里最多存63个int

//---------------------------------------------------------------------------------------------------------------

class CommonDbHelper {
public:
	/* @brief 从db中获取commons friends数据
   * @param hostid 用户id 
   * @param comms 返回的结果值
   */
	static void GetCommonsFromDb(int hostid, CommonFriendSeq& comms);
	
	/* @brief 向db中插入commons friends数据
   * @param hostid 用户id 
   * @param comms 插入的数据集合
   */
	static void SetCommonsToDb(int hostid, const CommonFriendSeq& comms);

	/* @brief 从db中删除commons friends数据,并加入block表
   * @param hostid 用户id 
   * @param friendid 删除id
   */
	static void RemoveCommonsFromDb(int hostid, int friendid);

	/* @brief 获取最近一天登录的用户id
   * @param userid_list 存储最近一天登录的用户id
   */
	static void GetRecentlyloginUserId(std::vector<int>& userid_list);

	/* @brief 获取用户数据的创建时间
   * @param hostid 用户id
   */
	static long GetCreateDataTime(int hostid);
};

//---------------------------------------------------------------------------------------------------------------

/* @brief common friends 数据库handler类 */
class CommonFriendSetResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	CommonFriendSetResultHandler(CommonFriendSeq& results) : results_(results) { }
	virtual ~CommonFriendSetResultHandler() {}

protected:
	virtual bool handle(mysqlpp::Row& row) const;	

private:
	CommonFriendSeq& results_;
};

//---------------------------------------------------------------------------------------------------------------

/* @brief 获取最近一天登录的用户id 数据库handler类*/
class UsertimeBatchReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UsertimeBatchReadHandler(int& last, std::vector<int>& idseq): last_(last), idseq_(idseq) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& last_;
	std::vector<int>& idseq_; 
};

}
}

#endif
