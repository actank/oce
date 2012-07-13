/*
 * =====================================================================================
 *
 *       Filename:  WapOnlineAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年04月23日 11时27分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "WapOnlineAdapter.h"

using namespace com::xiaonei::wap::adapter;

MyUtil::Int2IntMap WapOnlineAdapter::getOnlineStatus(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("WapOnlineAdapter::getOnlineStatus,ids.size() " << ids.size());
	return online_service_prx_->getOnlineStatus(ids);
}
