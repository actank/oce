#ifndef __TIPS_ADAPTER_H__
#define __TIPS_ADAPTER_H__

#include "Tips.h"
#include "UtilCxx/src/Singleton.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace tps {

class TipsAdapter : public MyUtil::Singleton<TipsAdapter> { 
public:
	/* @brief 在一个域给一个用户推荐一条公告
	 */
	TipsPtr show(int uid, int website);

	/* @brief 以公告id取出公告
	 */
	TipsPtr get(int tipsid);

	/* @brief 显示所有公告
	 */
	TipsSeq list();

	/* @brief 从DB中更新公告列表
	 * @param rebuildIndex	是否需要重建索引
	 */
	void renew(bool rebuildIndex);

	/* @brief update tips's bitset, whose tipsid = tpsId
	 *
	*/
	void reloadTipsInterest(int tpsId);

	/* @brief 更新针对用户的cache
	 */
	void relocateUserCache(int userid);

	/* @brief 用户点击或关闭一条公告
	 */
	void click(int uid, int tipsid, bool beClose);

private:
	/* @brief ClusterState的接口 */
	xce::clusterstate::ClientInterface<TipsManagerPrx> _clientCS;	

	friend class MyUtil::Singleton<TipsAdapter>;
	TipsAdapter():
		_clientCS("ControllerTips", 120, 300) {
		_clientCS.intialize();
	};
};

};
};

#endif
