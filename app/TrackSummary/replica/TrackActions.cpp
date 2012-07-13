#include "ServiceI.h"
#include "TrackActions.h"

using namespace ::xce::activetrack;
using namespace ::MyUtil;
using namespace ::xce::notifysender::adapter;
using namespace ::xce::aiguidecache::adapter;

void LastURLAction::execute(Ice::Int userId,std::string& ip,std::string& url,TrackPtr userTrack){
	MCE_DEBUG( "[LastURLAction]: execute(" << userId << ", " << url << ")" );
	userTrack->set("LastURL",url);
}

std::string AIGuideAction::_FARMGAMEURL = "http://apps.kaixin.com/farmgame/";
std::string AIGuideAction::_DOGURL = "http://dog.kaixin.com";
std::string AIGuideAction::_CARKPARKURL = "http://carpark.kaixin.com";

/* 开心水族箱替换黑社会;荣光医院替换超级大亨  -- 09.06.19 by zhanghan */
// std::string AIGuideAction::_GODFATHERURL = "http://gf.kaixin.com";
// std::string AIGuideAction::_TYCOONURL = "http://tycoon.kaixin.com";
std::string AIGuideAction::_GODFATHERURL = "http://apps.kaixin.com/fishbowl/";
std::string AIGuideAction::_TYCOONURL = "http://apps.kaixin.com/ronhospital/";

AIGuideAction::AIGuideAction(Ice::PropertiesPtr props):
	_startID(props->getPropertyAsIntWithDefault("Actions.AIGuide.startID",0)){
}

void AIGuideAction::execute(Ice::Int userId,std::string& ip,std::string& url,TrackPtr userTrack){
	if( userId<_startID){
		MCE_DEBUG( "[AIGuideAction]: not execute(" << userId << ")" );
		return;
	}
	if(url=="http://apps.kaixin.com/getMessageInfo.do"){
		return;
	}
	MCE_DEBUG( "[AIGuideAction]: execute(" << userId << ", " << url << ")" );
	std::map<std::string,std::string> track = userTrack->get();
	std::string value;
	std::map<std::string,std::string>::iterator it = track.find("AIGuide");
	if(it==track.end()){
		userTrack->set( "AIGuide", "P" );
//		TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::FRAMGAME));
//		TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::DOG));
	}else{
		value = it->second;
	}
	int urlCode = getURLCode(url);
//	MCE_DEBUG( "[AIGuideAction]: getMiniURL = " << urlCode );
    	if(value=="P"){
    		if(urlCode==0){
        		userTrack->set( "AIGuide", "P_FARM_A" );
    		}else if(urlCode==1){
        		userTrack->set( "AIGuide", "P_DOG_A" );
    		}
    	} else if((value=="P_FARM_A") && (urlCode!=0)){
    		userTrack->set( "AIGuide", "P_FARM_B" );
    		// TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::DOG));
    		TaskManager::instance().execute(new AIGuideCacheTask(userId, xce::aiguidecache::DOG));
    	} else if((value=="P_FARM_B") && (urlCode==1)){
        	userTrack->set( "AIGuide", "DOG_A" );
	} else if((value=="DOG_A") && (urlCode!=1)){
		userTrack->set( "AIGuide", "DOG_B" );
    		// TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::CARPARK));
		TaskManager::instance().execute(new AIGuideCacheTask(userId, xce::aiguidecache::CARPARK));
    	} else if((value=="P_DOG_A") && (urlCode!=1)){
    		userTrack->set( "AIGuide", "P_DOG_B" );
    		// TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::FRAMGAME));
		TaskManager::instance().execute(new AIGuideCacheTask(userId, xce::aiguidecache::FARMGAME));
    	} else if((value=="P_DOG_B") && (urlCode==0)){
    		userTrack->set( "AIGuide", "FARM_A" );
    	} else if((value=="FARM_A") && (urlCode!=0)){
    		userTrack->set( "AIGuide", "FARM_B" );
		// TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::CARPARK));
		TaskManager::instance().execute(new AIGuideCacheTask(userId, xce::aiguidecache::CARPARK));
	} else if((value=="DOG_B"||value=="FARM_B") && (urlCode==2)){
		userTrack->set( "AIGuide", "CARPARK_A" );
	} else if((value=="CARPARK_A") && (urlCode!=2)){
		userTrack->set( "AIGuide", "CARPARK_B" );
		// TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::GODFATHER));
		TaskManager::instance().execute(new AIGuideCacheTask(userId, xce::aiguidecache::GODFATHER));
	} else if((value=="CARPARK_B") && (urlCode==3)){
		userTrack->set( "AIGuide", "GOD_A" );
	} else if((value=="GOD_A") && (urlCode!=3)){
		userTrack->set( "AIGuide", "GOD_B" );
		// TaskManager::instance().execute(new SendNotifyTask(userId,com::xiaonei::wService::TYCOON));	
		TaskManager::instance().execute(new AIGuideCacheTask(userId, xce::aiguidecache::TYCOON));
	} else if((value=="GOD_B") && (urlCode==4)){
        	userTrack->set( "AIGuide", "TYCOON_A" );
    	} else if((value=="TYCOON_A") && (urlCode!=4)){
        	userTrack->set( "AIGuide", "TYCOON_B" );
    	} else if(value=="TYCOON_B"){
//		MCE_DEBUG("At CARPARK_B");
	}

	MCE_DEBUG("[AIGuideStatus]" << userId << " : " << value);
}


int AIGuideAction::getURLCode(std::string& url){
	if(startsWith(url,_FARMGAMEURL)){
		return 0;
	}else if(startsWith(url,_DOGURL)){
		return 1;
	}else if(startsWith(url,_CARKPARKURL)){
		return 2;
	}else if(startsWith(url,_GODFATHERURL)){
		return 3;
	}else if(startsWith(url,_TYCOONURL)){
		return 4;
	}else{
		return 5;
	}
}
bool AIGuideAction::startsWith(std::string& url, std::string& prefix) {
	int uc = url.size();
	int pc = prefix.size();
	int uo = 0;
	int po = 0;
	if (uc - pc < 0) {
		return false;
	}
	while (--pc >= 0) {
		if (url[uo++] != prefix[po++]) {
			return false;
		}
	}
	return true;
}

void AIGuideCacheTask::handle() {
    MCE_DEBUG( "[AIGuideCacheTask]: changeAIGuideStatus(" << _userId << ", " << _appId << ",1" << ")" );

    AIGuideCacheAdapter::instance().set(_userId, _appId, true);
}

void SendNotifyTask::handle() {
    MCE_DEBUG( "[SendNotifyTask]: sendByAIGuideStage(" << _id << ", " << _stage << ")" );
    NotifySenderAdapter::instance().sendByAIGuideStage(_id, _stage);
}
