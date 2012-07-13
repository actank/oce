#include "CommonFriendFinder.h"
#include <limits>
#include "QueryRunner.h"
#include "FriendUtils.h"

using namespace ::com::xiaonei::service;
using namespace std;
using namespace MyUtil;
using namespace ::com::xiaonei::xce;

void CommonFriendCalculator::buildWeight(const MyUtil::Int2IntSeqMap& users, int weight) {
	MCE_DEBUG("build " << _userId << " size=" << users.size());
	try
	{
		for(MyUtil::Int2IntSeqMap::const_iterator i=users.begin();
				i!=users.end();
				++i)
		{
			for(MyUtil::IntSeq::const_iterator ui=i->second.begin();ui!=i->second.end();++ui)
			{
				_weightCount[*ui] += weight;
				MyUtil::Int2IntSeqMap::iterator sIt = _shareCount.find(*ui);
				if(sIt == _shareCount.end())
				{
					MyUtil::IntSeq firstShares;
					firstShares.push_back( i->first );
					_shareCount.insert(make_pair(*ui, firstShares));
				}
				else
				{
					sIt->second.push_back( i->first );
				}
			}
		}
	}
	catch(Ice::Exception& e)
	{
		MCE_WARN("buildUser IceException " << _userId << e.what());
	}
	catch(std::exception& e)
	{
		MCE_WARN("buildUser stdException " << _userId << e.what());
	}
	catch(...)
	{
		MCE_WARN("buildUser exception " << _userId);
	}
}

void CommonFriendCalculator::fetchData() {
	//BuddyCoreSnapshotHelper snapshot = BuddyCoreSnapshotHelper::getForCommonFriend(_userId);
	// initializing
	_friendList=RelationBatchHelper::getFriendSeq(_userId);
	//_friendList = snapshot.getFriendSeq(_userId);
	MCE_INFO("FriendList "<< _userId<<" size "<<_friendList.size());
	_applyList=RelationBatchHelper::getApplySeq(_userId);
	//_applyList = snapshot.getApplySeq(_userId);
	MCE_INFO("ApplyList "<<_userId<<" size "<<_applyList.size());
	if (_friendList.empty() && _applyList.empty()){
		MCE_INFO("CommonFriendCalculator::fetchData, _friendList and _applyList double empty, return userId=" << _userId);
		return;
	}

	_blockList=RelationBatchHelper::getBlockSeq(_userId);
	//_blockList = snapshot.getBlockSeq(_userId);
	MCE_INFO("BlockList "<<_userId<<" size "<<_blockList.size());

	_blockedList = RelationBatchHelper::getBlockedSeq(_userId);	
	MCE_INFO("BlockedList " << _userId << " szie " << _blockedList.size());

	MyUtil::IntSeq commonBlock = CommonFriendsBlockHelper::getCommonFriendsBlock(_userId);
	MCE_INFO("CommonFriendsBlockList "<<_userId<<" size "<<commonBlock.size());

	// remove myself
	_weightCount[_userId]=numeric_limits<int>::min();

	// build weight
	MyUtil::Int2IntSeqMap ff(RelationBatchHelper::getFriendSeqBatch(_friendList));
	//MyUtil::Int2IntSeqMap ff(snapshot.getFriendSeqBatch(_friendList));
	buildWeight(ff, 100);

	// 暂时不要这个类型了。因为批量的查申请，压力太大。
	//	MyUtil::Int2IntSeqMap fa(RelationBatchHelper::getApplySeqBatch(_friendList));
	////MyUtil::Int2IntSeqMap fa(snapshot.getApplySeqBatch(_friendList));
	//buildWeight(fa, 70);

	MyUtil::Int2IntSeqMap af(RelationBatchHelper::getFriendSeqBatch(_applyList));
	//MyUtil::Int2IntSeqMap af(snapshot.getFriendSeqBatch(_applyList));
	buildWeight(af, 70);

	// remove blocks
	for (MyUtil::IntSeq::iterator i=_blockList.begin(); i!=_blockList.end(); ++i) {
		_weightCount[*i]=numeric_limits<int>::min();
	}
	for (MyUtil::IntSeq::iterator i=commonBlock.begin(); i!=commonBlock.end(); ++i) {
		_weightCount[*i]=numeric_limits<int>::min();
	}
	for (MyUtil::IntSeq::iterator i=_blockedList.begin(); i!=_blockedList.end(); ++i) {
		_weightCount[*i]=numeric_limits<int>::min();
	}

	//将已经是该用户的好友对应的权值设置为最小
	for(MyUtil::IntSeq::iterator i=_friendList.begin(); i!=_friendList.end(); ++i){
		_weightCount[(*i)] = numeric_limits<int>::min();
	}
	//将该用户已经发过申请的人权值设置为最小
	for(MyUtil::IntSeq::iterator i=_applyList.begin(); i!=_applyList.end(); ++i){
		_weightCount[(*i)] = numeric_limits<int>::min();
	}
	MCE_INFO("buildUser " << _userId << " weightCount;" << _weightCount.size());
}

std::vector<CommonFriend> CommonFriendCalculator::getComm() {
	fetchData();
	try
	{
		//std::vector<CommonFriend> ret;
		CommonFriend cf;
		for(CommonFriendCalculator::weightIter i=_weightCount.begin();i!=_weightCount.end();++i)
		{
			if(i->second<0)
			{
				continue;
			}
			cf.userId=i->first;
			cf.weight=i->second;
			MyUtil::Int2IntSeqMap::iterator shareIt = _shareCount.find(i->first);
			if(shareIt != _shareCount.end())
			{
				//    MCE_DEBUG("getComm(): sharesList.size = " << shareIt->second.size());
				MyUtil::IntSeq(shareIt->second).swap(cf.shares);
			}
			else
			{
				//  MCE_DEBUG("getComm(): Empty sharesList");
				cf.shares = MyUtil::IntSeq();
			}
			_ret.push_back(cf);
		}
	}
	catch(Ice::Exception& e)
	{
		MCE_WARN("getComm IceException" << e);
	}
	catch(std::exception& e)
	{
		MCE_WARN("getComm StdException" << e.what());
	}
	catch(...)
	{
		MCE_WARN("getComm unknown exception");
	}
	MCE_INFO("getComm userId="<<_userId<<"  size="<<_ret.size());
	if (_ret.empty())
		return _ret;
	sort(_ret.begin(), _ret.end(), CommonFriendWeight);
	return _ret;
}

