
#include "UserOnlineServiceI.h"
#include "OnlineEvictor.h"
#include "EvictorTemplate.h"
#include "UserOnline.h"
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include "OnlineEvictor.h"
#include "IceLogger.h"

using namespace MyUtil;
using namespace mop::hi::svc::online;
//using namespace mop::hi::svc::adapter;

OnlineEvictor::OnlineEvictor(int expStep)
	: _expStep(expStep)
{
}


Ice::Int
OnlineEvictor::size(::Ice::Int onlineType)
{
	MCE_DEBUG("OnlineEvictor::size working...");
	// onlineType is not used;
	if (onlineType == OT_ALL) {
		return EvictorTemplate<int, OnlineUserPtr>::size();
	} else {
		int result = 0;
		IceUtil::Mutex::Lock lock(*this);
		for(EvictorQueue::iterator it = _timers.begin();
				it != _timers.end();
				++it)
		{
			OnlineUserPtr wsit = (*it)->second->obj;
			if (!(wsit->onlineStat & onlineType)) {
				continue;
			}
			++result;
		}
		return result;
	}
	MCE_DEBUG("OnlineEvictor::size done");
}

void
OnlineEvictor::setStat(::Ice::Int userId, ::Ice::Int onlineType, bool online)
{
	MCE_DEBUG("OnlineEvictor::setStat userId=" << userId 
			<< " onlineType=" << onlineType 
			<< " online=" << online);
	OnlineUserPtr session;
	bool found = false;
	try {
		session = locate(userId);
		found = true;
	} catch (const Ice::Exception& e) {
		MCE_DEBUG("Create new session object for userId=" << userId);		
	}	
	if (online) {
		if (!session) {
			session = new OnlineUser;
			session->userId = userId;
			session->onlineStat = OFF_LINE;
			add(session, userId);			
		}
		session->timestamp = time(NULL);
		session->onlineStat |= onlineType;
	} else {
		if (session) {
			session->onlineStat &= ~onlineType;
			if (session->onlineStat == OFF_LINE) {
				remove(session->userId);
			}
		}		
	}
	if (!found || (session && session->onlineStat != onlineType)) {
		IceUtil::Mutex::Lock lock(_onlineChangeMutex);
		_onlineChange[userId] = session;
	}
}

::Ice::Int
OnlineEvictor::getStat(::Ice::Int userId)
{
	return has(userId) ? 1 : 2;
}


MyUtil::Int2IntMap
OnlineEvictor::checkStats(const MyUtil::IntSeq& userIds)
{
	MCE_DEBUG("OnlineEvictor::checkStats working...");
	MyUtil::Int2IntMap result;
	for(MyUtil::IntSeq::const_iterator it = userIds.begin();
			it != userIds.end();
			++it)
	{
		if( has( *it ) )
		{
			MCE_DEBUG("OnlineEvictor::getStats " << *it << " online");
			result.insert(make_pair(*it, 1));
		}
		else
		{
			MCE_DEBUG("OnlineEvictor::getStats " << *it << " offline");
			result.insert(make_pair(*it, 2));
		}
	}
	MCE_DEBUG("OnlineEvictor::checkStats done");
    return result;
}

IntSeq
OnlineEvictor::getStats(::Ice::Int begin,
					     ::Ice::Int limit,
						 ::Ice::Int onlineType)
{
	IceUtil::Mutex::Lock lock(*this);
	IntSeq result;
	int pos = 0;
	begin = (begin < 0) ? 0 : begin;
	limit = (limit < 0) ? _map.size() : limit;
	MCE_DEBUG( "OnlineEvictor::get " << begin << "\t"<< limit);
	for(EvictorQueue::iterator it = _timers.begin();
			it != _timers.end();
			++it)
	{
		OnlineUserPtr wsit = (*it)->second->obj;
		if (!(wsit->onlineStat & onlineType)) {
			continue;
		}
		if ( pos++ < begin)
		{
			MCE_DEBUG( "begin <= pos++" );
			continue;
		}
		
		if ( result.size() < (size_t)limit )
		{
			MCE_DEBUG( "OnlineEvictor get: push_back one. " );
			result.push_back( wsit->userId );
			continue;
		}
		break;
	}
	MCE_DEBUG("OnlineEvictor::get return. result.size() = " << result.size());
	return result;
}


void
OnlineEvictor::timedEvictEntries()
{
	MCE_DEBUG("OnlineEvictor::evictEntries begin.");
	IntSeq userIds;
	int latestValidTimestamp = time(NULL) - _expStep;
	MCE_DEBUG("OnlineEvictor::timedEvictEntries -> now: " << time(NULL)
		<< ", lastestValidTimestamp: " << latestValidTimestamp);
	{
		IceUtil::Mutex::Lock lock(*this);		
		for(EvictorQueue::reverse_iterator it = _timers.rbegin();
				it != _timers.rend(); ++it)
		{
			OnlineUserPtr obj = (*it)->second->obj;
			if ( obj->timestamp > latestValidTimestamp ) {
				break;
			}
			obj->onlineStat &= ~OT_WEB;
			if (obj->onlineStat == OFF_LINE) {
				userIds.push_back((*it)->first);
			}
		}
	}
	MCE_DEBUG("OnlineEvictor::timedEvictEntries -> evict total: " << userIds.size());
	for (size_t i = 0; i < userIds.size(); ++i) {
 		MCE_DEBUG("OnlineEvictor::timedEvictEntries -> evict: " << userIds.at(i));
		remove(userIds.at(i));		
		IceUtil::Mutex::Lock lock(_onlineChangeMutex);
		_onlineChange[userIds.at(i)] = 0;
 	}
}

void
OnlineEvictor::swapOnlineChange(OnlineUserMap& tmp)
{
	IceUtil::Mutex::Lock lock(_onlineChangeMutex);
	_onlineChange.swap(tmp);
};
