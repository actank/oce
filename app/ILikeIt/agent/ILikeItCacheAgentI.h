#ifndef __ILIKEIT_CACHE_AGENT_I_H__
#define __ILIKEIT_CACHE_AGENT_I_H__

#include <ILikeIt.h>
#include <Ice/Ice.h>
#include "UtilCxx/src/Singleton.h"

namespace xce {
namespace ilikeit {

typedef std::map<long, LikeDataPtr> LikeDataMap;

// ============== ILikeItCacheAgentI主类 =============================
class ILikeItCacheAgentI : virtual public ILikeItCache,
		public MyUtil::Singleton<ILikeItCacheAgentI>  {
public:
	virtual ::Ice::Int getLikeCount(const GlobalIdPtr& gid, const Ice::Current& ctr=Ice::Current());

    	virtual ::MyUtil::IntSeq getLikeCache(const GlobalIdPtr& gid, const Ice::Current& ctr=Ice::Current());

	/* @brief 	获取按照查看的用户进行个性排序的顶过一个对象的用户列表
	 * @param	gid		对象的GlobalId
	 * @param	uid		查看该对象的用户id
	 * @param	limit		显示详情的用户数量
	 * @param	showStrangers	是否显示陌生人(非自己及好友)的详情
	 * @return	符合条件的用户资料信息包
	 */
    	virtual LikeInfoPtr getLikeDetail(const GlobalIdPtr& gid, ::Ice::Int uid, ::Ice::Int limit, bool showStrangers, const Ice::Current& ctr=Ice::Current());

	/* @brief	批量获取按照查看的用户进行个性排序的顶过一组对象的用户列表
	 * @param	gids		一组对象的gids列表
	 * @param	uid		查看该对象的用户id
	 * @param	limit		显示详情的用户数量
	 * @param	detailNum	按照limit数量显示用户资料的对象(gid)的条数(其他的无论limit是多少均不显示用户详情)
	 * @return      符合条件的用户资料信息包列表,排列顺序和gids相同
	 */
    	virtual LikeInfoSeq getLikeDetailBatch(const GlobalIdSeq& gids, ::Ice::Int uid, ::Ice::Int limit, ::Ice::Int detailNum, const Ice::Current& ctr=Ice::Current());

	virtual void load(const GlobalIdSeq& gids, const Ice::Current& ctr=Ice::Current());

	virtual void addLikeship(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());
	
	virtual void removeLikeship(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());

	void bootstrap(int cluster, int mod);

private:
	friend class MyUtil::Singleton<ILikeItCacheAgentI>;
	ILikeItCacheAgentI() {
	};

	bool belongsMe(const GlobalIdPtr& gid);
	int _cluster;
	int _mod;
};


};
};

#endif
