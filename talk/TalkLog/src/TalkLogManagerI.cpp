#include "TalkLogManagerI.h"
#include "IceLogger.h"
#include "XmppTools.h"
#include "ServiceI.h"
using namespace com::xiaonei::talk::util;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&RecordManagerI::instance(), service.createIdentity("M", ""));
}


string RecordBase::str(){
	return _record;
}
void RecordBase::load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc){
	xml_node n = doc->first_child();
	string proto = n.name();
	ostringstream os;
	if(from){
		os << "[IN] ";
	}else if(to){
		os << "[OUT] ";
	}
	os << "{proto:" << proto << "} {type:" << n.attribute("type").value() << "}";

	vector<string> ids;
	ids.push_back(n.attribute("from").value());
	ids.push_back(n.attribute("to").value());
	for(size_t i=0; i<ids.size(); i++){
		if(!ids.at(i).empty()){
			IdType tmp = idType(ids.at(i));
			switch(tmp){
				case JID:
				{
					JidPtr f = stringToJid(ids.at(i));
					os << " {userid:" << f->userId << " endpoint:" << f->endpoint << " index:" << f->index << "}";
					break;
				}
				case ROOMID:
				{
					RoomIdPtr rId = stringToRoomId(ids.at(i));
					os << " {roomname:" << rId->name << "}";
					break;
				}
				case ROOMJID:
				{
					RoomJidPtr rJid = stringToRoomJid(ids.at(i));
					os << " {roomname:" << rJid->rid->name << " nick:" << rJid->nick << "}";
					break;
				}
				default:
				{
					os << " {id:" << ids.at(i) <<"}";
				}
			}
		}else{
			if(i==0 && from){
				os << " {userid:" << from->userId << " endpoint:" << from->endpoint << " index:" << from->index << "}";
			}else if(i==1 && to){
				os << " {userid:" << to->userId << " endpoint:" << to->endpoint << " index:" << to->index << "}";
			}else{
				os << " {}";
			}
		}
	}

	os << " {protoid:" << n.attribute("id").value() << "}";

	_record = os.str();
}
void RecordBase::handle(){
	MCE_DEBUG(_record);
};

//----------------------------------------------------------------------------
void IqRecord::load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc){
	RecordBase::load(from, to, doc);
	ostringstream os;
	os << " {ns:" << doc->child("iq").child("query").attribute("xmlns").value() << "}";
	_record += os.str();
}

//----------------------------------------------------------------------------
void PresenceRecord::load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc){
	RecordBase::load(from, to, doc);
	ostringstream os;
	xml_node presenceNode = doc->child("presence");
	os << " {show:" << presenceNode.child("show").first_child().value() << "}";
	os << " {status:" << presenceNode.child("status").first_child().value() << "}";
	_record += os.str();
}

//----------------------------------------------------------------------------
void MessageRecord::load(const JidPtr& from, const JidPtr& to, const xml_document_ptr doc){
	RecordBase::load(from, to, doc);
}

//----------------------------------------------------------------------------
RecordBasePtr RecordFactory::create(const JidPtr& from, const JidPtr& to, const string& act){
	xml_document_ptr doc = new xml_document;
	if(!doc || !doc->load(act.c_str())){
		return 0;
	}
	RecordBasePtr rec = 0;
	string type = doc->first_child().name();
	if(type == "iq"){
		rec = new IqRecord;
	}else if(type == "presence"){
		rec = new PresenceRecord;
	}else if(type == "message"){
		rec = new MessageRecord;
	}else{
		rec = new RecordBase;
	}
	rec->load(from, to, doc);
	return rec;
}

//----------------------------------------------------------------------------
void RecordManagerI::feed(const JidPtr& from, const JidPtr& to, const string& act, const Ice::Current&){
	RecordBasePtr rec = _factory.create(from, to, act);
	if(rec){
		rec->handle();
	}
}
