// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include "Channel.h"
#include "IceLogger.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace MyUtil;

//---------------------------------------------------------------------------
OceChannel::OceChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNei");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty(
			"Ice.Default.Locator",
			"XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNei/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

OceChannel::~OceChannel() {

}
//---------------------------------------------------------------------------
UserChannel::UserChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "User");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","User/Locator:default -h UserMaster -p 4075:default -h UserReplica1 -p 4075:default -h UserReplica2 -p 4075");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("User/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

UserChannel::~UserChannel() {
}
//---------------------------------------------------------------------------

BuddyChannel::BuddyChannel() {
        Ice::PropertiesPtr properties = Ice::createProperties();
        properties->setProperty("Ice.Override.ConnectTimeout", "300");
        properties->setProperty("IceGrid.InstanceName", "Buddy");
        properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
        properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

        properties->setProperty("Ice.MessageSizeMax", "20480");

        properties->setProperty("Ice.Default.Host", IpAddr::inner());
        properties->setProperty("Ice.Default.Locator","Buddy/Locator:tcp -h BuddyMaster -p 4077:tcp -h BuddyReplica1 -p 4077:tcp -h BuddyReplica2 -p 4077");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);
        _query  = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Buddy/Query"));
        TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

BuddyChannel::~BuddyChannel() {
}
//---------------------------------------------------------------------------
CoreChannel::CoreChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "CoreCache");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","CoreCache/Locator:default -h CoreCacheMaster -p 4068:default -h CoreCacheReplica1 -p 4068:default -h CoreCacheReplica2 -p 4068");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("CoreCache/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

CoreChannel::~CoreChannel() {
}
//---------------------------------------------------------------------------
PassportChannel::PassportChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Passport");
	properties->setProperty("Ice.ThreadPool.Client.Size","100");
	properties->setProperty("Ice.ThreadPool.Client.SizeMax","3000");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");
	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Passport/Locator:default -h PassportMaster -p 4076:default -h PassportReplica1 -p 4076:default -h PassportReplica2 -p 4076");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query  = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Passport/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

PassportChannel::~PassportChannel() {
}

//---------------------------------------------------------------------------
MenuChannel::MenuChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Menu");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Menu/Locator:tcp -h MenuMaster -p 4078:tcp -h MenuReplica1 -p 4078:tcp -h MenuReplica2 -p 4078");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Menu/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

MenuChannel::~MenuChannel() {
}
//---------------------------------------------------------------------------
SocialGraphChannel::SocialGraphChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "SocialGraph");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","SocialGraph/Locator:default -h SocialGraphMaster -p 4072:default -h SocialGraphReplica1 -p 4072:default -h SocialGraphReplica2 -p 4072");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("SocialGraph/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

SocialGraphChannel::~SocialGraphChannel() {
}
//---------------------------------------------------------------------------
LikeChannel::LikeChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Like");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Like/Locator:tcp -h LikeMaster -p 4071:tcp -h LikeReplica1 -p 4071:tcp -h LikeReplica2 -p 4071");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Like/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

LikeChannel::~LikeChannel() {
}
//---------------------------------------------------------------------------
FootprintChannel::FootprintChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Footprint");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Footprint/Locator:tcp -h FootprintMaster -p 4085:tcp -h FootprintReplica1 -p 4085:tcp -h FootprintReplica2 -p 4085");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Footprint/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

FootprintChannel::~FootprintChannel() {
}


//---------------------------------------------------------------------------
GadgetsChannel::GadgetsChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Gadgets");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Gadgets/Locator:tcp -h GadgetsMaster -p 4079:tcp -h GadgetsReplica1 -p 4079:tcp -h GadgetsReplica2 -p 4079");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Gadgets/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

GadgetsChannel::~GadgetsChannel() {
}

//---------------------------------------------------------------------------
QuestChannel::QuestChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Quest");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Quest/Locator:tcp -h QuestMaster -p 4084:tcp -h QuestReplica1 -p 4084:tcp -h QuestReplica2 -p 4084");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Quest/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

QuestChannel::~QuestChannel() {
}


//---------------------------------------------------------------------------
ImporterChannel::ImporterChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Importer");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator","Importer/Locator:tcp -h ImporterMaster -p 4080:tcp -h ImporterReplica1 -p 4080:tcp -h ImporterReplica2 -p 4080");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query	= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Importer/Query"));
	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

ImporterChannel::~ImporterChannel() {
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
TalkChannel::TalkChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNei");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	//	properties->setProperty(
	//			"Ice.Default.Locator",
	//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator",
			"XNTalk/Locator:default -h XNTalkRegistry -p 12000");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XNTalk/Query"));

	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

TalkChannel::~TalkChannel() {

}

//---------------------------------------------------------------------------
ImUgcChannel::ImUgcChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNei");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());

	properties->setProperty("Ice.Default.Locator", "ImUgc/Locator:default -h ImUgcRegistry -p 15000");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("ImUgc/Query"));

	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

ImUgcChannel::~ImUgcChannel() {
}


//---------------------------------------------------------------------------
FollowChannel::FollowChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Follow");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	//	properties->setProperty(
	//			"Ice.Default.Locator",
	//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator",
			"Follow/Locator:default -h FollowRegistry -p 15300:default -h FollowRegistryReplica1 -p 15301:default -h FollowRegistryReplica2 -p 15302");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Follow/Query"));
}

FollowChannel::~FollowChannel() {

}
//---------------------------------------------------------------------------
FeedChannel::FeedChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNei");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	//	properties->setProperty(
	//			"Ice.Default.Locator",
	//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty(
			"Ice.Default.Locator",
			"XiaoNei/Locator:default -h XceFeedMaster -p 12000:default -h XceFeedReplica1 -p 12001:default -h XceFeedReplica2 -p 12002");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNei/Query"));

	TaskManager::instance().schedule(new FlushBatchRequestsTimer(this));
}

FeedChannel::~FeedChannel() {

}


//---------------------------------------------------------------------------

XceFeedChannel::XceFeedChannel(){
		Ice::PropertiesPtr properties = Ice::createProperties();
		//properties->setProperty("Ice.Override.Timeout", "300");
		properties->setProperty("Ice.Override.ConnectTimeout", "300");
		properties->setProperty("IceGrid.InstanceName", "XceFeed");

		properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
		properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

		properties->setProperty("Ice.Default.Host", IpAddr::inner());
		//			properties->setProperty("Ice.Trace.Protocol", "3");
		//			properties->setProperty("Ice.Trace.Network", "3");

		//			properties->setProperty("Ice.ProgramName", "ice");
		//			properties->setProperty("Ice.UseSyslog", "1");

		//	properties->setProperty(
		//			"Ice.Default.Locator",
		//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

		properties->setProperty(
				"Ice.Default.Locator",
				"XceFeed/Locator:default -h XceFeedRegistry -p 14300:default -h XceFeedRegistryReplica1 -p 14301:default -h XceFeedRegistryReplica2 -p 14302");
		Ice::InitializationData id;
		id.properties = properties;
		_communicator = Ice::initialize(id);

		_query
				= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XceFeed/Query"));
}

XceFeedChannel::~XceFeedChannel(){

}

// 新鲜事controller 集群
XceFeedControllerChannel::XceFeedControllerChannel(){
		Ice::PropertiesPtr properties = Ice::createProperties();
		//properties->setProperty("Ice.Override.Timeout", "300");
		properties->setProperty("Ice.Override.ConnectTimeout", "300");
		properties->setProperty("IceGrid.InstanceName", "XceFeedController");

		properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
		properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

		properties->setProperty("Ice.Default.Host", IpAddr::inner());
		//			properties->setProperty("Ice.Trace.Protocol", "3");
		//			properties->setProperty("Ice.Trace.Network", "3");

		//			properties->setProperty("Ice.ProgramName", "ice");
		//			properties->setProperty("Ice.UseSyslog", "1");

		//	properties->setProperty(
		//			"Ice.Default.Locator",
		//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

		properties->setProperty(
				"Ice.Default.Locator",
				"XceFeedController/Locator:default -h XceFeedControllerRegistry -p 15200:default -h XceFeedControllerRegistryReplica1 -p 15201:default -h XceFeedControllerRegistryReplica2 -p 15202");
		Ice::InitializationData id;
		id.properties = properties;
		_communicator = Ice::initialize(id);

		_query
				= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XceFeedController/Query"));
}

XceFeedControllerChannel::~XceFeedControllerChannel(){

}

XceFeedDBChannel::XceFeedDBChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XceFeedDB");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	//	properties->setProperty(
	//			"Ice.Default.Locator",
	//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator",
			"XceFeedDB/Locator:default -h XceFeedDBRegistry -p 14400:default -h XceFeedDBRegistryReplica1 -p 14401:default -h XceFeedDBRegistryReplica2 -p 14402");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XceFeedDB/Query"));

}

XceFeedDBChannel::~XceFeedDBChannel() {

}

ZhanFeedChannel::ZhanFeedChannel(){
		Ice::PropertiesPtr properties = Ice::createProperties();
		//properties->setProperty("Ice.Override.Timeout", "300");
		properties->setProperty("Ice.Override.ConnectTimeout", "300");
		properties->setProperty("IceGrid.InstanceName", "ZhanFeed");

		properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
		properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

		properties->setProperty("Ice.Default.Host", IpAddr::inner());
		//			properties->setProperty("Ice.Trace.Protocol", "3");
		//			properties->setProperty("Ice.Trace.Network", "3");

		//			properties->setProperty("Ice.ProgramName", "ice");
		//			properties->setProperty("Ice.UseSyslog", "1");

		//	properties->setProperty(
		//			"Ice.Default.Locator",
		//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

		properties->setProperty(
				"Ice.Default.Locator",
				"ZhanFeed/Locator:default -h ZhanFeedRegistry -p 14500:default -h ZhanFeedRegistryReplica1 -p 14501:default -h ZhanFeedRegistryReplica2 -p 14502");
		Ice::InitializationData id;
		id.properties = properties;
		_communicator = Ice::initialize(id);

		_query
				= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("ZhanFeed/Query"));
}

ZhanFeedChannel::~ZhanFeedChannel(){

}

ZhanFeedDBChannel::ZhanFeedDBChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "ZhanFeedDB");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	//	properties->setProperty(
	//			"Ice.Default.Locator",
	//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator",
			"ZhanFeedDB/Locator:default -h ZhanFeedDBRegistry -p 14600:default -h ZhanFeedDBRegistryReplica1 -p 14601:default -h ZhanFeedDBRegistryReplica2 -p 14602");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("ZhanFeedDB/Query"));

}

ZhanFeedDBChannel::~ZhanFeedDBChannel() {

}

//---------------------------------------------------------------------------
//notify

NotifyChannel::NotifyChannel(){
		Ice::PropertiesPtr properties = Ice::createProperties();
		properties->setProperty("Ice.Override.ConnectTimeout", "300");
		properties->setProperty("IceGrid.InstanceName", "Notify");

		properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
		properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

		properties->setProperty("Ice.Default.Host", IpAddr::inner());

		properties->setProperty(
				"Ice.Default.Locator",
				"Notify/Locator:default -h NotifyRegistry -p 14400:default -h NotifyRegistryReplica1 -p 14401:default -h NotifyRegistryReplica2 -p 14402");
		Ice::InitializationData id;
		id.properties = properties;
		_communicator = Ice::initialize(id);

		_query
				= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Notify/Query"));
}

NotifyChannel::~NotifyChannel(){

}


//---------------------------------------------------------------------------
//McProxyChannel

McProxyChannel::McProxyChannel(){
        Ice::PropertiesPtr properties = Ice::createProperties();
        properties->setProperty("Ice.Override.ConnectTimeout", "300");
        properties->setProperty("IceGrid.InstanceName", "McProxy");

        properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
        properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

        properties->setProperty("Ice.Default.Host", IpAddr::inner());

        properties->setProperty(
                "Ice.Default.Locator",
                "McProxy/Locator:default -h McProxyRegistry -p 14700");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query
                = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("McProxy/Query"));
}

McProxyChannel::~McProxyChannel(){

}

//---------------------------------------------------------------------------
//AdChannel

AdChannel::AdChannel(){
        Ice::PropertiesPtr properties = Ice::createProperties();
        properties->setProperty("Ice.Override.ConnectTimeout", "300");
        properties->setProperty("IceGrid.InstanceName", "Ad");

        properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
        properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");
        properties->setProperty("Ice.Default.Host", IpAddr::inner());

        properties->setProperty(
                "Ice.Default.Locator",
                "Ad/Locator:default -h AdRegistry -p 14800:default -h AdRegistryReplica1 -p 14801:default -h AdRegistryReplica2 -p 14802");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query
                = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Ad/Query"));
}

AdChannel::~AdChannel(){

}






MsgFilterChannel::MsgFilterChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiAntiSpam");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator",
			"XiaoNeiAntiSpam/Locator:default -h XiaoNeiAntiSpamRegistry -p 4061:default -h XiaoNeiAntiSpamRegistryReplica1 -p 6001:default -h XiaoNeiAntiSpamRegistryReplica2 -p 6002");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XceFeedDB/Query"));

}

MsgFilterChannel::~MsgFilterChannel() {

}

/*
XceFeedSyncChannel::XceFeedSyncChannel() {
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNei");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	//	properties->setProperty(
	//			"Ice.Default.Locator",
	//			"XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator",
			"XiaoNei/Locator:default -h 123.125.45.125 -p 12000");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNei/Query"));

}

XceFeedSyncChannel::~XceFeedSyncChannel() {

}*/
//---------------------------------------------------------------------------
SearchChannel::SearchChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiSearch");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
        //                      properties->setProperty("Ice.Trace.Protocol", "3");
        //                      properties->setProperty("Ice.Trace.Network", "3");

        //                      properties->setProperty("Ice.ProgramName", "ice");
        //                      properties->setProperty("Ice.UseSyslog", "1");

        //      properties->setProperty(
        //                      "Ice.Default.Locator",
        //                      "XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator", "XiaoNeiSearch/Locator:default -h XiaoNeiSearchRegistry -p 4061:default -h XiaoNeiSearchRegistryReplica1 -p 4061:default -h XiaoNeiSearchRegistryReplica2 -p 4061");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNeiSearch/Query"));
}

SearchChannel::~SearchChannel() {

}
//---------------------------------------------------------------------------
AddressbookChannel::AddressbookChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiAddressbook");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
        //                      properties->setProperty("Ice.Trace.Protocol", "3");
        //                      properties->setProperty("Ice.Trace.Network", "3");

        //                      properties->setProperty("Ice.ProgramName", "ice");
        //                      properties->setProperty("Ice.UseSyslog", "1");

        //      properties->setProperty(
        //                      "Ice.Default.Locator",
        //                      "XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator", "AddressBook/Locator:default -h AddressBookMaster -p 4089:default -h AddressBookReplica1 -p 4089:default -h AddressBookReplica2 -p 4089");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("AddressBook/Query"));
}

AddressbookChannel::~AddressbookChannel() {

}
//---------------------------------------------------------------------------
SearchFoFChannel::SearchFoFChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiSearchFoF");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
        //                      properties->setProperty("Ice.Trace.Protocol", "3");
        //                      properties->setProperty("Ice.Trace.Network", "3");

        //                      properties->setProperty("Ice.ProgramName", "ice");
        //                      properties->setProperty("Ice.UseSyslog", "1");

        //      properties->setProperty(
        //                      "Ice.Default.Locator",
        //                      "XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator", "XiaoNeiSearchFoF/Locator:default -h XiaoNeiSearchFoFMaster.d.xiaonei.com -p 4061:default -h XiaoNeiSearchFoFReplica1.d.xiaonei.com -p 4061:default -h XiaoNeiSearchFoFReplica2.d.xiaonei.com -p 4061");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNeiSearchFoF/Query"));
}

SearchFoFChannel::~SearchFoFChannel() {

}
//---------------------------------------------------------------------------

SearchCacheChannel::SearchCacheChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	//properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiSearchCache");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");
  //TODO:服务器端响应调整最大消息长度
	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator", "XiaoNeiSearchCache/Locator:tcp -h XiaoNeiSearchCacheMaster -p 4061:tcp -h XiaoNeiSearchCacheReplica1 -p 4061:tcp -h XiaoNeiSearchCacheReplica2 -p 4061");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNeiSearchCache/Query"));
}

SearchCacheChannel::~SearchCacheChannel() {

}
///---------------------------------------------------------------------------
ShareSearchChannel::ShareSearchChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiShareSearch");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator", "ShareSearch/Locator:tcp -h ShareSearchMaster -p 4090:tcp -h ShareSearchReplica1 -p 4090:tcp -h ShareSearchReplica2 -p 4090");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("ShareSearch/Query"));
}

ShareSearchChannel::~ShareSearchChannel() {

}
BlogSearchChannel::BlogSearchChannel(){
  Ice::PropertiesPtr properties = Ice::createProperties();
  properties->setProperty("Ice.Override.ConnectTimeout", "300");
  properties->setProperty("IceGrid.InstanceName", "XiaoNeiBlogSearch");

  properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
  properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");
  properties->setProperty("Ice.MessageSizeMax", "204800");

  properties->setProperty("Ice.Default.Host", IpAddr::inner());
  properties->setProperty("Ice.Default.Locator", "BlogSearch/Locator:tcp -h BlogSearchMaster -p 4090:tcp -h BlogSearchReplica1 -p 4090:tcp -h BlogSearchReplica2 -p 4090");
  Ice::InitializationData id;
  id.properties = properties;
  _communicator = Ice::initialize(id);

  _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("BlogSearch/Query"));
}

BlogSearchChannel::~BlogSearchChannel() {

}
//---------------------------------------------------------------------------
LogChannel::LogChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName", "XceLog");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");

	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty(
			"Ice.Default.Locator",
			"XceLog/Locator:default -h LogMaster -p 4062:default -h LogReplica1 -p 4062:default -h LogReplica2 -p 4062");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XceLog/Query"));
}

LogChannel::~LogChannel() {

}

//---------------------------------------------------------------------------
ActiveTrackChannel::ActiveTrackChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName", "ActiveTrack");

	properties->setProperty("Ice.ThreadPool.Client.SizeMax", "1000");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "131072");
	properties->setProperty("Ice.ThreadPool.Server.SizeMax", "1000");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "131072");

	properties->setProperty("Ice.MessageSizeMax", "10240");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");
	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty(
			"Ice.Default.Locator",
			"ActiveTrack/Locator:default -h ActiveTrackMaster -p 4065:default -h ActiveTrackReplica1 -p 4065:default -h ActiveTrackReplica2 -p 4065");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("ActiveTrack/Query"));
}

ActiveTrackChannel::~ActiveTrackChannel() {

}

//---------------------------------------------------------------------------
WServiceChannel::WServiceChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName", "wService");

	properties->setProperty("Ice.ThreadPool.Client.SizeMax", "2000");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "131072");
	properties->setProperty("Ice.ThreadPool.Server.SizeMax", "2000");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "131072");

	properties->setProperty("Ice.MessageSizeMax", "10240");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");
	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty(
			"Ice.Default.Locator",
			"wService/Locator:default -h wServiceMaster -p 4064:default -h wServiceReplica1 -p 4064:default -h wServiceReplica2 -p 4064");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("wService/Query"));
}

WServiceChannel::~WServiceChannel() {

}

//---------------------------------------------------------------------------
WServiceUGCChannel::WServiceUGCChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName", "XwServiceUGC");

	properties->setProperty("Ice.ThreadPool.Client.SizeMax", "2000");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "131072");
	properties->setProperty("Ice.ThreadPool.Server.SizeMax", "2000");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "131072");

	properties->setProperty("Ice.MessageSizeMax", "10240");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");
	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty(
			"Ice.Default.Locator",
			"XwServiceUGC/Locator:tcp -h XwServiceUGCMaster -p 4082:tcp -h XwServiceUGCReplica1 -p 4082:tcp -h XwServiceUGCReplica2 -p 4082");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XwServiceUGC/Query"));
}

WServiceUGCChannel::~WServiceUGCChannel() {

}

//---------------------------------------------------------------------------
WServiceInviteChannel::WServiceInviteChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName", "XwServiceInvite");

	properties->setProperty("Ice.ThreadPool.Client.SizeMax", "2000");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "131072");
	properties->setProperty("Ice.ThreadPool.Server.SizeMax", "2000");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "131072");

	properties->setProperty("Ice.MessageSizeMax", "10240");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");
	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty(
			"Ice.Default.Locator",
			"XwServiceInvite/Locator:tcp -h XwServiceInviteMaster -p 4087:tcp -h XwServiceInviteReplica1 -p 4087:tcp -h XwServiceInviteReplica2 -p 4087");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XwServiceInvite/Query"));
}

WServiceInviteChannel::~WServiceInviteChannel() {

}

//---------------------------------------------------------------------------
UserMonitorChannel::UserMonitorChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();

	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName","XiaoNeiPredictor");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	//			properties->setProperty("Ice.Trace.Protocol", "3");
	//			properties->setProperty("Ice.Trace.Network", "3");
	//			properties->setProperty("Ice.ProgramName", "ice");
	//			properties->setProperty("Ice.UseSyslog", "1");

	properties->setProperty("Ice.Default.Locator",
			"XiaoNeiPredictor/Locator:default -h XiaoneiAegisRegistry -p 4061");

	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);

	_query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNeiPredictor/Query"));
}

UserMonitorChannel::~UserMonitorChannel() {

}
//---------------------------------------------------------------------------
XceCacheChannel::XceCacheChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNeiSearch");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
        //                      properties->setProperty("Ice.Trace.Protocol", "3");
        //                      properties->setProperty("Ice.Trace.Network", "3");

        //                      properties->setProperty("Ice.ProgramName", "ice");
        //                      properties->setProperty("Ice.UseSyslog", "1");

        //      properties->setProperty(
        //                      "Ice.Default.Locator",
        //                      "XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator", "SearchCache/Locator:default -h 10.2.17.61 -p 4061:default -h 10.2.17.61 -p 4061:default -h  10.2.17.61 -p 4061");
        Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("SearchCache/Query"));
}

XceCacheChannel::~XceCacheChannel() {

}

//---------------------------------------------------------------------------
ScoreChannel::ScoreChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "Score");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());
        //                      properties->setProperty("Ice.Trace.Protocol", "3");
        //                      properties->setProperty("Ice.Trace.Network", "3");

        //                      properties->setProperty("Ice.ProgramName", "ice");
        //                      properties->setProperty("Ice.UseSyslog", "1");

        //      properties->setProperty(
        //                      "Ice.Default.Locator",
        //                      "XiaoNei/Locator:default -h XiaoNeiTalkRegistry -p 4061:default -h XiaoNeiTalkRegistryReplica1 -p 4061:default -h XiaoNeiTalkRegistryReplica2 -p 4061");

	properties->setProperty("Ice.Default.Locator", "Score/Locator:default -h ScoreMaster -p 4070:default -h ScoreReplica1 -p 4070:default -h ScoreReplica2 -p 4070");

	Ice::InitializationData id;
        id.properties = properties;
        _communicator = Ice::initialize(id);

        _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Score/Query"));
}

ScoreChannel::~ScoreChannel() {

}


//---------------------------------------------------------------------------

BackupChannel::BackupChannel(){
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.ConnectTimeout", "300");
	properties->setProperty("IceGrid.InstanceName", "XiaoNei");

	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

	properties->setProperty("Ice.MessageSizeMax", "20480");

	properties->setProperty("Ice.Default.Host", IpAddr::inner());

	properties->setProperty(
			"Ice.Default.Locator",
			"XiaoNei/Locator:default -h 10.22.202.59 -p 12000:default -h 10.22.202.59 -p 12001:default -h 10.22.202.59 -p 12002");
	Ice::InitializationData id;
	id.properties = properties;
	_communicator = Ice::initialize(id);
	_query
			= IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XiaoNei/Query"));
}

BackupChannel::~BackupChannel() {

}


//---------------------------------------------------------------------------





void FlushBatchRequestsTimer::handle() {

	TaskManager::instance().schedule(new FlushBatchRequestsTimer(_channel,_delay));
	_channel->getCommunicator()->flushBatchRequests();
}

