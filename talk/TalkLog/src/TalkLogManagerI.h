#ifndef __USERACTRECORD_H__
#define __USERACTRECORD_H__

#include <string>
#include <Ice/Ice.h>
#include "pugixml.hpp"
#include <IceUtil/Shared.h>
#include "Singleton.h"
#include "TalkLog.h"
using namespace std;
using namespace com::xiaonei::talk::common;
using namespace pugi;
using namespace MyUtil;
using namespace xiaonei::talk;


class RecordBase : public IceUtil::Shared{
public:
	virtual string str();
	virtual void load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc);
	virtual void handle();
protected:
	string _record;
};

class IqRecord : public RecordBase{
	virtual void load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc);
};

class PresenceRecord : public RecordBase{
	virtual void load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc);
};

class MessageRecord : public RecordBase{
	virtual void load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc);
};

typedef IceUtil::Handle<RecordBase> RecordBasePtr;


class RecordFactory{
public:
	RecordBasePtr create(const JidPtr& from, const JidPtr& to, const string& act);
};

class RecordManagerI : public TalkLogManager, public Singleton<RecordManagerI>{
public:
	virtual void feed(const JidPtr& from, const JidPtr& to, const string& act, const Ice::Current& = Ice::Current());
private:
	RecordFactory _factory;
};

#endif
