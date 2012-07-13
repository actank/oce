#include "NotifyCacheI.h"
#include "Date.h"

using namespace mop::hi::svc::dispatch;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI::instance().getObjectCache()->registerCategory(TYPE_NOTE, "N", new NoteFactory);
}

//---------------------------------------------------------------------------
NoteFactory::NoteFactory() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	_database = props->getProperty("Service."+service.getName()+".Database");
	_databaseOld = props->getProperty("Service."+service.getName()
			+".DatabaseOld");
	_noteTable = props->getProperty("Service."+service.getName()+".NoteTable");
}

NoteFactory::~NoteFactory() {
	
}

ObjectResultPtr NoteFactory::create(const IntSeq& keys) {
	ConnectionHolder conn(_database);
	mysqlpp::Query query = conn.query();
	query << "SELECT * FROM " << _noteTable << " WHERE noteId in (";
	for (size_t i = 0; i < keys.size(); ++i) {
		query << keys.at(i) << ", ";
	}
	query << "0)";
	mysqlpp::Result res = query.store();
	ObjectResultPtr result = new ObjectResult;
	for (size_t i = 0; i < res.size(); ++i) {
		mysqlpp::Row row = res.at(i);
		NotePtr n = new Note;
		n->noteId = row["NOTEID"];
		n->quoteeId = row["QUOTEEID"];
		n->resource = row["RESOURCE"];
		n->type = row["TYPE"];
		n->unread = (int)row["UNREAD"] == 0 ? false : true;
		n->fromName = row["FROMNAME"].get_string();
		n->fromId = row["FROMID"];
		n->toId = row["TOID"];
		n->ownerName = row["OWNERNAME"].get_string();
		n->ownerId = row["OWNERID"];
		n->title = row["TITLE"].get_string();
		n->link = row["LINK"].get_string();
		n->imageLink = row["IMAGELINK"].get_string();
		n->timestamp = (time_t)mysqlpp::DateTime(row["TIMESTAMP"]);
		n->security = row["SECURITY"];
		result->data[n->noteId] = n;
	}
	return result;
}
