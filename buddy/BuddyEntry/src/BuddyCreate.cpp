#include "BuddyCreate.h"

using namespace std;
using namespace xce::buddy;
using namespace MyUtil;
using namespace com::xiaonei::xce;
/*********************************************************************************************/
Ice::ObjectPtr BuddyRelationFactory::create(Ice::Int id) {
        MCE_INFO("create -> " << id);
        set<int> building[8];
	//building[0] used for global set to create exclusive id
        BuddyRelationPtr duplicates = new BuddyRelation();
        {
                Statement sql;
                sql << "SELECT guest FROM relation_block WHERE  host = '" << id << "'";
                BuddyResultHandler handler(id, building[0], building[Block], duplicates->block, "guest");
                QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
        }

        {
                Statement sql;
                sql << "SELECT host FROM relation_block WHERE  guest = '" << id << "'";
                BuddyResultHandler handler(id, building[0], building[Blocked], duplicates->blocked, "host");
                QueryRunner(DB_BUDDY_BLOCK, CDbRServer).query(sql, handler);
        }

        {
                ostringstream pattern;
                pattern << "relation_" << abs(id % 100);
                Statement sql;
                sql << "SELECT guest FROM relation_" << abs(id % 100) << " WHERE host = '"
                                << id << "' AND disc=0";
                BuddyResultHandler handler(id, building[0], building[Friend], duplicates->friends, "guest");
                QueryRunner(DB_BUDDY_FRIEND, CDbRServer, pattern.str()).query(sql, handler);
        }

        {
		ostringstream pattern;
                pattern << "buddyapply_accepter_" << abs(id % 100);
                Statement sql;
                sql << "SELECT applicant FROM buddyapply_accepter_" << abs(id % 100) <<" WHERE accepter = '" << id
                                << "'";
                BuddyResultHandler handler(id, building[0], building[Applied], duplicates->applied, "applicant");
        	QueryRunner(DB_BUDDY_ACCEPTER, CDbRServer, pattern.str()).query(sql, handler);
	}

        {
		ostringstream pattern;
                pattern << "buddyapply_applicant_" << abs(id % 100);
                Statement sql;
                sql << "SELECT accepter FROM buddyapply_applicant_" << abs(id % 100) <<" WHERE applicant = '" << id
                                << "'";
                BuddyResultHandler handler(id, building[0], building[Apply], duplicates->apply, "accepter");
                QueryRunner(DB_BUDDY_APPLICANT, CDbRServer,pattern.str()).query(sql, handler);
        }
	{
                //build biblock list
                MyUtil::IntSeq tmp(16);
                MyUtil::IntSeq::iterator tmpit;
                tmpit=set_intersection(building[Block].begin(), building[Block].end(), 
				building[Blocked].begin(), building[Blocked].end(), tmp.begin());
                for(MyUtil::IntSeq::iterator it = tmp.begin(); it != tmpit; it++){
                        building[BiBlock].insert(*it);
                        building[Block].erase(*it);
                        building[Blocked].erase(*it);
                }

        }

        // construct result object.
        BuddyRelationPtr buddyrelation = new BuddyRelation();
        if( !building[Block].empty()){
                for (set<int>::iterator it = building[Block].begin(); it != building[Block].end(); ++it) {
                        buddyrelation->block.push_back(*it);
                }
        }
        if( !building[Blocked].empty()){
                for (set<int>::iterator it = building[Blocked].begin(); it != building[Blocked].end(); ++it) {
                        buddyrelation->blocked.push_back(*it);
                }
        }
        if( !building[BiBlock].empty()){
                for (set<int>::iterator it = building[BiBlock].begin(); it != building[BiBlock].end(); ++it) {
                        buddyrelation->biblock.push_back(*it);
                }
        }
        if( !building[Friend].empty()){
                for (set<int>::iterator it = building[Friend].begin(); it != building[Friend].end(); ++it) {
                        buddyrelation->friends.push_back(*it);
                }
        }
        if( !building[Apply].empty()){
                for (set<int>::iterator it = building[Apply].begin(); it != building[Apply].end(); ++it) {
                        buddyrelation->apply.push_back(*it);
                }
        }
        if( !building[Applied].empty()){
                for (set<int>::iterator it = building[Applied].begin(); it != building[Applied].end(); ++it) {
                        buddyrelation->applied.push_back(*it);
                }
        }
	
        TaskManager::instance().execute(new DupCleanupTask(id, duplicates));
	MCE_INFO("create -> " << id << " friendcount["<<buddyrelation->friends.size() << "] done");
        return buddyrelation;
}

void DupCleanupTask::handle() {
	if( !_dup->block.empty()){
                for (MyUtil::IntSeq::const_iterator it = _dup->block.begin(); it != _dup->block.end(); ++it) {
                        handle(Block,*it);
                }
		MCE_INFO(" DupCleanupTask::handle() block and size=" << _dup->block.size());
        }
        if(!_dup->blocked.empty()){
                for (MyUtil::IntSeq::const_iterator it = _dup->blocked.begin(); it != _dup->blocked.end(); ++it) {
                        handle(Blocked,*it);
                }
		MCE_INFO(" DupCleanupTask::handle() blocked and size=" << _dup->blocked.size());
        }
        if(!_dup->friends.empty()){
                for (MyUtil::IntSeq::const_iterator it = _dup->friends.begin(); it != _dup->friends.end(); ++it) {
                        handle(Friend,*it);
                }
		MCE_INFO(" DupCleanupTask::handle() friends and size=" << _dup->friends.size());
        }
        if(!_dup->apply.empty()){
                for (MyUtil::IntSeq::const_iterator it = _dup->apply.begin(); it != _dup->apply.end(); ++it) {
                        handle(Apply,*it);
                }
		MCE_INFO(" DupCleanupTask::handle() apply and size=" << _dup->apply.size());
        }
        if(!_dup->applied.empty()){
                for (MyUtil::IntSeq::const_iterator it = _dup->applied.begin(); it != _dup->applied.end(); ++it) {
                        handle(Applied,*it);
                }
		MCE_INFO(" DupCleanupTask::handle() applied and size=" << _dup->applied.size());
        }
}

void DupCleanupTask::handle(const BuddyDesc desc, const int to) {
        Statement sql,sqlto;
        switch (desc) {
        case Applied:
                {
			ostringstream pattern;
                        pattern << "buddyapply_accepter_" << abs(_userId % 100);
                        sql << "DELETE FROM buddyapply_accepter_" << abs(_userId % 100) <<" WHERE accepter = '" << _userId
                                        << "' AND applicant = '" << to << "' LIMIT 1";
                        //QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer, pattern.str()).schedule(sql);

                        ostringstream patternto;
                        patternto << "buddyapply_applicant_" << abs(to % 100);
                        sqlto << "DELETE FROM buddyapply_applicant_" << abs(to % 100) <<" WHERE accepter = '" << _userId
                                        << "' AND applicant = '" << to << "' LIMIT 1";
                        //QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,patternto.str()).schedule(sqlto);
		}break;
        case Apply:
                {
			ostringstream pattern;
                        pattern << "buddyapply_accepter_" << abs(_userId % 100);
                        sql << "DELETE FROM buddyapply_accepter_" << abs(to % 100) << " WHERE accepter = '" << to
                                        << "' AND applicant = '" << _userId << "' LIMIT 1";
                        //QueryRunner(DB_BUDDY_ACCEPTER, CDbWServer,pattern.str()).schedule(sql);

                        ostringstream patternto;
                        patternto << "buddyapply_applicant_" << abs(to % 100);
                        sqlto << "DELETE FROM buddyapply_applicant_" << abs(_userId % 100) << " WHERE accepter = '" << to
                                        << "' AND applicant = '" << _userId << "' LIMIT 1";
                        //QueryRunner(DB_BUDDY_APPLICANT, CDbWServer,patternto.str()).schedule(sqlto);
		}break;
        case Friend:
                sql << "DELETE FROM relation_" << abs(_userId % 100) << " WHERE host = '"
                                << _userId << "' AND guest='" << to
                                << "' AND disc=0 LIMIT 1";
                {
                        ostringstream pattern;
                        pattern << "relation_" << abs(_userId % 100);
                        //QueryRunner(DB_BUDDY_FRIEND, CDbWServer, pattern.str()).schedule(sql);
                }
                break;
	case Block:
                {
                        sql << "DELETE FROM relation_block WHERE host=" << _userId
                                        << " AND guest=" << to << " LIMIT 1";
                        QueryRunner(DB_BUDDY_BLOCK, CDbWServer).schedule(sql);
                }break;
        case Blocked:
                {
                        sql << "DELETE FROM relation_block WHERE host=" << to
                                << " AND guest=" << _userId << " LIMIT 1";
                        QueryRunner(DB_BUDDY_BLOCK, CDbWServer).schedule(sql);
                }break;
        default:
                MCE_WARN("Cleanup : " << _userId << " " << to);
        }
}

/*************************************************************************************************/

