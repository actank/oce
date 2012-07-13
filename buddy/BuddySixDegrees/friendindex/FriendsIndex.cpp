/*
 * =====================================================================================
 *
 *       Filename:  FriendsIndex.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年08月18日 14时45分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FriendsIndex.h"
#include "buddy/BuddySixDegrees/src/StartStopFlag.h"
#include <iostream>

using namespace std;
using namespace MyUtil;
using namespace xce::buddy;

//=============================应用主类================================
void FriendsIndex::asynBuildIndex(Ice::Int id) {
	TaskManager::instance().execute(new asynBuildIndexTask(id, this));
}

void FriendsIndex::asynBuildIndexTask::handle() {
	
	asynPushIndexThreadPtr asynPushThread = new asynPushIndexThread(_id, _builder);
	asynBuildIndexThreadPtr asynBuildThread = new asynBuildIndexThread(_id, _builder, asynPushThread);

	asynPushThread->start();
	asynBuildThread->start();

	asynBuildThread->getThreadControl().join();
	asynPushThread->getThreadControl().join();

}

//=============================asynPushIndexTask================================
void FriendsIndex::asynPushIndexTask::handle() {
	ostringstream os;
	os << "FriendsIndex::asynPushIndexTask, id=" << _id;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);

	AsynIndexDataPtr result = _builder->locateAsynIndex(_id);
	if(result) {
		result->push(_data);
	} else {
		MCE_WARN("FriendsIndex::asynPushIndexTask, Error AsynIndexDataPtr is NULL, id=" << _id);
	}
	return;
}

//=============================asynPushIndexThread================================
void FriendsIndex::asynPushIndexThread::setFinish() {
	AsynIndexDataPtr result = _builder->locateAsynIndex(_id);
	if(result) {
		MCE_INFO("FriendsIndex::asynBuildIndexThread::setFinish, id=" << _id << " freshers.size()=" << result->getFreshersSize());
		result->setFinish(true);
	} else {
		MCE_WARN("FriendsIndex::asynBuildIndexThread::setFinish, Error: AsynIndexDataPtr is NULL, id=" << _id);
	}
}

void FriendsIndex::asynPushIndexThread::pushIndexInfo(IndexInfoIPtr indexInfo) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_indexCache.push_back(indexInfo);
	if (_indexCache.size() > INDEX_CACHE_SIZE) {
		_mutex.notify();
	}
}

void FriendsIndex::asynPushIndexThread::pushIntoData(IndexInfoSeq& seq) {
	ostringstream os;
	os << "FriendsIndex.pushIntoData, id=" << _id;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);

	AsynIndexDataPtr result = _builder->locateAsynIndex(_id);
	if(result) {
		result->push(seq);
	} else {
		MCE_WARN("FriendsIndex::asynPushIndexThread::pushIntoData, Error AsynIndexDataPtr is NULL, id=" << _id);
	}
	return;
}

void FriendsIndex::asynPushIndexThread::run() {
	MCE_INFO("FriendsIndex::asynPushIndexThread::run(), start!");
	while (true) {
		IndexDataPtr pushData = new IndexData(_id);
		{
			if (StartStopFlag::instance().isBuildIndexStop(_id)== true) {
				MCE_INFO("StartStopFlag::instance().isBuildIndexStop, true");
				break;
			}

        		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);			      
			if (_indexCache.size() <= INDEX_CACHE_SIZE) {						
				_mutex.timedWait(IceUtil::Time::seconds(WAIT_TIME));
			}

			if (!_indexCache.empty()) {
				pushData->indexs.reserve(_indexCache.size());
				pushData->indexs.swap(_indexCache);
      			} else {
				MCE_INFO("endsIndex::asynPushIndexThread::run(), setFinish, id = " << _id);
				setFinish();
				break;														    
			}
		}
		TaskManager::instance().execute(new asynPushIndexTask(_id, pushData, _builder));
	}
	MCE_INFO("FriendsIndex::asynPushIndexThread::run(), end!");
}

//=============================asynBuildIndexThread================================
void FriendsIndex::asynBuildIndexThread::run() {
	ostringstream os;
	os << "FriendsIndex.asynBuildIndexThread.buildIndex, id=" << _id;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);

	bool isBreak = false;
	//1 degree, have done in twoFriends compare, so .
	//asynBuildIndexInfo(_id, 1);

	//2 degree , 我的好友是节点用户，则我和节点用户的好友为2度好友
	BuddyDataPtr oneInfo = _builder->locateFriend(_id);
	if(!oneInfo) {
		MCE_WARN("FriendsIndex, one degree friends is NULL");
		return;
	}

	MyUtil::LongSeq oneIds;
	for(std::vector<int>::const_iterator it = oneInfo->friends.begin();
			it != oneInfo->friends.end() && !isBreak; ++it) {
		int result = asynBuildIndexInfo((*it), 2);
		switch (result) {
			case BUILD_SUCESS:
				oneIds.push_back(*it);
				break;
			case BUILD_OVERFLOW:
				MCE_INFO("FriendsIndex::asynBuildIndexThread::run, indexCache is OVERFLOW, degree 2, id =" << _id);
				isBreak = true;
				break;
			default:
				break;
		}
	}

	//3 degree
	MyUtil::ObjectResultPtr twoInfo = _builder->locateFriends(oneIds);
	if (!twoInfo) {
		MCE_WARN("FriendsIndex, two degree friends is NULL");
		return;
	}

	for (ObjectMap::iterator it = twoInfo->data.begin(); it != twoInfo->data.end() && !isBreak; ++it) {

		BuddyDataPtr tmpInfo = BuddyDataPtr::dynamicCast((*it).second);
		if(!tmpInfo) {
			MCE_WARN("FriendsIndex, two degree friends is NULL, id=" << (*it).first);
			continue;
		}
		
		for(std::vector<int>::const_iterator itB = tmpInfo->friends.begin();
				itB != tmpInfo->friends.end() && !isBreak; ++itB) {
			if(BUILD_OVERFLOW == asynBuildIndexInfo((*itB), 3, (*it).first)) {
				MCE_INFO("FriendsIndex::asynBuildIndexThread::run, indexCache is OVERFLOW, degree 3, id = " << _id);
				isBreak = true;
				break;
			}

		}
	}

	return;
}

/**
 * 建立普通用户对节点用户的2/3级倒排索引
 * 这里先找节点用户，再找节点用户的朋友是个重大突破；而不是先找三度好友再找节点用户。
 * @param int id 用户ID
 * @param int degree 好友关系度
 * @param int preId 前面ID，构成路径
 */
int FriendsIndex::asynBuildIndexThread::asynBuildIndexInfo(int id, int degree, int preId) {
	BuddyDataPtr joinInfo = _builder->locateJoinFriend(id);

	if (joinInfo) {
		for(std::vector<int>::const_iterator it = joinInfo->friends.begin();it != joinInfo->friends.end(); ++it) {

			IndexInfoIPtr indexInfo = new IndexInfoI();
			indexInfo->id = (*it);
			indexInfo->degree = degree;
			switch (degree) {
				case 2:
					indexInfo->friends.push_back(id);
					break;
				case 3:
					indexInfo->friends.push_back(preId);
					indexInfo->friends.push_back(id);
					break;
				default:
					break;
			}
			_pushThread->pushIndexInfo(indexInfo);
		}
		_indexSum ++;
		return _indexSum > MAX_INDEX ? BUILD_OVERFLOW : BUILD_SUCESS;
	} else {
		return BUILD_SUCESS;
	}
}
