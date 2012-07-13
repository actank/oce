#include "TipsManagerI.h"
#include "app/Tips/share/TipsUtil.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"
#include "OceCxxAdapter/src/UserBornAdapter.h"
#include "OceCxxAdapter/src/UserStageAdapter.h"
#include "OceCxxAdapter/src/UserDescAdapter.h"
#include "OceCxxAdapter/src/UserNetworkAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::tps;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::QueryRunner;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::CDbRServer;
using xce::log::ActiveTrack;
using MyUtil::ServiceI;
using xce::usercache::UserCacheAdapter;
using xce::usercache::UserCachePtr;
using xce::adapter::userdesc::UserDescAdapter;
using ::xce::adapter::userstage::UserStageAdapter;
using ::xce::adapter::userborn::UserBornAdapter;
using mop::hi::svc::adapter::UserNetworkAdapter;

// ------- MyUtil::initialize ------------------------- 
void MyUtil::initialize() {

	TipsManagerI::instance().initialize();

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(UK, "UK", new UserActionFactory);
	service.registerObjectCache(UI, "UI", new UserInfoFactory);
	service.registerObjectCache(UA, "UA", new UserAppFactory);
	service.registerObjectCache(UC, "UC", new UserCompleteFactory);

	service.getAdapter()->add(&TipsManagerI::instance(), service.createIdentity("M", ""));
	ActiveTrack::instance().initialize();

	// 加入向Controller注册
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Tips.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Tips.Interval", 5);

	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerTips", 
			&TipsManagerI::instance(), mod, interval);

}	

// -------- TipsIndex 搜索类 ------------------------
TipsIndex::TipsIndex(const TipsListPtr tipss): 	
	tipss_(tipss) 	{

	// -- 只允许在构造时构造一次索引，之后不许再对索引进行改动, 从而保证在搜索过程中不需加锁 --	
	withApp_ = false;
	// 构建正排索引
	for (map<int, TipsPtr>::const_iterator it = tipss_->listMap.begin(); 
			it != tipss_->listMap.end(); ++it) {
		if ( !(*it).second->online ) {
			continue;
		}
		if ( (*it).second->onlinetime.size() > 0 ) {
			time_t now = time(NULL);
			bool inTime = false;
			for (TimeScope::iterator itT = (*it).second->onlinetime.begin();
					itT != (*it).second->onlinetime.end(); ++ itT ) {
				if ( (now >= ((*itT)->beginTime-(CREATE_INDEX_INTERVAL/2))) 
						&& (now <= ((*itT)->endTime-1)) ) { 	// 前后半个刻度的都加进来
					inTime = true;
					break;
				}
			}
			if ( ! inTime ) {
				continue;
			}
		}
		if ( ((*it).second->appuse.size() > 0) || ((*it).second->appunuse.size()>0) ) {
			withApp_ = true;
		}
		forwardIndex_.insert( pair<int, TipsPtr>((*it).first, (*it).second) );
	}
	MCE_INFO("[TipsIndex::TipsIndex] forward index size : " << forwardIndex_.size());

	// 构建倒排索引
	// website, star, vip, gender, idmods, idgiven, stage
	map<int, vector<int> > mapWebsite, mapStar, mapVip, mapGender, mapStage;
	for ( map<int, TipsPtr>::const_iterator it = forwardIndex_.begin();
			it != forwardIndex_.end(); ++it ) {
		TipsUtil::instance().insertMapVec(mapWebsite, (*it).second->website, (*it).first);
		TipsUtil::instance().insertMapVec(mapStar, (*it).second->star, (*it).first);
		TipsUtil::instance().insertMapVec(mapVip, (*it).second->vip, (*it).first);
		TipsUtil::instance().insertMapVec(mapGender, (*it).second->gender, (*it).first);
		// 将idmods由倒排索引转至过滤部分
		/* if ( (*it).second->idmods.size() <= 0 ) {
			TipsUtil::instance().insertMapVec(mapIdmods, -1, (*it).first);
		} else {
			for ( size_t i = 0; i < (*it).second->idmods.size(); ++i ) {
				TipsUtil::instance().insertMapVec(mapIdmods, (*it).second->idmods[i], (*it).first);
			}
		} */
		map<string, vector<int> >::iterator itStage = (*it).second->ctxs.find(CTXUSERSTAGE);
		if ( itStage != (*it).second->ctxs.end() ) {
			for ( size_t i = 0; i < (*itStage).second.size(); ++i ) {
				TipsUtil::instance().insertMapVec(mapStage, (*itStage).second[i], (*it).first);
			}
		} else {
			TipsUtil::instance().insertMapVec(mapStage, -1, (*it).first);
		}
	}
	invertedIndex_.insert( pair<int, map<int, vector<int> > >(QUERY_FIELD_WEBSITE, mapWebsite) );
	invertedIndex_.insert( pair<int, map<int, vector<int> > >(QUERY_FIELD_STAR, mapStar) );
	invertedIndex_.insert( pair<int, map<int, vector<int> > >(QUERY_FIELD_VIP, mapVip) );
	invertedIndex_.insert( pair<int, map<int, vector<int> > >(QUERY_FIELD_GENDER, mapGender) );
	// invertedIndex_.insert( pair<int, map<int, vector<int> > >(QUERY_FIELD_IDMODS, mapIdmods) );
	invertedIndex_.insert( pair<int, map<int, vector<int> > >(QUERY_FIELD_STAGE, mapStage) );
}

TipsPtr TipsIndex::get(int tipsid) {
	MCE_INFO("[TipsIndex::get] get tips of id : " << tipsid);
	map<int, TipsPtr>::iterator it =  tipss_->listMap.find(tipsid);
	if ( it != tipss_->listMap.end() ) {
		return (*it).second;
	} else {
		return NULL;
	}
}

TipsSeq TipsIndex::search(int userid, int website) {

	MCE_DEBUG("[TipsIndex::search] search tips for " << userid);
	TipsSeq results;

	// 0. 获取用户信息	
	UserInfoPtr userinfo = NULL;
	try {
		userinfo = ServiceI::instance().findObject<UserInfoPtr>(UI, long(userid));
		MCE_DEBUG("[TipsIndex::search] find info of user " << userid);
	} catch (...) {
		MCE_WARN("[TipsIndex::search] find userinfo " << userid << " exception ");
		return results;
	}	
	if ( userinfo ) {
		if ( userinfo->error ) {
			try {
				ServiceI::instance().getObjectCache()->removeObject(UI, userid);
				userinfo = ServiceI::instance().locateObject<UserInfoPtr>(UI, long(userid));
				MCE_DEBUG("[TipsIndex::search] get info of user " << userid);
			} catch (...) {
				MCE_WARN("[TipsIndex::search] relocate userinfo " << userid << " exception ");
				return results;
			}
		} 
	} else {
		try {
			userinfo = ServiceI::instance().locateObject<UserInfoPtr>(UI, long(userid));
			MCE_DEBUG("[TipsIndex::search] get info of user " << userid);
		} catch (...) {
			MCE_WARN("[TipsIndex::search] locate userinfo " << userid << " exception ");
			return results;
		}
	}	

	// I. 搜索
	vector< vector<int> > mays;
	// website	
	vector<int> websiteSeq;
	map<int, vector<int> >::iterator itWebsite = invertedIndex_[QUERY_FIELD_WEBSITE].find(website);
	map<int, vector<int> >::iterator itWebsite1 = invertedIndex_[QUERY_FIELD_WEBSITE].find(ALL);
	if ( (itWebsite != invertedIndex_[QUERY_FIELD_WEBSITE].end()) &&
		(itWebsite1 != invertedIndex_[QUERY_FIELD_WEBSITE].end()) ) {
		TipsUtil::instance().merge_union(websiteSeq, (*itWebsite).second, (*itWebsite1).second);
	} else if ( itWebsite != invertedIndex_[QUERY_FIELD_WEBSITE].end() ) {
		websiteSeq.insert(websiteSeq.begin(), (*itWebsite).second.begin(), (*itWebsite).second.end());
	} else if ( itWebsite1 != invertedIndex_[QUERY_FIELD_WEBSITE].end() ) {
		websiteSeq.insert(websiteSeq.end(), (*itWebsite1).second.begin(), (*itWebsite1).second.end());
	} else {
		return results;
	}
	mays.push_back(websiteSeq);
	// star
	vector<int> starSeq;
	map<int, vector<int> >::iterator itStar = invertedIndex_[QUERY_FIELD_STAR].find(userinfo->star);
	map<int, vector<int> >::iterator itStar1 = invertedIndex_[QUERY_FIELD_STAR].find(ALL);
	if ( (itStar != invertedIndex_[QUERY_FIELD_STAR].end())
		&& (itStar1 != invertedIndex_[QUERY_FIELD_STAR].end()) ) {
		TipsUtil::instance().merge_union(starSeq, (*itStar).second, (*itStar1).second);
	} else if ( itStar != invertedIndex_[QUERY_FIELD_STAR].end() ) {
		starSeq.insert(starSeq.begin(), (*itStar).second.begin(), (*itStar).second.end());
	} else if ( itStar1 != invertedIndex_[QUERY_FIELD_STAR].end() ) {
		starSeq.insert(starSeq.end(), (*itStar1).second.begin(), (*itStar1).second.end());
	} else {
		return results;
	}
	mays.push_back(starSeq);
	// vip
	vector<int> vipSeq;
	map<int, vector<int> >::iterator itVip = invertedIndex_[QUERY_FIELD_VIP].find(userinfo->vip);
	map<int, vector<int> >::iterator itVip1 = invertedIndex_[QUERY_FIELD_VIP].find(ALL);
	if ( (itVip != invertedIndex_[QUERY_FIELD_VIP].end())
		&& (itVip1 != invertedIndex_[QUERY_FIELD_VIP].end()) ) {
		TipsUtil::instance().merge_union(vipSeq, (*itVip).second, (*itVip1).second);
	} else if ( itVip != invertedIndex_[QUERY_FIELD_VIP].end() ) {
		vipSeq.insert(vipSeq.begin(), (*itVip).second.begin(), (*itVip).second.end());
	} else if ( itVip1 != invertedIndex_[QUERY_FIELD_VIP].end() ) {
		vipSeq.insert(vipSeq.end(), (*itVip1).second.begin(), (*itVip1).second.end());
	} else {
		return results;
	}
	mays.push_back(vipSeq);
	// gender
	vector<int> genderSeq;
	map<int, vector<int> >::iterator itGender = invertedIndex_[QUERY_FIELD_GENDER].find(userinfo->gender);
	map<int, vector<int> >::iterator itGender1 = invertedIndex_[QUERY_FIELD_GENDER].find(ALL);
	if ( (itGender != invertedIndex_[QUERY_FIELD_GENDER].end())
		&& (itGender1 != invertedIndex_[QUERY_FIELD_GENDER].end()) ) {
		TipsUtil::instance().merge_union(genderSeq, (*itGender).second, (*itGender1).second);
	} else if ( itGender != invertedIndex_[QUERY_FIELD_GENDER].end() ) {
		genderSeq.insert(genderSeq.begin(), (*itGender).second.begin(), (*itGender).second.end());
	} else if ( itGender1 != invertedIndex_[QUERY_FIELD_GENDER].end() ) {
		genderSeq.insert(genderSeq.end(), (*itGender1).second.begin(), (*itGender1).second.end());
	} else {
		return results;
	}
	mays.push_back(genderSeq);
	// idmods
	/* vector<int> idmodsSeq;
	map<int, vector<int> >::iterator itIdmods = invertedIndex_[QUERY_FIELD_IDMODS].find(userinfo->idmod);
	map<int, vector<int> >::iterator itIdmods1 = invertedIndex_[QUERY_FIELD_IDMODS].find(ALL);
	if ( (itIdmods != invertedIndex_[QUERY_FIELD_IDMODS].end()) 
		&& (itIdmods1 != invertedIndex_[QUERY_FIELD_IDMODS].end()) ) {
		TipsUtil::instance().merge_union(idmodsSeq, (*itIdmods).second, (*itIdmods1).second);
	} else if ( itIdmods != invertedIndex_[QUERY_FIELD_IDMODS].end() ) {
		idmodsSeq.insert(idmodsSeq.begin(), (*itIdmods).second.begin(), (*itIdmods).second.end());
	} else if ( itIdmods1 != invertedIndex_[QUERY_FIELD_IDMODS].end() ) {
		idmodsSeq.insert(idmodsSeq.end(), (*itIdmods1).second.begin(), (*itIdmods1).second.end());
	} else {
		return results;
	}
	mays.push_back(idmodsSeq); */
	// stage
	vector<int> stageSeq;
	map<int, vector<int> >::iterator itStage = invertedIndex_[QUERY_FIELD_STAGE].find(userinfo->stage);
	map<int, vector<int> >::iterator itStage1 = invertedIndex_[QUERY_FIELD_STAGE].find(ALL);
	if ( (itStage != invertedIndex_[QUERY_FIELD_STAGE].end())
		&& (itStage1 != invertedIndex_[QUERY_FIELD_STAGE].end()) ) {
		TipsUtil::instance().merge_union(stageSeq, (*itStage).second, (*itStage1).second);
	} else if ( itStage != invertedIndex_[QUERY_FIELD_STAGE].end() ) {
		stageSeq.insert(stageSeq.begin(), (*itStage).second.begin(), (*itStage).second.end());
	} else if ( itStage1 != invertedIndex_[QUERY_FIELD_STAGE].end() ) {
		stageSeq.insert(stageSeq.end(), (*itStage1).second.begin(), (*itStage1).second.end());
	} else {
		return results;
	}
	mays.push_back(stageSeq);

	// merge them
	vector<int> merged;
	TipsUtil::instance().merge_intersection(merged, mays);

	// II. 过滤	
	if ( merged.size() <= 0 ) {
		return results;
	}
	vector<TipsPtr> filted;
	bool needApp = false;
	bool needComplete = false;
	for ( vector<int>::iterator it = merged.begin(); it != merged.end(); ++it ) {
		map<int, TipsPtr>::iterator itTps = forwardIndex_.find(*it);
		if ( itTps == forwardIndex_.end() ) {
			continue;
		}
		if ( !(*itTps).second ) {
			continue;
		}
		// idgiven
		if ( (*itTps).second->idgiven.size() > 0 ) {
			bool match = false;
			for ( size_t i = 0; i< (*itTps).second->idgiven.size(); ++i ) {
				if ( userid == (*itTps).second->idgiven[i] ) {
					match = true;
					break;
				}
			}
			if ( !match ) {
				continue;
			}
		}
		// idmods	-- 将idmods改放在过滤部分 2010.7.6 by han.zhang
		if ( (*itTps).second->idmods.size() > 0 ) {
			bool match = false;
			int moddiv = TipsUtil::instance().getModDiv((*itTps).second);
			if ( moddiv <= 0 ) {
				continue;
			}
			int mod = userid % moddiv;

			if ( userid == 222525090 ) {
				MCE_INFO("[Tips::222525090] moddiv:" << moddiv << " mod:" << mod);
			}

			for ( size_t i = 0; i < (*itTps).second->idmods.size(); ++i ) {

				MCE_INFO("[Tips::222525090] mod[" << i << "]:" << (*itTps).second->idmods[i]);

				if ( mod == (*itTps).second->idmods[i] ) {
					match = true;
					break;
				}
			}
			if ( ! match ) {
				continue;
			}
		}
		// ages
		if ( (*itTps).second->ages.size() > 0 ) {
			bool match = false;
			for (size_t i = 0; i < (*itTps).second->ages.size(); ++i ) {
				if ( (userinfo->age >= (*itTps).second->ages[i]->minAge) 
					&& (userinfo->age <= (*itTps).second->ages[i]->maxAge) ) {
					match = true;
					break;
				}
			}
			if ( !match ) {
				continue;
			}
		}

		// 指定大用户集(interest)过滤
		if ( ! TipsManagerI::instance().isInterested((*itTps).second->tipsid, userid) ) {
			continue;
		}

		// 加入过滤结果
		filted.push_back((*itTps).second);

		// 判断是否需要app及complete条件
		if ( ((*itTps).second->appuse.size() > 0)
			|| ((*itTps).second->appunuse.size() >0 ) ) {
			needApp = true;
		}
		if ( ((*itTps).second->completes.size() >0)
			|| ((*itTps).second->uncompletes.size()>0) ) {
			needComplete = true;
		}
	}

	// III.App过滤 & Complete过滤
	if ( (!needApp) && (!needComplete) ) {
		results.insert(results.begin(), filted.begin(), filted.end());
		return results;
	}
	UserAppPtr userapp = NULL;
	UserCompletePtr usercomplete = NULL;
	if ( needApp ) {
		try {
			userapp = ServiceI::instance().locateObject<UserAppPtr>(UA, long(userid));
		} catch (...) {
			MCE_WARN("[TipsIndex::search] locate userapp " << userid << " exception ");
			needApp = false;
		}
	}
	if ( needComplete ) {
		try {
			usercomplete = ServiceI::instance().locateObject<UserCompletePtr>(UC, long(userid));
		} catch (...) {
			MCE_WARN("[TipsIndex::search] locate complete " << userid << " exception ");
			needComplete = false;
		}
	}
	// filting
	for ( vector<TipsPtr>::iterator it = filted.begin(); it != filted.end(); ++it ) {
		if ( needApp ) {
			if ( (*it)->appunuse.size() > 0 ) {
				bool bBend = false;
				for ( size_t i = 0; i < (*it)->appunuse.size(); ++i ) {
					if ( userapp->apps.find((*it)->appunuse[i]) != userapp->apps.end() )  {
						bBend = true;
						break;
					}
				}
				if ( bBend ) {
					continue;
				}
			}
			if ( (*it)->appuse.size() > 0 ) {
				bool bFind = false;
				for ( size_t i = 0; i < (*it)->appuse.size(); ++i ) {
					if ( userapp->apps.find((*it)->appuse[i]) != userapp->apps.end() ) {
						bFind = true;
						break;
					}
				}
				if ( ! bFind ) {
					continue;
				}
			}
		}

		if ( needComplete ) {
			if ( (*it)->uncompletes.size() > 0 ) {
				bool bBend = false;
				for ( size_t i = 0; i < (*it)->uncompletes.size(); ++i ) {
					if ( usercomplete->completes.find((*it)->uncompletes[i]) 
							!= usercomplete->completes.end() ) {
						bBend = true;
						break;
					}
				}
				if ( bBend ) {
					continue;
				}
			}
			if ( (*it)->completes.size() > 0 ) {
				bool bFind = false;
				for ( size_t i = 0; i < (*it)->completes.size(); ++i ) {
					if ( usercomplete->completes.find((*it)->completes[i]) 
							!= usercomplete->completes.end()) {
						bFind = true;
						break;
					}
				}
				if ( !bFind ) {
					continue;
				}
			}
		}

		results.push_back(*it);		
	}

	return results;

}

bool TipsIndex::compare(const TipsIndexPtr index2) {
	
	map<int, TipsPtr>& forwardIndex2 = index2->getForwardIndex();

	if ( forwardIndex_.size() != forwardIndex2.size() ) {
		return false;
	}

	map<int, TipsPtr>::iterator it1 = forwardIndex_.begin();
	map<int, TipsPtr>::iterator it2 = forwardIndex2.begin();
	while ( it1!= forwardIndex_.end() ) {
		if ( (*it1).first != (*it2).first ) {
			return false;
		}
		++it1;
		++it2;
	}
	return true;
}

map<int, TipsPtr>& TipsIndex::getForwardIndex() {
	return forwardIndex_;
}


// -------- TipsManagerI 主类 -----------------------
void TipsManagerI::initialize() {

	createThread_.start(128*1024).detach();

	// 建立索引
	renew(true);
}

TipsPtr TipsManagerI::show(::Ice::Int userid, ::Ice::Int website,
                                 const Ice::Current& current)
{
	MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website);

	// 复制一个index的指针,为了保证其在搜索完成前不会被销毁
	TipsIndexPtr index = index_;
	TipsSeq seq = index->search(userid, website);
	MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website << " tipsseq.size=" << seq.size());

	// 去掉用户过去点击或关闭过的
	UserActionPtr useraction = locateAction(userid);
	TipsSeq unclickedseq;
	for ( size_t i = 0; i < seq.size(); ++i ) {
		if ( ! useraction->denied(seq[i]->tipsid) ) {
			unclickedseq.push_back(seq[i]);
		}
	}

	MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website << " unclickedseq.size=" << unclickedseq.size());
	if ( unclickedseq.size() <= 0 ) {
		return TipsUtil::instance().emptyTips();
	}

	// 按weight进行概率选择
	int resultPos = 0;
	vector<int> probs;
	probs.push_back(unclickedseq[0]->weight);
	for ( size_t i = 1; i < unclickedseq.size(); ++i ) {
		probs.push_back(probs[i-1]+unclickedseq[i]->weight);
	}
	int max = probs[probs.size()-1];
	MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website << " totalweight=" << max);
	if ( max <= 0 ) {
		// 对于均为0的情况
		return TipsUtil::instance().emptyTips();
	}
	int seed = (rand() % max);
	
	if ( seed < probs[0] ) {
		resultPos = 0;
	} else {
		for ( size_t i = 1; i < unclickedseq.size(); ++i ) {
			if ( (seed >= probs[i-1]) && (seed < probs[i]) ) {
				resultPos = i;
				break;
			}			
		}
	}

	// 对选出的公告增加该用户的查看数
	if (unclickedseq[resultPos]->maxshow > 0 ) {
		int newcount = useraction->add_count(unclickedseq[resultPos]->tipsid, unclickedseq[resultPos]->maxshow);
		MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website << " tips=" << unclickedseq[resultPos]->tipsid << " count:" << newcount );
		if ( newcount < 0 ) {
			UpdateClickInDB(userid, unclickedseq[resultPos]->tipsid);
		}
	} else {
		MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website << " tips=" << unclickedseq[resultPos]->tipsid << " maxshow:" << unclickedseq[resultPos]->maxshow );
	}

	// 记录远程log
	ostringstream actionObject;
	actionObject<<"TIPS_" << unclickedseq[resultPos]->tipsid;
	ActiveTrack::instance().action(userid, TIPS_SHOW_LOG_PATH, "VISIT", actionObject.str());

	MCE_INFO("[TipsManagerI::show] uid=" << userid << " site=" << website << " tips=" << unclickedseq[resultPos]->tipsid );
	return unclickedseq[resultPos];
}

TipsPtr TipsManagerI::get(::Ice::Int tipsid,
                                const Ice::Current& current) {
	// 复制一个tipss的指针,为了保证其在搜索完成前不会被销毁
	TipsListPtr tipss = tipss_;
	map<int, TipsPtr>::iterator it = tipss->listMap.find(tipsid);
	if ( it != tipss->listMap.end() ) {
		return (*it).second;
	} else {
		return TipsUtil::instance().emptyTips();
	}
}

void TipsManagerI::renew(bool rebuildIndex, const Ice::Current& current) {

	TipsListPtr tipss = loadTipss();
	minTipsId_ = tipss->minTipsId;
	MCE_INFO("[TipsManagerI::renew] reload tipss. size : " << tipss->listMap.size());
	reloadTipsInterest(tipss, false);
	if ( tipss ) {
		tipss_ = tipss;

		if ( rebuildIndex ) {
			TipsIndexPtr index = createIndex(tipss);
			if ( index ) {
				{
					IceUtil::Mutex::Lock lock(mutexRenew_);
					// tipss_ = tipss;
					index_ = index;
					MCE_INFO("[TipsManagerI::renew] renew ok, " << tipss->listMap.size() << " tips(s)");
				}

			} else {
				MCE_WARN("[TipsManagerI::renew] renew error, can't create index");
			}
			
		}
	} else {
		MCE_WARN("[TipsManagerI::renew] renew error, can't load tipss from DB");
	}
}

void TipsManagerI::renewIndexOnly() {

	MCE_INFO("[TipsManagerI::renewIndexOnly] " << " renew index from tipss for online-time config");
	TipsListPtr tipss = tipss_;
	if ( ! tipss ) {
		return;
	}
	TipsIndexPtr indexNew = createIndex(tipss);
	if ( indexNew ) {	
		{	
			IceUtil::Mutex::Lock lock(mutexRenew_);
			if ( ! index_->compare(indexNew) ) {
				index_ = indexNew;
			}
		}
	}
}

void TipsManagerI::click(::Ice::Int userid, ::Ice::Int tipsid, bool beClose, const Ice::Current&) {

	MCE_INFO("[TipsManagerI::click] " << userid << " click " << tipsid << " close?" << beClose);
	// 打远程log
	ostringstream actionObject;
	actionObject<< "TIPS_" << tipsid;
	if(beClose)
		ActiveTrack::instance().action(userid, TIPS_CLOSE_LOG_PATH, "VISIT", actionObject.str());
	else
		ActiveTrack::instance().action(userid, TIPS_CLICK_LOG_PATH, "VISIT", actionObject.str());

	// 点击
	UserActionPtr useraction = locateAction(userid);
	if ( !useraction ) {
		return;
	}
	if ( useraction->denied(userid) ) {
		return;
	}
	// 查找persist字段，若大于0则不点击
	TipsIndexPtr index = index_;
	if ( (beClose) || (!index->get(tipsid)->persist) ) {
		// 点击记录, 更新数据库
		useraction->click(tipsid);
		UpdateClickInDB(userid, tipsid);
	}
}

void TipsManagerI::relocateUserCache(::Ice::Int userid,
		const Ice::Current& current)  {

	MCE_INFO("[TipsManagerI::relocateUserCache] remove UserInfo & UserApp Cache of " << userid);
	ServiceI::instance().getObjectCache()->removeObject(UI, userid);
	ServiceI::instance().getObjectCache()->removeObject(UA, userid);
	ServiceI::instance().getObjectCache()->removeObject(UC, userid);
	ServiceI::instance().getObjectCache()->removeObject(UK, userid);
}

TipsSeq TipsManagerI::list(const Ice::Current& current) {

	MCE_INFO("[TipsManagerI::list] list all tipss : size(" << tipss_->listMap.size() << ")");
	TipsSeq seq;
	for ( map<int, TipsPtr>::const_iterator it = tipss_->listMap.begin();
			it != tipss_->listMap.end(); ++it) {
		seq.push_back((*it).second);
	}
	MCE_DEBUG("[TipsManagerI::list] seq size(" << seq.size() << ")");
    	return seq;
}

TipsIndexPtr TipsManagerI::createIndex(const TipsListPtr& tipss) {

	if ( tipss ) {
		TipsIndexPtr index = new TipsIndex(tipss);
		return index;
	} else {
		return NULL;
	}
}

TipsListPtr TipsManagerI::loadTipss() {

	TipsListPtr tipss = new TipsList;

	// 从DB中读取所有公告
	Statement sql;
	// sql << "SELECT * from " << TABLE_TIPS << " ORDER BY " << FIELD_TIPSID << " ASC";
	sql << "SELECT * from " << TABLE_TIPS << " ORDER BY " << FIELD_TIPSID << " ASC";
	try {
		QueryRunner(DB_DESC_TIPS, CDbRServer).query(sql, TipsHandler(tipss->listMap));
	} catch (mysqlpp::Exception& e)  {
		MCE_WARN("[TipsManagerI::createIndex] read fullIndex from DB mysqlpp::exception : " << e.what());
		return NULL;
	} catch (...) {
		MCE_WARN("[TipsManagerI::createIndex] read fullIndex from DB exception");
		return NULL;
	}
	if ( tipss->listMap.size() > 0 ) {
		tipss->minTipsId = (*tipss->listMap.begin()).first;
	} else {
		tipss->minTipsId = 0;
	}
	return tipss;
}	

void TipsManagerI::UpdateClickInDB(int userId, int tipsId)	{
	
	ostringstream sql, pattern;
	Statement stat;
	pattern << TABLE_ACTION_RECORD << (userId%100);
	sql << "INSERT IGNORE INTO " << pattern.str() 
		<< "(" << FIELD_ACTION_USERID << "," << FIELD_ACTION_BULLTINID << ")"
		<< " VALUES(" << userId << "," << tipsId << ")";
	stat << sql.str();
	QueryRunner(DB_DESC_RECORD, CDbWServer, pattern.str()).schedule(stat);	
}	

UserActionPtr TipsManagerI::locateAction(Ice::Int userid)	{

	return ServiceI::instance().locateObject<UserActionPtr>(UK, userid);
}

TipsInterestPtr TipsManagerI::getTpsInterest(int tpsId) {
    map<int, TipsInterestPtr>::iterator it = tpsInterest.find(tpsId);
    if(it != tpsInterest.end()) {
	    return it->second;
    }
    else {
	    return NULL;
    }
}

void TipsManagerI::setTpsInterest(int tpsId, const TipsInterestPtr& interest) {
    if ( tpsInterest.find(tpsId) != tpsInterest.end() ) {
	    tpsInterest.erase(tpsId);
	    MCE_INFO("[TipsManagerI::setTpsInterest] earse TpsInterest of tips :" << tpsId);
    }
    tpsInterest.insert(make_pair<int, TipsInterestPtr>(tpsId, interest));
    MCE_INFO("[TipsManagerI::setTpsInterest] set TpsInterest of tips :" << tpsId);
}
void TipsManagerI::eraseTpsInterest(int tpsId) {
	tpsInterest.erase(tpsId);
}

// 重新加载1条公告的所有
void TipsManagerI::reloadTipsInterest(int tpsId, bool force, const Ice::Current&) {

	TipsListPtr tipss = tipss_;
	map<int, TipsPtr>::iterator it = tipss->listMap.find(tpsId);
	if ( it == tipss->listMap.end() ) {
		MCE_WARN("[reloadTipsInterest] do not contain tips : " + tpsId);
		return;
	}
	TipsPtr tps = (*it).second;
	string tableName = getInterestTable(tps);
	if ( tableName == "" ) {
		MCE_INFO("[reloadTipsInterest] clear TipsInterest of tips : " +tpsId);
		eraseTpsInterest(tpsId);
		return;
	}
	TipsInterestPtr newInterest = loadTipsInterest(tableName);
	setTpsInterest(tpsId, newInterest);
}

// 重新加载所有公告的
void TipsManagerI::reloadTipsInterest(TipsListPtr tipss, bool force) {

	// 获取新的需要加载TipsInterest的列表
	map<int, string> newTpsInterestMap;
	for(map<int, TipsPtr>::iterator it = tipss->listMap.begin(); it != tipss->listMap.end(); ++it) {
		string tableName = getInterestTable((*it).second);
		if ( tableName != "" ) {
			newTpsInterestMap.insert(make_pair<int, string>((*it).second->tipsid, tableName));
		}
	}

	// 比较,需要新增和更新的
	for ( map<int, string>::iterator it = newTpsInterestMap.begin(); it != newTpsInterestMap.end(); ++ it ) {
		int tipsid = (*it).first;
		TipsInterestPtr interest = getTpsInterest(tipsid);
		bool renew = false;
		if ( force ) {
			renew = true;
		} else {
			if ( ! interest ) {
				// 新增
				renew = true;
			} else if ( interest->getTableName() != (*it).second ) {
				// 改换db位置
				renew = true;
			}						
		}
		// 更新该id的TipsInterest
		if ( renew ) {
			TipsInterestPtr newInterest = loadTipsInterest((*it).second);
			setTpsInterest((*it).first, newInterest);			
		}
	}

	// 比较，需要清除的
	vector<int> toClear;
	for ( map<int, TipsInterestPtr>::iterator it = tpsInterest.begin(); it != tpsInterest.end(); ++it ) {
		int tipsid = (*it).first;
		if ( newTpsInterestMap.find(tipsid) == newTpsInterestMap.end() ) {
			// 清除该TipsInterestPtr
			toClear.push_back(tipsid);			
		}
	}
	for (size_t i = 0; i < toClear.size(); ++i ) {
		eraseTpsInterest(toClear[i]);
	}
}

TipsInterestPtr TipsManagerI::loadTipsInterest(const string& tableName) {

	TipsInterestPtr tpsInterest = new TipsInterest();
	tpsInterest->setTableName(tableName);

	int maxid = 0;
	int totalsize = 0;
	while(true) { 
		vector<int> results; 
		Statement sql; 
		sql << "select " << FIELD_TIPS_INTEREST << " from " << tableName << " where " << FIELD_TIPS_INTEREST << ">" << maxid << " limit " << MAX_RESULT_LINES << ";";
		TipsInterestResultHandler handler(results); 
		try {
			QueryRunner("tips_users", CDbRServer).query(sql, handler); 
		} catch(mysqlpp::Exception& e) {
			MCE_WARN("[TipsManagerI::reloadTipsInterest] read " << tableName << "." << FIELD_TIPS_INTEREST << " from DB mysqlpp::exception : " << e.what());	
			break;
		}
		int size = results.size();
		MCE_DEBUG("TipsManager::loadTipsInterest:" << tableName << "." << FIELD_TIPS_INTEREST << " " << size << " userids were set"); 
		if(size <= 0) { 
			break; 
		} 
		for(int i = 0; i < size; ++i) { 
			tpsInterest->set(results[i]); 
			if ( results[i] > maxid ) {
				maxid = results[i];
			}
		}
		totalsize += size;
	}
	MCE_INFO("[TipsManager::loadTipsInterest] load " << totalsize << " interest_users from talbe: " << tableName);
	return tpsInterest;
}

string TipsManagerI::getInterestTable(const TipsPtr& tps) {

	string tableName = ""; 
	MCE_DEBUG("[TipsManagerI::ctx2DataSource] " << " size of ctxs is  " << tps->ctxs.size());	
	for(map<string, ::Ice::IntSeq>::const_iterator it = tps->ctxs.begin(); it != tps->ctxs.end(); ++it) {
		string temp_str(it->first);
		if ( ( temp_str.length() >= FIELD_CTX_TPSINTEREST.length() ) 
			&&  (temp_str.substr(0, FIELD_CTX_TPSINTEREST.length()).compare(FIELD_CTX_TPSINTEREST) == 0) ) {

			size_t first_pos = temp_str.find("-.-");
			size_t last_pos = temp_str.length();
			if(first_pos != string::npos) {
				tableName = temp_str.substr(first_pos + 3, last_pos - first_pos - 3);
				return tableName;
			}
		}
	}
	return tableName;
}



// ---------- UserAction 部分 -------------------------
bool UserAction::denied(int tipsid)	{

	IceUtil::Mutex::Lock lock(mutex_);
	map<int, int>::iterator it = actions_.find(tipsid);
	if ( it != actions_.end() ) {
		if ( (*it).second < 0 ) {
			return true;
		}
	}
	return false;
	/* vector<int>::iterator itPos = lower_bound(clickedIds_.begin(), clickedIds_.end(), tipsid);
	if (itPos == clickedIds_.end() )	{		
		return false;
	}
	else if ( (*itPos) == tipsid ) {
		return true;
	}
	else {
		return false;
	} */
}

int UserAction::add_count(int tipsid, int maxShow) {
	
	IceUtil::Mutex::Lock lock(mutex_);
	map<int, int>::iterator it = actions_.find(tipsid);
	if ( it == actions_.end() ) {
		if(maxShow <= 1){
			actions_.insert(pair<int, int>(tipsid, -1));
		}else{
			actions_.insert(pair<int, int>(tipsid, 1));
		}
	} else if ( (*it).second >= 0 ) {
		(*it).second ++;
		if ( (maxShow > 0) && ((*it).second >= maxShow) ) {
			(*it).second = -1;
		}
	
	}
	return (*it).second;
}

void UserAction::click(int tipsid)	{

	MCE_DEBUG("UserAction::click tipsid :" << tipsid);
	IceUtil::Mutex::Lock lock(mutex_);

	map<int, int>::iterator it = actions_.find(tipsid);
	if ( it == actions_.end() ) {
		actions_.insert(pair<int, int>(tipsid, -1));
	} else {
		(*it).second = -1;
	}

	/* if ( clickedIds_.size() <= 0 )	{
		// 如果为空, 则直接加入
		clickedIds_.push_back(tipsid);
		return;
	} else 	{
		// 否则, 找到适宜位置插入，保持有序
		vector<int>::iterator itPos = lower_bound(clickedIds_.begin(), clickedIds_.end(), tipsid);
		if ( (*itPos) == tipsid ) {
			// 如果已有则不插入
			return;
		} else if ( itPos == clickedIds_.end() )  {
			clickedIds_.push_back(tipsid);
		} else {
			clickedIds_.insert(itPos, tipsid);
		}		
	} */
}

void  UserAction::add(int tipsid, int count) {
	IceUtil::Mutex::Lock lock(mutex_);
	actions_.insert(pair<int, int>(tipsid, count));
}

// ------------------- Thread 部分 -----------------------------
void CreateIndexThread::run() {
	while (true) {
		time_t now = time(NULL);
		// time_t des = ((now+1) / CREATE_INDEX_INTERVAL + 1) * CREATE_INDEX_INTERVAL -1;
		time_t des = (now / CREATE_INDEX_INTERVAL + 1) * CREATE_INDEX_INTERVAL;
		sleep(des-now);
		TipsManagerI::instance().renewIndexOnly();
	}
}

// ------------------- Factory部分 -----------------------------
Ice::ObjectPtr UserInfoFactory::create(Ice::Int userid)	{

	MCE_DEBUG("UserInfoFactory::create -> " << userid);
	
	// 默认初值
	UserInfoPtr user = new UserInfo;
	user->userid = userid;
	user->idmod = userid % 10;
	user->star = -1;
	user->vip = -1;
	user->gender = -1;
	user->age = 0;
	user->complete = -1;
	user->error = false;
	user->stage = -1;

	// 从UserCache中取信息
	try {
		vector<int> uids;
		uids.push_back(userid);
		UserCachePtr userCache = UserCacheAdapter::instance().GetUserCache(uids)[userid];
		if ( userCache ) {
			user->star = userCache->IsSelected()?1:0;
			user->vip = userCache->IsVipMember()?1:0;
			if ( userCache->gender().find("女生") != string::npos ) {
				user->gender = 0;
			} else {
				user->gender = 1;	
			}
		} else {
			MCE_WARN("[UserInfoFactory::create] getUserCache " << userid << " is NULL");
			user->error = true;
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserInfoFactory::create] getUserCache " << userid << " Ice::Excetpion : " << e.what());
		user->error = true;
	} catch (std::exception& e) {
		MCE_WARN("[UserInfoFactory::create] getUserCache " << userid << " std::excetpion : " << e.what());
		user->error = true;
	} catch (...) {
		MCE_WARN("[UserInfoFactory::create] getUserCache " << userid << " Excetpion");
		user->error = true;
	}

	// 从UserStage取Stage信息
	try {
		::xce::adapter::userstage::UserStageInfoPtr stageData = UserStageAdapter::instance().getUserStage(userid);
		int stageInt = stageData->stage();
		if (stageInt == 10) {
			user->stage = 1;
		} else if (stageInt == 20) {
			user->stage = 2;
		} else if (stageInt == 30 ) {
			user->stage = 3;
		} else {
			user->stage = 0;
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserInfoFactory::create] getUserStage Ice::Excetpion : " << e.what());
		user->error = true;
	} catch (std::exception& e) {
		MCE_WARN("[UserInfoFactory::create] getUserStage std::excetpion : " << e.what());
		user->error = true;
	} catch (...) {
		MCE_WARN("[UserInfoFactory::create] getUserStage Excetpion");
		user->error = true;
	}


	// 从UserBorn取年龄信息
	try {
		::xce::adapter::userborn::UserBornInfoPtr born = UserBornAdapter::instance().getUserBorn(userid);
		// age
		time_t timeStruct;  
		struct tm *p;  
		time(&timeStruct);  
		p = localtime(&timeStruct); 
		// 针对xx后特殊处理
    int bornyear = born->birthYear();
		if (born->birthYear() < 1900 ) {
			bornyear += 200;
		}
		int age = p->tm_year + 1900 - bornyear;
		if ( (p->tm_mon+1) < born->birthMonth() )	{
			age --;
		}
		else if ( ((p->tm_mon+1) == born->birthMonth()) && (p->tm_mday < born->birthDay()) )	{
			age --;
		}
		user->age = age;	

	} catch (Ice::Exception& e) {
		MCE_WARN("[UserInfoFactory::create] getUserBorn Ice::Excetpion : " << e.what());
		user->error = true;
	} catch (std::exception& e) {
		MCE_WARN("[UserInfoFactory::create] getUserBorn std::excetpion : " << e.what());
		user->error = true;
	} catch (...) {
		MCE_WARN("[UserInfoFactory::create] getUserBorn Excetpion");
		user->error = true;
	}

	return user;
}

Ice::ObjectPtr UserAppFactory::create(Ice::Int userid)	{

	MCE_DEBUG("UserAppFactory::create -> " << userid);

	UserAppPtr app = new UserApp;
	app->error = false;
	Statement sql;
	ostringstream pattern;
	pattern << TABLE_USERAPP << userid % 100;
	sql << "SELECT " << FIELD_USERAPP_APPID << " FROM " << pattern.str() << " WHERE " << FIELD_USERAPP_USERID << " = " << userid;
	try {
		QueryRunner(DB_DESC_USERAPP, CDbRServer).query(sql, UserAppResultHandler(app->apps));
	} catch (mysqlpp::Exception& e ) {
		MCE_WARN("[UserAppFactory::create] getUserApp mysqlpp::excetpion : " << e.what());
		app->error = true;
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserAppFactory::create] getUserApp Ice::Excetpion : " << e.what());
		app->error = true;
	} catch (std::exception& e) {
		MCE_WARN("[UserAppFactory::create] getUserApp std::excetpion : " << e.what());
		app->error = true;
	} catch (...) {
		MCE_WARN("[UserAppFactory::create] getUserApp Excetpion");
		app->error = true;
	}

	return app;
}

Ice::ObjectPtr UserCompleteFactory::create(Ice::Int userid)	{

	MCE_DEBUG("UserCompleteFactory::create -> " << userid);
	
	// 默认初值
	UserCompletePtr cpl = new UserComplete;
	cpl->error = false;

	// 从UserDesc中取信息
	try {
		xce::userdesc::UserDescDataPtr desc = UserDescAdapter::instance().getUserDesc(userid);
		if ( desc ) {
			// USERCONTACT, USERFAVOR
			if ( (desc->userContact->msn != "")
				|| (desc->userContact->mobilePhone != "") 
				|| (desc->userContact->fixPhone != "")
				|| (desc->userContact->qq > 0) 
				|| (desc->userContact->homepage != "") ) {
				cpl->completes.insert(USERCONTACT);
			}

			if ( desc->userFav->fav.find("userProfileItem") != string::npos ) {
				cpl->completes.insert(USERFAVOR);
			}
		} else {
			MCE_WARN("[UserCompleteFactory::create] getUserDesc of " << userid << " is null");
		}

	} catch (Ice::Exception& e) {
		MCE_WARN("[UserCompleteFactory::create] getUserDesc of " << userid << " Ice::Excetpion : " << e.what());
		cpl->error = true;
	} catch (std::exception& e) {
		MCE_WARN("[UserCompleteFactory::create] getUserDesc of " << userid << " std::excetpion : " << e.what());
		cpl->error = true;
	} catch (...) {
		MCE_WARN("[UserCompleteFactory::create] getUserDesc of " << userid << " Excetpion");
		cpl->error = true;
	}

	// 从UserNetwork中取信息
	try {
		mop::hi::svc::model::NetworkBigObjectNPtr network = UserNetworkAdapter::instance().getBigObjectN(userid);
		if ( network ) {

			// USERUNIV, USERCAREER
			if ( (network->highschools.size() > 0)
				|| (network->universitys.size() > 0)
				|| (network->collages.size() > 0) ) {
				cpl->completes.insert(USERUNIV);
			} else if ( (network->elementary) && (network->elementary->elementarySchoolName != "") ) {
				cpl->completes.insert(USERUNIV);
			} else if ( (network->juniorhighschool) && (network->juniorhighschool->juniorHighSchoolName !="") ) {
				cpl->completes.insert(USERUNIV);
			}
			if ( network->workspaces.size() > 0 ) {
				cpl->completes.insert(USERCAREER);
			}
		}
		else {
			MCE_WARN("[UserCompleteFactory::create] getUserNetwork of " << userid << " is null");
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserCompleteFactory::create] getUserNetwork of " << userid << " Ice::Excetpion : " << e.what());
		cpl->error = true;
	} catch (std::exception& e) {
		MCE_WARN("[UserCompleteFactory::create] getUserNetwork of " << userid << " std::excetpion : " << e.what());
		cpl->error = true;
	} catch (...) {
		MCE_WARN("[UserCompleteFactory::create] getUserNetwork of " << userid << " Excetpion");
		cpl->error = true;
	}

	return cpl;
}

Ice::ObjectPtr UserActionFactory::create(Ice::Int userid)	{

	MCE_DEBUG("UserActionFactory::create -> " << userid);
	ostringstream pattern0;
	Statement sql;
	pattern0 << TABLE_ACTION_RECORD << (userid%100);
	sql << "SELECT * FROM " << pattern0.str() << " WHERE " << FIELD_ACTION_USERID << "  = " << userid;
	int minTipsId = TipsManagerI::instance().getMinTipsId();
	if ( minTipsId > 0 ) {
		sql << " AND " << FIELD_ACTION_BULLTINID << " >= " << minTipsId;
	}

	UserActionPtr useraction = new UserAction(userid);
	try {
		QueryRunner(DB_DESC_RECORD, CDbRServer, pattern0.str())
			.query(sql, UserActionResultHandler(useraction));
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[UserActionFactory::create] create from db mysqlpp::exception : " << e.what());
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserActionFactory::create] create from db Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserActionFactory::create] create from db std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserActionFactory::create] create from db UNKNOWN exception");
	}

	return useraction;
}

// --------------- ResultHandler部分 --------------------------
bool UserAppResultHandler::handle(mysqlpp::Row& row) const {

	int appid = (int) row[FIELD_USERAPP_APPID.c_str()];
	apps_.insert(appid);

	return true;
}

bool TipsHandler::handle(mysqlpp::Row& row) const {

	const vector<string>& fields = TipsUtil::instance().getFieldList();
	map<string, string> mapTps;
	for ( vector<string>::const_iterator itField = fields.begin();
			itField != fields.end(); ++itField ) {
		try {
			string value = (string) row[(*itField).c_str()];
			mapTps.insert( pair<string, string>(*itField, value));
			//MCE_DEBUG("[ TipsHandler::handle] load field : " << (*itField) << " : " << value);
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("[TipsHandler::handle] read " << (*itField) << " from db mysqlpp::exception :" << e.what());
		} catch (...) {
			MCE_WARN("[TipsHandler::handle] read " << (*itField) << " from db exception");
		}
	}
	TipsPtr tips = TipsUtil::instance().map2Tips(mapTps);
	MCE_DEBUG("[TipsHandler::handle] load tips : " << tips->tipsid << " / " 
			<< tips->title << "/" << tips->text);
	result_.insert(pair<int, TipsPtr>(tips->tipsid, tips) );
	return true;
}

bool UserActionResultHandler::handle(mysqlpp::Row& row) const   {

	int tipsid = (int) row[FIELD_ACTION_BULLTINID.c_str()];
	useraction_->add(tipsid, -1);
	return true;
}

bool TipsInterestResultHandler::handle(mysqlpp::Row& row) const {
	int userId = (int) row[FIELD_TIPS_INTEREST.c_str()];
	_userIds.push_back(userId);
	return true;
}
