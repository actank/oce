/*
 * =====================================================================================
 *
 *       Filename:  BuddyRelationPreloaderI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年07月15日 14时51分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#ifndef __BUDDY_RELATION_PRELOADER_H__
#define __BUDDY_RELATION_PRELOADER_H__

#include <IceUtil/IceUtil.h>

namespace xce {
namespace buddy {

class BuddyRelationPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<BuddyRelationPreloader> {
public:
	BuddyRelationPreloader() {}
	virtual ~BuddyRelationPreloader() {}
};

}
}
#endif
