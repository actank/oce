/**
 * @file
 * @author  guanghe.ge@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * BuddyRelationCacheI.h：BuddyRelationCacheManager
 */

#ifndef __BUDDY_RELATION_CACHE_I_H__
#define __BUDDY_RELATION_CACHE_I_H__

#include "ServiceI.h"
#include "Singleton.h"
#include "buddy/BuddyRelationCache/logic/BuddyRelationLogic.h"
#include "buddy/BuddyRelationCache/share/TokyoTyrantHelper.h"

namespace xce {
namespace buddy {

using namespace std;
using namespace xce::buddy;
using namespace com::xiaonei::xce; 

static const int BUDDY_RELATION_CACHE = 0;

static const bool IS_USE_TT = false;

/*****************************************************************************/


/**
  *  BuddyRelationCache服务主类，提供Buddy关系操作接口
  * 
  */
class BuddyRelationCacheManagerI: virtual public BuddyRelationCacheManager , virtual public MyUtil::Singleton<BuddyRelationCacheManagerI> 
{
public:
	/**
	 *@param Relationship& --- 两人ID，包含from和to
	 *@return BuddyDesc --- 两人关系描述
	 */
	virtual BuddyDesc getRelation (const Relationship&, const Ice::Current& =
			Ice::Current());
	RelationshipEntryList getRelationBatch(const MyUtil::Int2IntMap& ids,  const Ice::Current& =	Ice::Current());
	/**
	 *@param Relationship& --- 两人ID，包含from和to
	 *@param BuddyDesc --- 两人关系描述
	 */
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
class BuddyRelationCacheLogic : virtual public BuddyRelationLogic , virtual public MyUtil::Singleton<BuddyRelationCacheLogic>
{
public:
	virtual EntryListHolderPtr readObject(uint32_t id);
	virtual void writeObject(uint32_t id, const EntryListHolderPtr&);
	virtual void writeObjects(const MyUtil::ObjectResultPtr&);
};

/*****************************************************************************/
class BuddyRelationCacheCaller : public MyUtil::ServantFactory, virtual public MyUtil::Singleton<BuddyRelationCacheCaller>
{
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

/*****************************************************************************/

class ServiceSchemaI: public MyUtil::Descriptor 
{
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};
/*****************************************************************************/
class TokyoTyrantRelationLoader : public xce::tokyo::TokyoTyrantLoader {
public:
	TokyoTyrantRelationLoader(xce::tokyo::TokyoTyrantTranslatePtr translate) : 
		xce::tokyo::TokyoTyrantLoader(translate) {}
	virtual void addObject(uint32_t id, Ice::ObjectPtr obj);
};


}
}

#endif
