/*
 * =====================================================================================
 *
 *       Filename:  BuddyRelationLogic.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月15日 13时57分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDY_RELATION_LOGIC_H__
#define __BUDDY_RELATION_LOGIC_H__

#include <BuddyRelationCache.h>
#include <IceUtil/IceUtil.h>
#include <BuddyEntry.h>

namespace xce {
namespace buddy {

static const uint32_t TASK_LEVEL_FILL = 1;

class BuddyRelationLogic : virtual public IceUtil::Shared {
public:
    virtual ~BuddyRelationLogic() {}

    virtual EntryListHolderPtr readObject(uint32_t id) = 0;
    virtual void writeObject(uint32_t id, const EntryListHolderPtr& obj) = 0;
    virtual void writeObjects(const MyUtil::ObjectResultPtr& result) = 0;

    BuddyDesc getRelation(const Relationship&);
    void setRelation(const Relationship&, BuddyDesc);

    void addBuddyRelationData(Ice::Int, const BuddyRelationDataPtr&);
    void setData(const MyUtil::ObjectResultPtr&);
private:
    void createRelation(const Relationship&, BuddyDesc);
    bool operRelationship(Relationship& relation);
};
typedef IceUtil::Handle<BuddyRelationLogic> BuddyRelationLogicPtr;
/*****************************************************************************/
class FillTask: virtual public MyUtil::Task {
public:
        FillTask(const MyUtil::ObjectResultPtr& buddyData, BuddyRelationLogic*  logic, ::Ice::Int level=TASK_LEVEL_FILL):
                MyUtil::Task(level), _buddyData(buddyData), _logic(logic) {
        }
        virtual void handle();
private:
        MyUtil::ObjectResultPtr _buddyData;
	BuddyRelationLogic* _logic;
};


}
}
#endif
