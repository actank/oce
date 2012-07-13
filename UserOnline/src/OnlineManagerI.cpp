#include "UserOnlineServiceI.h"
#include "OnlineManagerI.h"
#include "IceLogger.h"
#include "ConnectionHolder.h"

using namespace mop::hi::svc::online;
//using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::xce;



void OnlineManagerI::init(::Ice::Int evictStep, ::Ice::Int expStep) {
	MCE_DEBUG("OnlineManagerI init.");
	_sessions = new OnlineEvictor(expStep);
	_sessions->setSize(-1);

	TaskManager::instance().config(1, 2);
	TaskManager::instance().execute(new KickTimeoutTask(_sessions, evictStep));
	TaskManager::instance().execute(new UpdateOnlineStatTask(_sessions, 1, true));
	MCE_DEBUG(".done");
}

OnlineManagerI::~OnlineManagerI() {
	IntSeq userIds = _sessions->getStats(0, -1, OT_ALL);
	mop::hi::svc::adapter::UserAdapter::instance().setUserOnlineStats(userIds, false);
	//UserAdapter::instance().setUserIMOnlineStats(userIds, false);
	//mop::hi::svc::adapter::MainAdapter::instance().webLogout(userIds);
}


ActiveUserManagerPrx
OnlineManagerI::getActiveUserManager()
{
	return _activeUserManager;
}

string& OnlineManagerI::getImOnlineDb()
{
	return _imOnlineDb;
}

string& OnlineManagerI::getOnlineDb()
{
	return _onlineDb;
}



void OnlineManagerI::setStat(::Ice::Int userId, ::Ice::Int onlineType,
		bool stat, const Ice::Current& current) {
	_sessions->setStat(userId, onlineType, stat);

	//--------------------------
//	if ((onlineType == OT_WEB) && stat) {
//		MCE_DEBUG("WEB  ONLINE");
//		::mop::hi::svc::im::OnlineUserSeq ous;
//		::mop::hi::svc::im::OnlineUserPtr user =
//				new ::mop::hi::svc::im::OnlineUser();
//		user->userId = userId;
//		user->source = "WEB";
//		user->sourceType = 4;//Client_Web
//		user->stat = 2;
//		user->extIp = 0;
//		user->localIp = 0;
//		user->port = 0;
//		user->emotion = 0;
//		user->version = 0;
//		user->timeStamp = 0;
//		user->uTimeStamp = 0;
//		ous.push_back(user);
//
//		//mop::hi::svc::adapter::MainAdapter::instance().login(ous);
//	}

	//--------------------------
}

void OnlineManagerI::setStats(const ::MyUtil::IntSeq& userIds,
		::Ice::Int onlineType, bool online, const Ice::Current& current) {
	MCE_DEBUG("OnlineManagerI::setStats working...");
	for (size_t i = 0; i< userIds.size() ; ++i) {
		setStat(userIds.at(i) , onlineType, online);
	}
}

Ice::Int OnlineManagerI::size(::Ice::Int onlineType, const Ice::Current&) {
	return _sessions->size(onlineType);
}

Ice::Int OnlineManagerI::getStat(::Ice::Int userId, const Ice::Current&) {
	return _sessions->getStat(userId);
}

MyUtil::Int2IntMap OnlineManagerI::checkStats(const MyUtil::IntSeq& userIds,
		const Ice::Current& current) {
	return _sessions->checkStats(userIds);
}

IntSeq OnlineManagerI::getStats(::Ice::Int begin, ::Ice::Int limit,
		::Ice::Int onlineType, const Ice::Current& current) {
	return _sessions->getStats(begin, limit, onlineType);
}

KickTimeoutTask::KickTimeoutTask(const OnlineEvictorPtr& sessions, int evictStep) :
	Task(time(NULL) + evictStep), _sessions(sessions), _evictStep(evictStep) {

}
;

void KickTimeoutTask::handle() {
	level(time(NULL) + _evictStep);
	TaskManager::instance().execute(this);
	_sessions->timedEvictEntries();
}

UpdateOnlineStatTask::UpdateOnlineStatTask(OnlineEvictorPtr evictor,
		int timeslot, bool isAuto) :
	Task(timeslot + time(NULL) ), _timeslot(timeslot), _isAuto(isAuto),
			_evictor(evictor) {

}
;

void UpdateOnlineStatTask::handle() {
	
	if (_isAuto) {
		level(_timeslot + time(NULL));
		TaskManager::instance().execute(this);
	}
	OnlineUserMap onlineChange;
	_evictor->swapOnlineChange(onlineChange);

	MyUtil::IntSeq onlineIds, offlineIds;
	for (OnlineUserMap::iterator it = onlineChange.begin(); it
			!= onlineChange.end(); ++it) {
		if (!it->second || it->second->onlineStat == OFF_LINE) {
			offlineIds.push_back(it->first);
		} else {
			onlineIds.push_back(it->first);
		}
	}

	if (!offlineIds.empty()) {
		mop::hi::svc::adapter::UserAdapter::instance().setUserOnlineStats(offlineIds, false);
		//------------------------

		//mop::hi::svc::adapter::MainAdapter::instance().webLogout(offlineIds);

		//------------------------


		ostringstream offlineSql;
		for (uint i = 0; i< offlineIds.size(); ++i) {
			offlineSql << offlineIds[i];
			if (i != offlineIds.size()-1) {
				offlineSql << " , ";
			}
		}
		try {
			ConnectionQuery conn1(OnlineManagerI::instance().getOnlineDb());
			conn1.doDelete(
					"delete from online where id in ( " + offlineSql.str() + ")");
		} catch(mysqlpp::Exception& e) {
			MCE_WARN("Error: " << e.what());
		} catch (Ice::Exception& e) {
			MCE_WARN("Error: " << e);
		} catch (...) {
			MCE_WARN("Error: ");
		}
		try {
			ConnectionQuery conn1(OnlineManagerI::instance().getOnlineDb());
			conn1.doDelete(
					"delete from online where id in ( " + offlineSql.str() + ")");
			ConnectionQuery conn2(OnlineManagerI::instance().getImOnlineDb());
			conn2.doDelete(
					"delete from im_online where id in ( " + offlineSql.str() + ")");
		} catch(mysqlpp::Exception& e) {
			MCE_WARN("Error: " << e.what());
		} catch (Ice::Exception& e) {
			MCE_WARN("Error: " << e);
		} catch (...) {
			MCE_WARN("Error: ");
		}
	}
	if (!onlineIds.empty()) {
		mop::hi::svc::adapter::UserAdapter::instance().setUserOnlineStats(onlineIds, true);
		OnlineManagerI::instance().getActiveUserManager()->addActiveUsers(onlineIds);

		ostringstream sql;
		sql << "select id, name, stage, univ, univ_year, "
			"home_province, home_city, "
			"headurl, gender, selected "
			"from user_basic where id in (";
		for (uint i = 0; i < onlineIds.size(); ++i) {
			sql << onlineIds[i];
			if (i != onlineIds.size() - 1) {
				sql << ",";
			}
		}
		sql << ")";

		ConnectionQuery conn0("common");
		DefResSetPtr drs = DefResSetPtr::dynamicCast(conn0.doSelect(sql.str()));
		ConnectionHolder conn1(OnlineManagerI::instance().getImOnlineDb());
		mysqlpp::Query query1 = conn1.query();

		ConnectionQuery conn2(OnlineManagerI::instance().getOnlineDb());
		mysqlpp::Query query2 = conn2.query();
		query1 << "replace into im_online (" <<"id," <<"type, " <<"name,"
				<<"stage," <<"univ," <<"univ_year," <<"home_province,"
				<<"home_city," <<"headurl," <<"gender," <<"selected) values ";

		query2 << "replace into online (id,type,stage,network_id) values";
		for (size_t row = 0; row < drs->size(); ++row) {
			OnlineUserPtr u = onlineChange[drs->getField<int>(row, "id")];

			query1 << "(" << mysqlpp::quote << u->userId << ","
					<< mysqlpp::quote << u->onlineStat << "," << mysqlpp::quote
					<< drs->getField(row, "name") << "," << mysqlpp::quote
					<< drs->getField(row, "stage") << "," << mysqlpp::quote
					<< drs->getField(row, "univ") << "," << mysqlpp::quote
					<< drs->getField(row, "univ_year") << "," << mysqlpp::quote
					<< drs->getField(row, "home_province") << ","
					<< mysqlpp::quote << drs->getField(row, "home_city") << ","
					<< mysqlpp::quote << drs->getField(row, "headurl") << ","
					<< mysqlpp::quote << drs->getField(row, "gender") << ","
					<< mysqlpp::quote << drs->getField(row, "selected") << ")";

			query2 << " ( " << u->userId << " , " << u->onlineStat << " , "
					<< mysqlpp::quote << drs->getField(row, "stage") << ","
					<< mysqlpp::quote << drs->getField(row, "univ") << ")";

			if (row != drs->size() - 1) {
				query1 << ",";
				query2 << ",";
			}
		}
		try {
			query1.execute();
		} catch(mysqlpp::Exception& e) {
			MCE_WARN("Error: " << e.what());
		} catch (Ice::Exception& e) {
			MCE_WARN("Error: " << e);
		} catch (...) {
			MCE_WARN("Error: ");
		}
		try {
			query2.execute();
		} catch(mysqlpp::Exception& e) {
			MCE_WARN("Error: " << e.what());
		} catch (Ice::Exception& e) {
			MCE_WARN("Error: " << e);
		} catch (...) {
			MCE_WARN("Error: ");
		}

	}
}

