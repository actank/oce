#ifndef _PERSONALMESSAGE_ICE_
#define _PERSONALMESSAGE_ICE_

#include <Util.ice>
#include <DataWrapper.ice>

module mop
{
module hi
{
module svc
{
module personalmessage
{
	class Gossip
	{
		int sender;
		string senderName;
		string senderUniv;
		int owner;
		string tinyurl;
		string body;
		int time;
	};
	class Message
	{
		int source;
		string sourceName;
		bool saveInSourceOutbox;
		int target;
		string targetName;
		bool saveInTargetInbox;
		string subject;
		string body;
		int time;
		int type;
	};
	class Email
	{
		string from;
		MyUtil::StrSeq to;
		MyUtil::StrSeq cc;
		MyUtil::StrSeq bcc;
		string subject;
		string body;
		bool isHtml;
	};

	interface PersonalMessageManager
	{
		int sendGossip(Gossip word);
		int sendMessage(Message msg);
		int sendEmail(Email mail);
	};
};
};
};
};

#endif
