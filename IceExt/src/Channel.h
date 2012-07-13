// **********************************************************************
//
// Copyright (c) 2005-2006 DuDu, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef __ICE_CHANNEL_H__
#define __ICE_CHANNEL_H__

#include <Ice/Ice.h>
#include "UtilCxx/src/TaskManager.h"
#include <IceGrid/Query.h>
#include "util/cpp/IpAddr.hpp"


using namespace std;

namespace MyUtil {

enum ChannelType {
	OCE_CHANNEL,
	TALK_CHANNEL
};

enum InvokeType {
	ONE_WAY,
	TWO_WAY,
	BATCH_ONE_WAY,
	DATAGRAM
};

class Channel : public IceUtil::Shared {
public:
	Channel(const Ice::PropertiesPtr& props) {
		Ice::InitializationData id;
		id.properties = props;
		_communicator = Ice::initialize(id);
	}
	virtual ~Channel() {
		_communicator->destroy();;
	}

	template<class Prx> Prx uncheckedCast(const Ice::ObjectPrx& prx) {
		return Prx::uncheckedCast(prx);
	}
	template<class Prx> Prx checkedCast(const Ice::ObjectPrx& prx) {
		return Prx::checkedCast(prx);
	}

	Ice::ObjectPrx locate(const string& id, const string& endpoints,
			InvokeType it, int timeout) {
		Ice::ObjectPrx prx = _communicator->stringToProxy(id + endpoints)->ice_timeout(timeout);
		switch (it) {
		case TWO_WAY:
			return prx;
		case ONE_WAY:
			return prx->ice_oneway();
		case BATCH_ONE_WAY:
			return prx->ice_batchOneway();
		case DATAGRAM:
			return prx->ice_datagram();
		default:
			return prx;
		}
	}

	template<class Prx> Prx locate(const string& id,
			const string& endpoints = "", InvokeType it = TWO_WAY, int timeout = 260) {
		return uncheckedCast<Prx>(locate(id, endpoints, it, timeout));
	}
	template<class Prx> Prx locate(const Ice::Identity& id,
			const string& endpoints = "", InvokeType it = TWO_WAY, int timeout = 260) {
		return locate<Prx>(id.category + "/" + id.name, endpoints, it, timeout);
	}

	vector<Ice::ObjectPrx> findAllReplicas(const Ice::ObjectPrx& proxy){
		return _query->findAllReplicas(proxy);
	}


	Ice::CommunicatorPtr& getCommunicator() {
		return _communicator;
	}



protected:
	Channel() {
		;
	}
	Channel(const Channel& rhs);
	Channel& operator=(const Channel& rhs);

	Ice::CommunicatorPtr _communicator;
	IceGrid::QueryPrx _query;
	IceUtil::Mutex _mutex;
};
typedef IceUtil::Handle<Channel> ChannelPtr;


//---------------------------------------------------------------------------
class OceChannel : public Channel {
public:
	OceChannel();
	virtual ~OceChannel();
};

//---------------------------------------------------------------------------
class UserChannel : public Channel {
public:
	UserChannel();
	virtual ~UserChannel();
};

//---------------------------------------------------------------------------
class BuddyChannel : public Channel {
public:
        BuddyChannel();
        virtual ~BuddyChannel();
};

//---------------------------------------------------------------------------
class TalkChannel : public Channel {
public:
	TalkChannel();
	virtual ~TalkChannel();
};
//---------------------------------------------------------------------------
class ImUgcChannel : public Channel {
public:
	ImUgcChannel();
	virtual ~ImUgcChannel();
};
//---------------------------------------------------------------------------
class FollowChannel : public Channel {
public:
	FollowChannel();
	virtual ~FollowChannel();
};
//---------------------------------------------------------------------------
class FeedChannel : public Channel {
public:
	FeedChannel();
	virtual ~FeedChannel();
};

//---------------------------------------------------------------------------
class SearchChannel : public Channel {
public:
	SearchChannel();
	virtual ~SearchChannel();
};
//---------------------------------------------------------------------------
class AddressbookChannel : public Channel {
public:
	AddressbookChannel();
	virtual ~AddressbookChannel();
};


//---------------------------------------------------------------------------
class SearchCacheChannel : public Channel {
public:
	SearchCacheChannel();
	virtual ~SearchCacheChannel();
};

//---------------------------------------------------------------------------
class SearchFoFChannel : public Channel {
public:
	SearchFoFChannel();
	virtual ~SearchFoFChannel();
};

//---------------------------------------------------------------------------
class ShareSearchChannel : public Channel {
public:
	ShareSearchChannel();
	virtual ~ShareSearchChannel();
};
//---------------------------------------------------------------------------
class BlogSearchChannel : public Channel {
public:
	BlogSearchChannel();
	virtual ~BlogSearchChannel();
};



//---------------------------------------------------------------------------
class CoreChannel : public Channel {
public:
	CoreChannel();
	virtual ~CoreChannel();
};

//---------------------------------------------------------------------------
class LogChannel : public Channel {
public:
	LogChannel();
	virtual ~LogChannel();
};

//---------------------------------------------------------------------------
class ActiveTrackChannel : public Channel {
public:
	ActiveTrackChannel();
	virtual ~ActiveTrackChannel();
};

//---------------------------------------------------------------------------
class WServiceChannel : public Channel {
public:
	WServiceChannel();
	virtual ~WServiceChannel();
};

//---------------------------------------------------------------------------
class WServiceUGCChannel : public Channel {
public:
	WServiceUGCChannel();
	virtual ~WServiceUGCChannel();
};

//---------------------------------------------------------------------------
class WServiceInviteChannel : public Channel {
public:
	WServiceInviteChannel();
	virtual ~WServiceInviteChannel();
};

//---------------------------------------------------------------------------
class SocialGraphChannel : public Channel {
public:
	SocialGraphChannel();
	virtual ~SocialGraphChannel();
};

//---------------------------------------------------------------------------
class LikeChannel : public Channel {
public:
	LikeChannel();
	virtual ~LikeChannel();
};
//---------------------------------------------------------------------------
class FootprintChannel : public Channel {
public:
	FootprintChannel();
	virtual ~FootprintChannel();
};
//---------------------------------------------------------------------------
class UserMonitorChannel : public Channel {
public:
	UserMonitorChannel();
	virtual ~UserMonitorChannel();
};

//---------------------------------------------------------------------------
class PassportChannel : public Channel {
public:
	PassportChannel();
	virtual ~PassportChannel();
};
//---------------------------------------------------------------------------
class MenuChannel : public Channel {
public:
	MenuChannel();
	virtual ~MenuChannel();
};
//---------------------------------------------------------------------------
class ImporterChannel : public Channel {
public:
	ImporterChannel();
	virtual ~ImporterChannel();
};

//---------------------------------------------------------------------------
class GadgetsChannel : public Channel {
public:
	GadgetsChannel();
	virtual ~GadgetsChannel();
};

//---------------------------------------------------------------------------
class QuestChannel : public Channel {
public:
	QuestChannel();
	virtual ~QuestChannel();
};

//---------------------------------------------------------------------------
//新鲜事重构后使用以下两个Channel

class XceFeedChannel : public Channel {
public:
	XceFeedChannel();
	virtual ~XceFeedChannel();
};

class XceFeedDBChannel : public Channel {
public:
	XceFeedDBChannel();
	virtual ~XceFeedDBChannel();
};

// 新鲜事controller 集群 channel
class XceFeedControllerChannel : public Channel {
public:
	XceFeedControllerChannel();
	virtual ~XceFeedControllerChannel();
};

class ZhanFeedChannel : public Channel {
public:
	ZhanFeedChannel();
	virtual ~ZhanFeedChannel();
};

class ZhanFeedDBChannel : public Channel {
public:
	ZhanFeedDBChannel();
	virtual ~ZhanFeedDBChannel();
};

class McProxyChannel : public Channel {
public:
  McProxyChannel();
    virtual ~McProxyChannel();
};

//新版回复提示及通知服务
class NotifyChannel : public Channel {
public:
	NotifyChannel();
	virtual ~NotifyChannel();
};

class AdChannel : public Channel {
public:
  AdChannel();
  virtual ~AdChannel();
};

/*
class XceFeedSyncChannel : public Channel {
public:
	XceFeedSyncChannel();
	virtual ~XceFeedSyncChannel();
};
*/

class MsgFilterChannel: public Channel {
public:
	MsgFilterChannel();
	virtual ~MsgFilterChannel();
};

//---------------------------------------------------------------------------
class FlushBatchRequestsTimer : public Timer
{
	ChannelPtr _channel;
	int _delay;
public:
	FlushBatchRequestsTimer(const ChannelPtr& channel,int delay=100):Timer(delay),_channel(channel),_delay(delay){};
	virtual void handle();
};
//---------------------------------------------------------------------------
class XceCacheChannel : public Channel {
public:
	XceCacheChannel();
	virtual ~XceCacheChannel();
};


//---------------------------------------------------------------------------
class ScoreChannel : public Channel {
public:
	ScoreChannel();
	virtual ~ScoreChannel();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class BackupChannel : public Channel {
public:
	BackupChannel();
	virtual ~BackupChannel();
};
//---------------------------------------------------------------------------

}
;
#endif

