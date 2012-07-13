/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantPreloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月28日 11时57分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDY_BYAPPLICANT_PRELOADER_H__
#define __BUDDY_BYAPPLICANT_PRELOADER_H__

#include <IceUtil/IceUtil.h>

namespace xce {
namespace buddy {

class BuddyByApplicantPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<BuddyByApplicantPreloader> {
public:
	BuddyByApplicantPreloader() {}
	virtual ~BuddyByApplicantPreloader() {}
};

}
}
#endif
