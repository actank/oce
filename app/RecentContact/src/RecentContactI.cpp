#include "RecentContactI.h"
#include "util/cpp/String.h"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"

using namespace MyUtil;
using namespace xce::recentcontact;
using namespace xce::serverstate;
using namespace ::xce::log;
using namespace com::xiaonei::xce;

//----------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	
	service.getAdapter()->add(&RecentContactManagerI::instance(), service.createIdentity("RCM", ""));

	TaskManager::instance().config(TASK_LEVEL_BUILD, ThreadPoolConfig(0,30));

	service.registerObjectCache(RECENTCONTACT_SET, "RCS", new RecentContactFactoryI);
	
	//加入向Controller注册
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("RecentContact.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("RecentContact.Interval", 5);
	string controllerEndpoints = service.getCommunicator()->getProperties()->getPropertyWithDefault("RecentContact.ControllerEndpoint","ControllerRecentContact");

	ServerStateSubscriber::instance().initialize(controllerEndpoints, &RecentContactManagerI::instance(), mod, interval);
  ServiceMonitorManager::instance().start();
}

//-------------------------------------RecentContactFactoryI--------------------------------
Ice::ObjectPtr RecentContactFactoryI::create(int ownerId) {
	GetProps& props = GetProps::instance();
        string table = props.getTableName(ownerId);
        Statement sql;
        sql << "SELECT * FROM " << table << " WHERE " << COLUMN_HOST << "=" << ownerId;

        RecentContactSetPtr obj = new RecentContactSet;
        QueryRunner(DB_SOURCE, CDbRServer, table).query(sql, RecentContactSetHandler(ownerId,obj));
 
        ServiceI::instance().addObject(obj, RECENTCONTACT_SET, (long)ownerId);
	return obj;
}

bool RecentContactSetHandler::handle(mysqlpp::Row& row) const{

	MyUtil::IntSeq byteSeq = IntSeqSerialize::unserialize(row["guests"]);	

	ContactInfoSeq result;
	for ( size_t i = 0; i < byteSeq.size()/3; ++i ) {
	        ContactInfoPtr conInfo = new ContactInfo;
                conInfo->guestId = byteSeq[3*i];
                conInfo->ownerId = _ownerId;
                conInfo->time = byteSeq[3*i+1];
                int typeTemp = byteSeq[3*i+2];
                if(typeTemp==0)
                        conInfo->type = XNMESSAGE;
                if ( (conInfo->guestId > 0) && (conInfo->time >0) ) {
                        result.push_back(conInfo);
                }
        }
        sort(result.begin(), result.end(), SortContactInfo::OrderByTimeDesc);

	_recConSetPtr->getContactInfos() = result;
	return true;
}

//-----------------------------------GetProps-----------------------------------------------------------
GetProps::GetProps(){
	ServiceI& service = ServiceI::instance();
        Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

        _tableCount = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);
        _tableName = props->getPropertyWithDefault("Service." + service.getName() + ".TableName", DB_SOURCE);
}

string GetProps::getTableName(int ownerId) {
        ostringstream result;
        result << _tableName << "_" << (ownerId % _tableCount);
        return result.str();
}

//-----------------------------------RecentContactSet----------------------------------------------------
ContactInfoSeq& RecentContactSet::getContactInfos() {
	return _contactInfos;
}

ContactInfoSeq RecentContactSet::get(ContactType type, Ice::Int begin, Ice::Int limit ) {
	IceUtil::Mutex::Lock lock(_mutex);
	begin = begin < 0 ? 0 : begin;
	limit = limit < 0 ? _contactInfos.size() : limit;
	ContactInfoSeq contactInfos;
	for(size_t i = begin; i < _contactInfos.size(); ++i) {
		if (i >= (size_t)limit)
			break;
		if(_contactInfos.at(i)->type == type) {
			contactInfos.push_back(_contactInfos.at(i));
		}
	}
	return contactInfos;
}

void RecentContactSet::add(ContactInfoSeq::const_iterator it){
        IceUtil::Mutex::Lock lock(_mutex);
	ContactInfoSeq::iterator itMem;
	ContactInfoSeq::iterator itNext;
        for(itMem = _contactInfos.begin(); itMem!=_contactInfos.end(); ++itMem){
		itNext = itMem + 1;
	        if(isEqual(it, itMem)){
	                //将原有的移出，重新插入新的数据信息，能够保持按照时间顺序排序
			ContactInfoSeq::iterator itTemp = itMem;
                        _contactInfos.erase(itMem);
                        (*it)->time = time(NULL);
			_contactInfos.push_back(*it);
                        try{
                       		updateDb((*it)->ownerId, _contactInfos);
			} catch (mysqlpp::Exception& e) {
		                MCE_WARN("[RecentContactSet::add]  from db mysqlpp::exception : " << e.what());
				for(ContactInfoSeq::iterator itRemove = _contactInfos.begin();itRemove!=_contactInfos.end();++itRemove){
                                	if(isEqual(it, itRemove)){
                                        	_contactInfos.erase(itRemove);
	                                        break;
        	                        }
                        	}
				_contactInfos.insert(itNext ,*itTemp);
		        } catch (...) {
		                MCE_WARN("[RecentContactSet::add]  exception ");
				for(ContactInfoSeq::iterator itRemove = _contactInfos.begin();itRemove!=_contactInfos.end();++itRemove){
                                        if(isEqual(it, itRemove)){
                                                _contactInfos.erase(itRemove);
                                                break;
                                        }
                                }
                                _contactInfos.insert(itNext ,*itTemp);

		        }

                        break;
                  }
        }

	if(itMem == _contactInfos.end()){
		ContactInfoSeq::iterator itFir = _contactInfos.begin();
	        if(isReachRecentCountLimit()){
                        //对应删除数据库中的多余数据
                        _contactInfos.erase(itFir);
                 }
                (*it)->time = time(NULL);
                 _contactInfos.push_back(*it);
		try{ 
	                updateDb((*it)->ownerId, _contactInfos);
		} catch (mysqlpp::Exception& e) {
	                MCE_WARN("[RecentContactSet::add] from db mysqlpp::exception : " << e.what());
			if(isReachRecentCountLimit()){
				ContactInfoSeq::iterator itSec = _contactInfos.begin();
	                        _contactInfos.insert(itSec, *itFir);

			}
			for(ContactInfoSeq::iterator itTemp = _contactInfos.begin();itTemp!=_contactInfos.end();++itTemp){
				if(isEqual(it, itTemp)){
					_contactInfos.erase(itTemp);
					break;
				}
			}
	        } catch (...) {
        	        MCE_WARN("[RecentContactSet::add]  exception ");
			if(isReachRecentCountLimit()){
                                ContactInfoSeq::iterator itSec = _contactInfos.begin();
                                _contactInfos.insert(itSec, *itFir);

                        }
                        for(ContactInfoSeq::iterator itTemp = _contactInfos.begin();itTemp!=_contactInfos.end();++itTemp){
                                if(isEqual(it, itTemp)){
                                        _contactInfos.erase(itTemp);
                                        break;  
                                }
                        }
	        }

        }

}

void RecentContactSet::remove(int ownerId,int guestId){
        IceUtil::Mutex::Lock lock(_mutex);
	ContactInfoSeq contactInfos = _contactInfos;
	ContactInfoSeq::iterator itNext;
        for(ContactInfoSeq::iterator it = _contactInfos.begin(); it!=_contactInfos.end(); ++it) {
	        if(((*it)->guestId)==guestId) {
        	        _contactInfos.erase(it);
			itNext = it + 1;
                	try{
				updateDb(ownerId, _contactInfos);
			 } catch(mysqlpp::Exception& e) {
	                        MCE_WARN("[RecentContactSet::remove] from db mysqlpp::exception : " << e.what());
				_contactInfos.insert(itNext, *it);
             		 } catch (...) {
                        	MCE_WARN("[RecentContactSet::remove]  exception ");
				_contactInfos.insert(itNext , *it);
        	 	}
	                 break;
		}
         }	
}

void RecentContactSet::updateDb(int ownerId,const ContactInfoSeq& contactInfos){
   	GetProps& props = GetProps::instance();
        string table = props.getTableName(ownerId);
        Statement sqlAdd;
	
	size_t guests_size = contactInfos.size() > size_t(GUESTS_SIZE) ? size_t(GUESTS_SIZE):contactInfos.size();
        MyUtil::IntSeq byteSeq;
        for ( size_t i = 0; i < guests_size; ++i ) {
                byteSeq.push_back((contactInfos[i]->guestId));
                byteSeq.push_back(time(NULL));
                byteSeq.push_back((contactInfos[i]->type));
        }

        sqlAdd << "REPLACE INTO " << table << "(" << COLUMN_HOST << ", " << COLUMN_GUESTS << ")" << "  values" << " ("<< ownerId << ","<<mysqlpp::quote<<IntSeqSerialize::serialize(byteSeq)<<")";

 //       QueryRunner(DB_SOURCE, CDbWServer, table).execute(sqlAdd);
          QueryRunner(DB_SOURCE, CDbWServer, table).schedule(sqlAdd);
}

bool RecentContactSet::isEqual(ContactInfoSeq::const_iterator it, ContactInfoSeq::iterator itMem) {	
	if((*itMem)->guestId !=  (*it)->guestId)
		return false;
	if((*itMem)->ownerId != (*it)->ownerId)
		return false;
	if((*itMem)->type != (*it)->type)
		return false;
	return true;
}

bool RecentContactSet::isReachRecentCountLimit(){
	return (int)_contactInfos.size()==LIMIT_COUNT;
}

//------------------------------------RecentContactManagerI-----------------------------------------------
RecentContactManagerI::RecentContactManagerI() {
}

RecentContactManagerI::~RecentContactManagerI() {
}

RecentContactSetPtr RecentContactManagerI::getRecentContactSet(int ownerId) {
	return ServiceI::instance().locateObject<RecentContactSetPtr>(RECENTCONTACT_SET, ownerId);
}

ContactInfoSeq RecentContactManagerI::get(int ownerId,const ContactType type, int begin, int limit,const Ice::Current& crt) {
  ostringstream oss;
  oss << ownerId << " " << type << " " << begin << " " << limit;
  InvokeClient::create(crt, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
	ContactInfoSeq contactInfoResult ;
	RecentContactSetPtr recentContactSet = getRecentContactSet(ownerId);
	recentContactSet->getContactInfos() = recentContactSet->get(type, begin, limit);
	contactInfoResult = recentContactSet->getContactInfos();

	return contactInfoResult;
}

void RecentContactManagerI::add(const ContactInfoSeq& contactInfos,const Ice::Current& crt){
  InvokeClient::create(crt);
	int ownerId = contactInfos.at(0)->ownerId;
	MCE_INFO("[RecentContactManagerI::add] " << ownerId);
	try{
        	RecentContactSetPtr obj = ServiceI::instance().locateObject<RecentContactSetPtr>(RECENTCONTACT_SET, (long)ownerId);
		for(ContactInfoSeq::const_iterator it = contactInfos.begin();it!=contactInfos.end();++it){
			obj->add(it);
		}

	} catch (Ice::Exception& e) {
		MCE_WARN("[RecentContactManager::add] " << ownerId <<  e.what());
	} catch (...) {
		MCE_WARN("[RecentContactManager::add] " << ownerId << " exception ");
	}
}

void RecentContactManagerI::remove(int ownerId, int guestId,const Ice::Current& crt) {
  ostringstream oss;
  oss << ownerId << " " << guestId;
  InvokeClient::create(crt, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
	RecentContactSetPtr initObj = new RecentContactSet;
	try {
		initObj = ServiceI::instance().locateObject<RecentContactSetPtr>(RECENTCONTACT_SET, (long)ownerId);
	} catch (Ice::Exception& e) {
                MCE_WARN("[RecentContactManager::remove] " << ownerId <<  e.what());
        } catch (...) {
                MCE_WARN("[RecentContactManager::remove] " << ownerId << " exception ");
        }

	if(initObj) {
		initObj->remove(ownerId, guestId);
 	}
}

