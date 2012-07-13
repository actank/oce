#ifndef __TALK_PROXY_ICE__
#define __TALK_PROXY_ICE__

#include <TalkCommon.ice>

module com
{
module xiaonei
{
module talk
{
	interface DeliverInterface{
		void express(common::Jid jid, string msg);
		void deliver(common::MessageSeq msgs);
	};
	
	interface ProxyManager extends DeliverInterface
	{
		bool indexExist(long index);
		bool checkTicket(common::Jid jid , string ticket);
		bool verify(common::Jid jid, string ticket);
		string GetTicket(common::Jid jid);
		
		void leave(common::Jid jid);
		void relogin();
		
		void broadcast(string xmlMsg);
		//void event(common::EventSeq seq);
		void kick(common::Jid jid);
	};
};
};
};

#endif
