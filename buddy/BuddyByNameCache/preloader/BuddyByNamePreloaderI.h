/*
 * =====================================================================================
 *
 *       Filename:  BuddyByNamePreloaderI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年05月26日 16时46分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDYBYNAMEPRELOADER_H__
#define __BUDDYBYNAMEPRELOADER_H__

#include <IceUtil/IceUtil.h>

namespace xce {
namespace buddybynamepreloader {

class BuddyByNamePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<BuddyByNamePreloader> {
	public:
		BuddyByNamePreloader() {}
		virtual ~BuddyByNamePreloader() {}
};

//------------------------------------------------------------------------------

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}
}
#endif
