/*
 * =====================================================================================
 *
 *       Filename:  BuddyRelationLogic.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月15日 14时30分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "BuddyRelationLogic.h"
#include "buddy/BuddyRelationCache/share/BuddyRelationShare.h"

using namespace xce::buddy;

/*******************pubilc interface***********************/
BuddyDesc BuddyRelationLogic::getRelation(const Relationship& relationship) {
    if (relationship.from == relationship.to) {
        return ::Self;
    }
	
    Relationship relation = relationship;
    bool changed = operRelationship(relation);
		
    Entry key(relation.to, 0);
	
    EntryListHolderPtr holder = readObject(relation.from);
	
    if (holder) {
        EntryList entryResult = holder->get(key);
        if (entryResult.empty()) {
            MCE_DEBUG("BuddyRelationCacheManagerI::getRelation found empty.");
            return ::NoPath;
        }

        if (entryResult.size() != 1) {
            MCE_WARN("GetRelation should found only one entry. But found "
					<< entryResult.size() << " entries");
        }
        if(changed){
            return BuddyDescHelper::oppositeDesc(BuddyDescHelper::translateDesc(entryResult.at(0).desc));
        }else{
            return BuddyDescHelper::translateDesc(entryResult.at(0).desc);
        }
    } else {
        MCE_INFO("BuddyRelationCacheManagerI::getRelation, from: " << relation.from << " is not in cache");
    }
    return ::NoPath;
}

void BuddyRelationLogic::setRelation(const Relationship& relationship, 
		BuddyDesc buddydesc) {
    if (relationship.from == relationship.to) 
    {
        return;
    }

    Relationship relation = relationship;
    bool changed = operRelationship(relation);
    BuddyDesc desc = buddydesc;
    if(changed){
        desc = BuddyDescHelper::oppositeDesc(buddydesc);
    }

    EntryListHolderPtr holder = readObject(relation.from);
    if (holder) {
        if (desc == NoPath) {
            Entry entry;
            entry.to = relation.to;
            holder->remove(entry);
	    writeObject(relation.from, holder);
            return;
	}

        Entry key;
        key.to = relation.to;
        key.desc = BuddyDescHelper::translateInt(desc);
        holder->add(key);

	writeObject(relation.from, holder);
    } else {
        createRelation(relation, desc);
    }
}

void BuddyRelationLogic::addBuddyRelationData(Ice::Int id, 
		const BuddyRelationDataPtr& data) {
    EntryList list;
    ostringstream info;
    for (RelationEntryList::iterator en = data->list.begin(); en != data->list.end(); ++en) {
        Entry entry(en->id, BuddyDescHelper::translateInt(en->desc));
        list.push_back(entry);
        info << "<" << en->id << "/" << BuddyDescHelper::translateInt(en->desc) << "> ";
    }
    MCE_DEBUG("BuddyRelationLogic::addBuddyRelationData, id "<< id << " relations " << info.str());
    sort(list.begin(), list.end(), less_entry());
    EntryListHolderPtr holder = new EntryListHolder(id, list);

    writeObject(id, holder);
}

void BuddyRelationLogic::setData(const ::MyUtil::ObjectResultPtr& buddyData) {
	MCE_DEBUG("BuddyRelationLogic::setData, datasize " << buddyData->data.size());
	TaskManager::instance().execute(new FillTask(buddyData, this));
}
/*******************private interface***********************/
void BuddyRelationLogic::createRelation(const Relationship& relation,
		BuddyDesc desc) {
    BuddyRelationDataPtr data = BuddyRelationFactory::instance().create(relation.from);
    EntryList list;
    for(RelationEntryList::iterator en = data->list.begin(); en != data->list.end(); ++en) {
        Entry entry(en->id, BuddyDescHelper::translateInt(en->desc));
        list.push_back(entry);
    }
    sort(list.begin(), list.end(), less_entry());
    EntryListHolderPtr holder = new EntryListHolder(relation.from, list);

    Entry key;
    key.to = relation.to;
    key.desc = BuddyDescHelper::translateInt(desc);

    holder->add(key);

    writeObject(relation.from, holder);
}

bool BuddyRelationLogic::operRelationship(Relationship& relation) {
    bool changed = false;
    if(relation.from < relation.to){
        int id;
        id = relation.from;
        relation.from = relation.to;
        relation.to = id;
        changed = true;
    }
    return changed;
}
/************Tasks ***********************************************************/

void FillTask::handle() {
	ObjectResultPtr result = new ObjectResult();
	for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _buddyData->data.begin();
			it!= _buddyData->data.end(); ++it ){
		BuddyRelationDataPtr data = BuddyRelationDataPtr::dynamicCast(it->second);
		EntryList list;
		list.reserve(data->list.size());
		for(RelationEntryList::iterator en = data->list.begin(); en != data->list.end(); ++en) {
			Entry entry(en->id, BuddyDescHelper::translateInt(en->desc));
			list.push_back(entry);
		}
		sort(list.begin(), list.end(), less_entry());
		EntryListHolderPtr holder = new EntryListHolder(it->first, list);
		result->data[it->first] = holder;
	}
	
	_logic->writeObjects(result);
}



