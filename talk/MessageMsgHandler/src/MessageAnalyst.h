#ifndef MESSAGEFACTORY_H_
#define MESSAGEFACTORY_H_

#include "Singleton.h"
#include "TaskManager.h"
#include "TalkUtil/src/pugixml.hpp"
#include "TalkCommon.h"

namespace com
{
namespace xiaonei
{
namespace talk
{

using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;

class MessageAnalyst : public Singleton<MessageAnalyst>{
public:
	void handle(const JidPtr& jid, const string& msg, const string& ip = "");
};

class IqTaskBuilder : public Singleton<IqTaskBuilder>{
public:
	void build(const JidPtr& jid, const xml_document_ptr& doc);
};

class PresenceTaskBuilder : public Singleton<PresenceTaskBuilder>{
public:
	void build(const JidPtr& jid, const xml_document_ptr& doc);
};

class MessageTaskBuilder : public Singleton<MessageTaskBuilder>{
public:
	void build(const JidPtr& jid, const xml_document_ptr& doc, const string& ip);
};

};
};
};

#endif /*MESSAGEFACTORY_H_*/
