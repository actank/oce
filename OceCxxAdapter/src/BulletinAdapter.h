/*
 * BulletinAdapter.h
 *
 *  Created on: 2009-11-02
 *      Author: zhanghan
 *  Modified(add function reloadBulletinInterest()) on: 2010-08-13
 *      Author: shangmin
 */

#ifndef __BULLETIN_ADAPTER_H__
#define __BULLETIN_ADAPTER_H__

#include "Bulletin.h"
#include "UtilCxx/src/Singleton.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace blt {

class BulletinAdapter : public MyUtil::Singleton<BulletinAdapter> { 
public:
	/* @brief 在一个网站给一个用户推荐一条公告
	 */
	BulletinPtr show(int uid, int website);

	/* @brief 以公告id取出公告
	 */
	BulletinPtr get(int bulletinid);

	/* @brief 显示所有公告
	 */
	BulletinSeq list();

	/* @brief 从DB中更新公告列表
	 * @param rebuildIndex	是否需要重建索引
	 */
	void renew(bool rebuildIndex);

	/* @brief update bulletin's bitset, whose bulletinid = bltId
	 *
	*/
	void reloadBulletinInterest(int bltId);

	/* @brief 更新针对用户的cache
	 */
	void relocateUserCache(int userid);

	/* @brief 用户点击或关闭一条公告
	 */
	void click(int uid, int bulletinid, bool beClose);

private:
	/* @brief ClusterState的接口 */
	xce::clusterstate::ClientInterface<BulletinManagerPrx> _clientCS;	

	friend class MyUtil::Singleton<BulletinAdapter>;
	BulletinAdapter():
		_clientCS("ControllerBulletinN", 120, 300) {
		_clientCS.intialize();
	};
};

};
};

#endif
