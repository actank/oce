/*
 * =====================================================================================
 *
 *       Filename:  FeedItemFacadeI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年10月27日 18时46分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FEEDITEMFACADE_I_H__
#define __FEEDITEMFACADE_I_H__

#include <UserFeeds.h>
#include "Singleton.h"

namespace xce {
namespace feed {

class FeedItemFacadeI : public FeedItemFacade, public MyUtil::Singleton<FeedItemFacadeI> {
public:
	FeedItemFacadeI();
	virtual ~FeedItemFacadeI();

	virtual void put(const FeedSeedPtr& seed, const ::Ice::Current& = ::Ice::Current());
	virtual void del(const FeedMeta& meta, const ::Ice::Current& = ::Ice::Current());
};

}
}

#endif
