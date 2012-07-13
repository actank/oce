#include "BatchUpdaterI.h"
#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

using namespace mop::hi::svc::model;
using namespace std;
using namespace MyUtil;
using namespace mop::hi::svc::batchtraits;
using namespace xce::serverstate;

//---------------------------------------------------------------------------
void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(new BatchUpdaterManagerI, service.createIdentity(
                        MANAGER, ""));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BatchUpdater.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BatchUpdater.Interval", 5);
        string controllerEndpoints = service.getCommunicator()->getProperties()->getPropertyWithDefault("BatchUpdater.ControllerEndpoint","ControllerBatchUpdater");

	ServerStateSubscriber::instance().initialize(controllerEndpoints, new BatchUpdaterManagerI, mod, interval);
  ServiceMonitorManager::instance().start();
}

BatchUpdaterManagerI::BatchUpdaterManagerI() {
	_mapTrait = BatchFactory().create();

	for (std::map<std::string, mop::hi::svc::batchtraits::BatchTraitBase*>::iterator
			it = _mapTrait.begin(); it != _mapTrait.end(); ++it) {
		MCE_INFO("Trait map: " << it->first << " => " << it->second);
	}
}

BatchUpdaterManagerI::~BatchUpdaterManagerI() {
	for (std::map<std::string, mop::hi::svc::batchtraits::BatchTraitBase*>::iterator
			i = _mapTrait.begin(); i != _mapTrait.end(); ++i) {
		delete i->second;
	}
}

void BatchUpdaterManagerI::inc(const MyUtil::IntSeq& keyvalue,
                ::Ice::Int count, const std::string& type, const Ice::Current& current) {
        InvokeClient ic = InvokeClient::create(current);

        MyUtil::LongSeq keyValue;
        for(std::vector<int>::const_iterator it = keyvalue.begin();it!=keyvalue.end();it++){
                keyValue.push_back((long)(*it));
        }
        incLong(keyValue, count, type, current);

}

void BatchUpdaterManagerI::dec(const MyUtil::IntSeq& keyvalue,
                ::Ice::Int count, const std::string& type, const Ice::Current& current) {
        InvokeClient ic = InvokeClient::create(current);
        MyUtil::LongSeq keyValue;
        for(std::vector<int>::const_iterator it = keyvalue.begin();it!=keyvalue.end();it++){
                keyValue.push_back((long)(*it));
        }
        decLong(keyValue, count, type, current);
}


void BatchUpdaterManagerI::set(const MyUtil::IntSeq& keyvalue,
                const std::string& value, const std::string& type, const Ice::Current& current) {
        InvokeClient ic = InvokeClient::create(current);

        MyUtil::LongSeq keyValue;
        for(std::vector<int>::const_iterator it = keyvalue.begin();it!=keyvalue.end();it++){
                keyValue.push_back((long)(*it));
        }
        setLong(keyValue, value, type, current);

}

void BatchUpdaterManagerI::incLong(const MyUtil::LongSeq& keyvalue,
                ::Ice::Int count, const std::string& type, const Ice::Current& current) {
        ostringstream inMsg;
        inMsg << "type=" << type << " increment=" << count;
        InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

        std::map<std::string, mop::hi::svc::batchtraits::BatchTraitBase*>::iterator
                        i = _mapTrait.find(type);
        if (i == _mapTrait.end()) {
                MCE_WARN("BatchUpdaterManagerI::inc type " << type << "  call Error!");
                return;
        }

        //双写，兼容过渡，数据库导完后可以删除
        if(type == PhotoViewNName){
                MyUtil::LongSeq oldkeyvalue;
		oldkeyvalue.push_back(keyvalue[0]); //ownerId
		oldkeyvalue.push_back(keyvalue[1]); //photoId
                _mapTrait[PhotoViewName]->incLong(oldkeyvalue, count);
        }

        i->second->incLong(keyvalue, count);
}

void BatchUpdaterManagerI::decLong(const MyUtil::LongSeq& keyvalue,
                ::Ice::Int count, const std::string& type, const Ice::Current& current) {
        ostringstream inMsg;
        inMsg << "type=" << type << " increment=" << count;
        InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

        std::map<std::string, mop::hi::svc::batchtraits::BatchTraitBase*>::iterator
                        i = _mapTrait.find(type);
        if (i == _mapTrait.end()) {
                MCE_WARN("BatchUpdaterManagerI::dec type " << type << "  call Error!");
                return;
        }

        //双写，兼容过渡，数据库导完后可以删除
        if(type == PhotoViewNName){
                MyUtil::LongSeq oldkeyvalue;
                oldkeyvalue.push_back(keyvalue[0]); //ownerId
                oldkeyvalue.push_back(keyvalue[1]); //photoId
                _mapTrait[PhotoViewName]->decLong(oldkeyvalue, count);
        }

        i->second->decLong(keyvalue, count);
}

void BatchUpdaterManagerI::setLong(const MyUtil::LongSeq& keyvalue,
                const std::string& value, const std::string& type, const Ice::Current& current) {
        ostringstream oss;
        oss << *keyvalue.begin() << " " << value << " " << type;
        InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
        std::map<std::string, mop::hi::svc::batchtraits::BatchTraitBase*>::iterator
                        i = _mapTrait.find(type);
        if (i == _mapTrait.end()) {
                MCE_WARN("BatchUpdaterManagerI::set type " << type << "  call Error!");
                return;
        }

        i->second->setLong(keyvalue, value);
}


void mop::hi::svc::model::BatchUpdaterManagerI::incUserViewCount(::Ice::Int id,
		::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[UserViewName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incLoginCount(::Ice::Int id,
		::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[LoginName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::setLastLoginTime(::Ice::Int id,
		const string& time, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	StateUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[LastLoginName]->setLong(ids, time);
}

void mop::hi::svc::model::BatchUpdaterManagerI::setLastLogoutTime(
		::Ice::Int id, const string& time, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	StateUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[LastLogoutName]->setLong(ids, time);
}

void mop::hi::svc::model::BatchUpdaterManagerI::setLastModifyTime(
		::Ice::Int id, const string& time, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	StateUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[LastModifyName]->setLong(ids, time);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incFriendDist(
		::Ice::Int userId, ::Ice::Int uniId, ::Ice::Int count,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(uniId);
	_mapTrait[FriendDistName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decFriendDist(
		::Ice::Int userId, ::Ice::Int uniId, ::Ice::Int count,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(uniId);
	_mapTrait[FriendDistName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incExp(::Ice::Int userId,
		::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[ExpName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decExp(::Ice::Int userId,
		::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[ExpName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incNewMessageCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "DEPEACATED method", InvokeClient::INVOKE_LEVEL_WARN);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decNewMessageCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "DEPEACATED method", InvokeClient::INVOKE_LEVEL_WARN);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incMessageInCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[MessageInName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decMessageInCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[MessageInName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incTribeCount(
		::Ice::Int userId, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[TribeName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decTribeCount(
		::Ice::Int userId, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[TribeName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incTribeRequestCount(
		::Ice::Int userId, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[TribeRequestName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decTribeRequestCount(
		::Ice::Int userId, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[TribeRequestName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incMessageOutCount(
		::Ice::Int userId, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[MessageOutName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decMessageOutCount(
		::Ice::Int userId, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	_mapTrait[MessageOutName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incFriendCount(::Ice::Int id,
		::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[FriendName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decFriendCount(::Ice::Int id,
		::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[FriendName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incGuestRequestCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "DEPEACATED method", InvokeClient::INVOKE_LEVEL_WARN);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decGuestRequestCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "DEPEACATED method", InvokeClient::INVOKE_LEVEL_WARN);
}


void mop::hi::svc::model::BatchUpdaterManagerI::incLeavewordCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[LeavewordName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decLeavewordCount(
		::Ice::Int id, ::Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(id);
	_mapTrait[LeavewordName]->decLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::incBlogViewCount(Ice::Int id,
		::Ice::Int userId, Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_DEBUG);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(id);
	_mapTrait[BlogViewName]->incLong(ids, count);
}

void mop::hi::svc::model::BatchUpdaterManagerI::decBlogViewCount(Ice::Int id,
		Ice::Int userId, Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(id);
	_mapTrait[BlogViewName]->decLong(ids, count);

}

void mop::hi::svc::model::BatchUpdaterManagerI::incPhotoViewCount(Ice::Int id,
		Ice::Int userId, Ice::Int count, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "photoId=" << id << " ownerId=" << userId << " increment=" << count;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(id);
	_mapTrait[PhotoViewName]->incLong(ids, count);

}

void mop::hi::svc::model::BatchUpdaterManagerI::decPhotoViewCount(Ice::Int id,
		Ice::Int userId, Ice::Int count, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "photoId=" << id << " ownerId=" << userId << " increment=" << count;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(id);
	_mapTrait[PhotoViewName]->decLong(ids, count);

}

void mop::hi::svc::model::BatchUpdaterManagerI::incTribePageViewCount(
		Ice::Int tribeId, Ice::Int count, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(tribeId);
	_mapTrait[TribePageViewName]->incLong(ids, count);

}

void mop::hi::svc::model::BatchUpdaterManagerI::incFlyerUserAdCount(
		Ice::Int userId, Ice::Int adId, Ice::Int count,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	CountUpdater::IdType ids;
	ids.push_back(userId);
	ids.push_back(adId);
	_mapTrait[TribePageViewName]->decLong(ids, count);

}
