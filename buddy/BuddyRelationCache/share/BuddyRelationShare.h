/*
 * =====================================================================================
 *
 *       Filename:  BuddyRelationShare.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年07月14日 15时50分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#ifndef __BUDDY_RELATION_SHARE_H__
#define __BUDDY_RELATION_SHARE_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "Singleton.h"
#include <QueryRunner.h>
#include <TaskManager.h>
#include "BuddyEntry.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"

namespace xce {
namespace buddy {

using namespace MyUtil;
using namespace com::xiaonei::xce;

const static string DB_BUDDY_FRIEND = "buddycore_friend";
//master database
const static string DB_BUDDY_APPLY = "buddycore_apply";
//slave database
const static string DB_BUDDY_APPLICANT = "buddyrelation_applicant_preloader";//"buddycore_applicant";
const static string DB_BUDDY_ACCEPTER = "buddyrelation_accepter_preloader";//"buddycore_accepter";

const static string DB_BUDDY_BLOCK = "buddycore_block";

class BuddyRelationCacheResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BuddyRelationCacheResultHandler(int userId, EntrySet& building,
			EntryList& duplicates, const std::string& toField,
			const uint32_t desc) :
		_userId(userId), _building(building), _duplicates(duplicates),
				_toField(toField), _desc(desc) {
	}
	virtual ~BuddyRelationCacheResultHandler() {
	}
private:
	virtual bool handle(mysqlpp::Row& row) const {
		int to = (int) row[_toField.c_str()];
		if (!_building.insert(Entry(to, _desc)).second) {
			_duplicates.push_back(Entry(to, _desc));
		}
		return true;
	}

	int _userId;
	EntrySet& _building;
	EntryList& _duplicates;
	std::string _toField;
	uint32_t _desc;
};

class BuddyRelationFactory : virtual public xce::generalpreloader::IdListFactory,
	virtual public MyUtil::Singleton<BuddyRelationFactory> {
public:
	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
		MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
		int emptyCount = 0;
		ostringstream emptyId;
		for( MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){
			BuddyRelationDataPtr obj = create(*idIt); 
			if(!obj->list.empty()) {
				res->data[*idIt] = obj;
			} else {
				emptyId << *idIt << " ";
				emptyCount ++ ;
			}
		}
		MCE_DEBUG("BuddyRelationFactory::create: emptyCount : " << emptyCount << " emptyIds: " << emptyId.str());
		MCE_INFO("BuddyRelationFactory::create: emptyCount : " << emptyCount << " ids.size(): " << ids.size());
		return res;
	}

	BuddyRelationDataPtr create(Ice::Int id) {
		MCE_DEBUG("BuddyRelationCacheFactory::create(" << id << ")");
		EntrySet building;
		EntryList duplicates;
		{
			Statement sql;
			sql << "SELECT guest FROM relation_block WHERE  host = '" << id << "'";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "guest",
					DESC_BLOCK);
			QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
		}

		{
			Statement sql;
			sql << "SELECT host FROM relation_block WHERE  guest = '" << id << "'";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "host",
					DESC_BLOCKED);
			QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
		}

		{
			ostringstream pattern;
			pattern << "relation_" << id % 100;
			Statement sql;
			sql << "SELECT guest FROM relation_" << id % 100 << " WHERE host = '"
				<< id << "' AND disc=0";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "guest",
					DESC_FRIEND);
			QueryRunner(DB_BUDDY_FRIEND, CDbRServer, pattern.str()).query(
					sql, handler);
		}
	
		{
			ostringstream pattern;
			pattern << "buddyapply_accepter_" << id % 100;
			Statement sql;
			sql << "SELECT applicant FROM buddyapply_accepter_" << id % 100 <<" WHERE accepter = '" << id
			<< "'";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "applicant",
			DESC_APPLIED);
			QueryRunner(DB_BUDDY_ACCEPTER, CDbRServer, pattern.str()).query(sql, handler);
		}

		{
			ostringstream pattern;
			pattern << "buddyapply_applicant_" << id % 100;
			Statement sql;
			sql << "SELECT accepter FROM buddyapply_applicant_" << id % 100 <<" WHERE applicant = '" << id
			<< "'";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "accepter",
			DESC_APPLY);
			QueryRunner(DB_BUDDY_APPLICANT, CDbRServer, pattern.str()).query(sql, handler);
		}
			
		/*
		{
			Statement sql;
			sql << "SELECT applicant FROM buddyapply WHERE accepter = '" << id
				<< "'";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "applicant",
					DESC_APPLIED);
			QueryRunner(DB_BUDDY_APPLY, CDbRServer).query(sql, handler);
		}

		{
			Statement sql;
			sql << "SELECT accepter FROM buddyapply WHERE applicant = '" << id
				<< "'";
			BuddyRelationCacheResultHandler handler(id, building, duplicates, "accepter",
					DESC_APPLY);
			QueryRunner(DB_BUDDY_APPLY, CDbRServer).query(sql, handler);
		}
		*/
		deduplicate(id, building, duplicates);

		// construct result object.
		ostringstream loster;
		ostringstream winner;
		int lostCount = 0;
		int winCount = 0;
		BuddyRelationDataPtr obj = new BuddyRelationData;
		for (EntrySet::iterator it = building.begin(); it != building.end(); ++it) {
			if ((uint32_t)id > it->to){
				RelationEntry entry;
				entry.id = it->to;
				entry.desc = BuddyDescHelper::translateDesc(it->desc);
				obj->list.push_back(entry);
				winner << it->to << " ";
				winCount ++;
			} else {
				loster << it->to << " ";
				lostCount ++;
			}
		}

		MCE_DEBUG("BuddyRelationFactory::create -> " << id << " done, loster: " << loster.str() << " winner: " << winner.str());
		MCE_INFO("BuddyRelationFactory::create -> " << id << " done, lostCount: " << lostCount << " winCount: " << winCount);
		return obj;
	}

	void deduplicate(int userId, EntrySet& building, EntryList& duplicates) {

		for (EntryList::iterator dupIt = duplicates.begin(); dupIt
				!= duplicates.end(); //++dupIt
		    ) {
			EntrySet::iterator buildingIt = building.find(*dupIt);
			if (buildingIt == building.end()) {
				//             This should not happend.
				MCE_WARN(
						"EntryListHolderFactory::create found a fake duplicate Entry. userId="
						<< userId << " " << *dupIt);
				++dupIt;
				continue;
			}

			if (dupIt->desc == DESC_BLOCKED) {
				if (buildingIt->desc == DESC_BLOCK) {
					building.erase(buildingIt);
					building.insert(Entry(dupIt->to, (DESC_BLOCK | DESC_BLOCKED)));
					dupIt = duplicates.erase(dupIt);
					continue;
				}
			}
			if (dupIt->desc == DESC_BLOCK) {
				if (buildingIt->desc == DESC_BLOCKED) {
					building.erase(buildingIt);
					building.insert(Entry(dupIt->to, (DESC_BLOCK | DESC_BLOCKED)));
					dupIt = duplicates.erase(dupIt);
					continue;
				}
			}
			++dupIt;
		}
	}
};

}
}

#endif

