/*
 * =====================================================================================
 *
 *       Filename:  UserContactReaderAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时02分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wenlong.zhang (ggh), wenlong.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#ifndef __USER_CONTACT_READER_ADAPTER_H__
#define __USER_CONTACT_READER_ADAPTER_H__

#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace usercontact{

using namespace ::xce::userdesc;
using namespace MyUtil;

class UserContactReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserContactReaderPrx>,
	public MyUtil::Singleton<UserContactReaderAdapter>
{
public:
	UserContactReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserContactReaderPrx>("ControllerUserDescReader",120,300,new MyUtil::OceChannel,true,"C"){

	}
	UserContactDataPtr getUserContact(int userId, const Ice::Context& ctx = Ice::Context());
	UserContactDataNPtr getUserContactN(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserContact(int userid, const MyUtil::Str2StrMap& properities);
	void setData(int userid, const Ice::ObjectPtr& data);
private:
	UserContactReaderPrx getUserContactReader(int userId);
};

}
}
}

#endif

