/**
 * @file
 * @author  guanghe.ge@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * BuddyRelationTTI.h：BuddyRelationTTManager
 */

#ifndef __BUDDY_RELATION_TT_I_H__
#define __BUDDY_RELATION_TT_I_H__

#include "ServiceI.h"
#include "Singleton.h"
#include "buddy/BuddyRelationCache/logic/BuddyRelationLogic.h"
#include "buddy/BuddyRelationCache/share/TokyoTyrantHelper.h"

namespace xce {
namespace buddy {

using namespace std;
using namespace xce::buddy;
using namespace com::xiaonei::xce; 

const static int CACHE_CLUSTER = 1;

/*****************************************************************************/


/**
  *  BuddyRelationTT服务主类，提供Buddy关系操作接口
  * 
  */
class BuddyRelationTTManagerI: virtual public BuddyRelationTTManager , virtual public MyUtil::Singleton<BuddyRelationTTManagerI> 
{
public:
	/**
	 *@param Relationship& --- 两人ID，包含from和to
	 *@param BuddyDesc --- 两人关系描述
	 */
	virtual BuddyDesc getRelation (const Relationship&, const Ice::Current& = Ice::Current());

	virtual void setRelation(const Relationship&,  BuddyDesc, const Ice::Current& =
			Ice::Current());

	virtual void addBuddyRelationData(Ice::Int, const BuddyRelationDataPtr&, const Ice::Current& =
			Ice::Current());

	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid,const Ice::Current& = Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& BuddyData, const Ice::Current& = Ice::Current());
private:
	bool _isValid;
	IceUtil::RWRecMutex _validMutex;
};

/*****************************************************************************/
class BuddyRelationTTLogic : virtual public BuddyRelationLogic , virtual public MyUtil::Singleton<BuddyRelationTTLogic>
{
public:
	BuddyRelationTTLogic() {
	    initialize();
	}
	void initialize();
	virtual EntryListHolderPtr readObject(uint32_t id);
	virtual void writeObject(uint32_t id, const EntryListHolderPtr&);
	virtual void writeObjects(const MyUtil::ObjectResultPtr&);
private:
	xce::tokyo::TokyoTyrantTranslatePtr _translate;
	xce::tokyo::TokyoTyrantHelperPtr _helper;
};


/*****************************************************************************/

class ServiceSchemaI: public MyUtil::Descriptor 
{
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};


}
}

#endif
