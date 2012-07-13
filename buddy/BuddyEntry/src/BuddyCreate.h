#ifndef __BUDDY_CREATE_H__
#define __BUDDY_CREATE_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "BuddyRelation.h"
#include "../../../ReplicaCluster/CacheLoader/src/CacheLoaderFactory.h"
#include "BuddyListI.h"

namespace xce {
namespace buddy {

//const static std::string DB_BUDDY_FRIEND = "buddy_friend";
//const static std::string DB_BUDDY_APPLY = "buddy_apply";
//const static std::string DB_BUDDY_BLOCK = "buddy_block";

const static std::string DB_BUDDY_FRIEND = "buddycore_friend";
const static string DB_BUDDY_APPLICANT = "buddycore_applicant";
const static string DB_BUDDY_ACCEPTER = "buddycore_accepter";
const static std::string DB_BUDDY_BLOCK = "buddycore_block";
const static int TABLE_FRIEND_COUNT = 100;
const int TABLE_ACCEPTER_COUNT = 100;
const int TABLE_APPLICANT_COUNT = 100;
const static std::string TABLE_FRIEND = "relation_";
const static string TABLE_APPLY_ACCEPTER = "buddyapply_accepter_";
const static string TABLE_APPLY_APPLICANT = "buddyapply_applicant_";
const static std::string TABLE_BLOCK = "relation_block";

const int TASK_LEVEL_DUPCLEANUP = 9;//ensure it's different
/*************************************************************************************/

class BuddyRelationFactory: virtual public MyUtil::Singleton<BuddyRelationFactory> {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
private:
	friend class MyUtil::Singleton<BuddyRelationFactory>;
        BuddyRelationFactory(){
                ;
        }
};

class BuddyRelationHoldersICacheFactory: public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id){
		StateRecorder sr;
                sr.lastid = 0;
                sr.laststate = NULLSTATE;
		try {
                        BuddyRelationPtr buddyrelation = BuddyRelationPtr::dynamicCast(BuddyRelationFactory::instance().create(id));
                	BuddyRelationHoldersPtr brhp = new BuddyRelationHolders(id,buddyrelation,sr);
			return new BuddyRelationHoldersI(brhp);
		} catch (Ice::Exception& e) {
                        MCE_WARN("[create] Ice::Exception " << id << ": " << e.what());
                } catch (std::exception& e) {
                        MCE_WARN("[create] std::excetpion " << id << ": " << e.what());
                } catch (...) {
                        MCE_WARN("[create] exception" << id );
                }
		BuddyRelationHoldersPtr brhp = new BuddyRelationHolders(id,NULL,sr);
                return new BuddyRelationHoldersI(brhp);
	}
};

class BuddyRelationHoldersILoaderFactory: public xce::cacheloader::IdListFactory, virtual public MyUtil::Singleton<BuddyRelationHoldersILoaderFactory> {
public:
	Ice::ObjectPtr create(Ice::Int id){
		StateRecorder sr;
                sr.lastid = 0;
                sr.laststate = NULLSTATE;
		try {
        		BuddyRelationPtr buddyrelation = BuddyRelationPtr::dynamicCast(BuddyRelationFactory::instance().create(id));
			BuddyRelationHoldersPtr brhp = new BuddyRelationHolders(id,buddyrelation,sr);
                        return new BuddyRelationHoldersI(brhp);
		} catch (Ice::Exception& e) {
                        MCE_WARN("[create] Ice::Exception " << id << ": " << e.what());
                } catch (std::exception& e) {
                        MCE_WARN("[create] std::excetpion " << id << ": " << e.what());
                } catch (...) {
                        MCE_WARN("[create] exception" << id );
                }
		BuddyRelationHoldersPtr brhp = new BuddyRelationHolders(id,NULL,sr);
                return new BuddyRelationHoldersI(brhp);
	}
	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids){
		MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
       		for (size_t i = 0; i < ids.size(); ++i) {
                	try {
                        	Ice::ObjectPtr obj = create( (Ice::Int)ids.at(i) );
                        	if (obj) {
                                	res->data[ids.at(i)] = obj;
                        	}
                	} catch (Ice::Exception& e) {
                        	MCE_WARN("[create] Ice::Exception " << ids.at(i) << ": " << e.what());
                	} catch (std::exception& e) {
                        	MCE_WARN("[create] std::excetpion " << ids.at(i) << ": " << e.what());
                	} catch (...) {
                        	MCE_WARN("[create] exception" << ids.at(i) );
                	} 
        	}
        	return res;	
	}
private:
	friend class MyUtil::Singleton<BuddyRelationHoldersILoaderFactory>;
	BuddyRelationHoldersILoaderFactory(){
		;
	}

};

class BuddyResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BuddyResultHandler(int userId, set<int>& totalbuilding, set<int>& building, MyUtil::IntSeq& duplicates, const std::string& toField) :
                _userId(userId), _totalbuilding(totalbuilding),_building(building), _duplicates(duplicates),_toField(toField) {
        }
        virtual ~BuddyResultHandler() {
        }
private:
        virtual bool handle(mysqlpp::Row& row) const {
                int to = (int) row[_toField.c_str()];
                if (!_totalbuilding.insert(to).second) {
                        _duplicates.push_back(to);
			MCE_DEBUG(" BuddyResultHandler::handle duplicates push [ userId: " <<_userId<< "to: "<<to<<" ]");
                }else{
			_building.insert(to);
		}
		
                return true;
        }

        int _userId;
        set<int>& _totalbuilding;
	set<int>& _building;
        MyUtil::IntSeq& _duplicates;
        std::string _toField;
};

class DupCleanupTask: public MyUtil::Task {
public:
        DupCleanupTask(int userId, BuddyRelationPtr& dup) :
                MyUtil::Task(TASK_LEVEL_DUPCLEANUP), _userId(userId), _dup(dup) {
        }
        virtual void handle();
private:
        void handle(const BuddyDesc,const int);
        int _userId;
        BuddyRelationPtr _dup;
};
/********************************************************************************/

class BuddyListCacheFactory: public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id){
		try {
                        BuddyRelationPtr ptr = BuddyRelationPtr::dynamicCast(BuddyRelationFactory::instance().create(id));
                	MyUtil::ByteSeq seq;
			unsigned char index;
			BuddyListHelper::instance().initHash(ptr,seq,index);	
			BuddyListPtr blp =new BuddyList(ptr,seq,index);
                        return new BuddyListI(blp);
		}catch (Ice::Exception& e) {
                        MCE_WARN("[create] Ice::Exception " << id << ": " << e.what());
                } catch (std::exception& e) {
                        MCE_WARN("[create] std::excetpion " << id << ": " << e.what());
                } catch (...) {
                        MCE_WARN("[create] exception" << id );
                }
                return new BuddyListI();
	}
};

class BuddyListLoaderFactory: public xce::cacheloader::IdListFactory,virtual public MyUtil::Singleton<BuddyListLoaderFactory> {
public:
        Ice::ObjectPtr create(Ice::Int id){
		try {
			BuddyRelationPtr ptr = BuddyRelationPtr::dynamicCast(BuddyRelationFactory::instance().create(id));
                	MyUtil::ByteSeq seq;
                        unsigned char index;
                        BuddyListHelper::instance().initHash(ptr,seq,index);
			BuddyListPtr blp =new BuddyList(ptr,seq,index);
			return new BuddyListI(blp);
		}catch (Ice::Exception& e) {
                        MCE_WARN("[create] Ice::Exception " << id << ": " << e.what());
                } catch (std::exception& e) {
                        MCE_WARN("[create] std::excetpion " << id << ": " << e.what());
                } catch (...) {
	                MCE_WARN("[create] exception" << id );
                }

		return new BuddyListI();
        }
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids){
                //size = 0;
		MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
                for (size_t i = 0; i < ids.size(); ++i) {
                	Ice::ObjectPtr obj = create( (Ice::Int)ids.at(i) );
                        if (obj) {
                        	res->data[ids.at(i)] = obj;
                        }
                }
		MCE_INFO("[BuddyListLoaderFactory:createBatch] done");
                return res;
        }
private:
	friend class MyUtil::Singleton<BuddyListLoaderFactory>;
        BuddyListLoaderFactory(){
                ;
        }

};
}
}
#endif//__BUDDY_CREATE_H__
