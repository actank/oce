#include "NotifyI.h"
#include <deque>
#include <boost/tokenizer.hpp>
#include "ConnectionQuery.h"

using namespace mop::hi::svc::dispatch;
using namespace MyUtil;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(NOTE_SET, "NS", new SyncServantFactory(new ServantFactoryI));
	service.getAdapter()->add(new NotifierI, service.createIdentity(NOTIFIER, ""));
	NotifyCacheAdapter::instance().initializeWith(service.getAdapter());
}

//---------------------------------------------------------------------------
Ice::ObjectPtr ServantFactoryI::create(Ice::Int id) {
	NotifierI& service = NotifierI::instance();
	NoteSetPtr result = new NoteSet();
	ConnectionHolder conn(service.getDatabase());
	mysqlpp::Query query = conn.query();
	query << "SELECT * FROM " << service.getNoteDispatchedTable(id)
			<< " WHERE ownerId=" << id << " and timestamp>=" << mysqlpp::quote
			<< Date(time(NULL) - service.getNoteMaxTTL()).str() << " ORDER BY timestamp DESC LIMIT "
			<< service.getNoteMaxSize();
	mysqlpp::Result res = query.store();
	for (size_t i = 0; i < res.size(); ++i) {
		mysqlpp::Row row = res.at(i);
		HintPtr ns = new Hint;
		ns->noteId = row["NOTEID"];
		ns->unread = (int)row["UNREAD"] > 0 ? true : false;
		ns->resource = row["RESOURCE"];
		ns->type = row["TYPE"];
		ns->timestamp = (time_t)mysqlpp::DateTime(row["TIMESTAMP"]);
		result->notes.push_back(ns);
	}
	return result;
}

NotifierI::NotifierI() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	_database = props->getProperty("Service."+service.getName()+".Database");
	_noteTable = props->getProperty("Service."+service.getName()+".NoteTable");
	_noteDispatchedTable = props->getProperty("Service."+service.getName()
			+".DispatchedNoteTable");

	_noteDispatchedTableCount = props->getPropertyAsIntWithDefault("Service."
			+service.getName() +".DispatchedTableCount", 100);
	_noteMaxTTL = props->getPropertyAsIntWithDefault("Service."
			+service.getName()+".NoteMaxTTL", 3 *24*60 *60);
	_noteMaxSize = props->getPropertyAsIntWithDefault("Service."
			+service.getName()+".NoteSetMax", 100);// n notes

	string token = props->getProperty("Service." + service.getName()
			+ ".Note.Homotypes");
	MCE_DEBUG("token: " << token);

	boost::char_separator<char> colon(":");
	boost::tokenizer<boost::char_separator<char> > categories(token, colon);
	boost::tokenizer<boost::char_separator<char> >::iterator itc;
	for (itc = categories.begin(); itc != categories.end(); ++itc) {
		string category = *itc;
		MCE_DEBUG("Category: " << category);
		IntSeq vcTypes;
		boost::char_separator<char> comma(",");
		boost::tokenizer<boost::char_separator<char> > types(category, comma);
		boost::tokenizer<boost::char_separator<char> >::iterator itt;
		for (itt = types.begin(); itt != types.end(); ++itt) {
			vcTypes.push_back(atoi((*itt).c_str()));
		}
		_noteHomotypes.push_back(vcTypes);
	}
}

NotifierI::~NotifierI() {

}

NoteSetPtr NotifierI::getNoteSet(Ice::Int userId) {
	return NoteSetPtr::dynamicCast(ServiceI::instance().locateObject(NOTE_SET, userId));
}

string NotifierI::getDatabase() {
	return _database;
}
;

string NotifierI::getDatabaseOld() {
	return _databaseOld;
}
;

string NotifierI::getNoteTable() {
	return _noteTable;
}
;

string NotifierI::getNoteDispatchedTable(int ownerId) {
	ostringstream table;
	table << _noteDispatchedTable << "_" << (ownerId
			% getNoteDispatchedTableCount());
	MCE_DEBUG("----ownerId:"<<ownerId<<"  " <<getNoteDispatchedTableCount()
			<<" table:"<< table.str());

	return table.str();
}

int NotifierI::getNoteDispatchedTableCount() {
	MCE_DEBUG("--------------getNoteDispatchedTableCount"
			<<_noteDispatchedTableCount);
	return _noteDispatchedTableCount;
}

int NotifierI::getNoteMaxTTL() {
	return _noteMaxTTL;
}

int NotifierI::getNoteMaxSize() {
	return _noteMaxSize;
}

bool NotifierI::isHomotypic(Ice::Int source, Ice::Int target) {
	for (size_t i = 0; i < _noteHomotypes.size(); ++i) {
		IntSeq& types = _noteHomotypes.at(i);
		if (find(types.begin(), types.end(), source) == types.end()) {
			continue;
		}
		return find(types.begin(), types.end(), target) != types.end();
	}
	return false;
}

MyUtil::IntSeq NotifierI::notify(const NotePtr& theNote,
		const MyUtil::IntSeq& userIds, const Ice::Current&) {
	MyUtil::IntSeq result;
	for (size_t i = 0; i<userIds.size(); ++i) {
		if (!ServiceI::instance().hasObject(NOTE_SET, userIds.at(i))) {
			MCE_DEBUG("userid: " << userIds.at(i)
					<< " not in memory. SKIP notify.");
			result.push_back(userIds.at(i));
			continue;
		}
		getNoteSet(userIds.at(i))->add(theNote);
	}
	return result;
}

Ice::Int NotifierI::getCount(Ice::Int userId, Ice::Int type, bool unreadOnly,
		const Ice::Current&) {
	return NotifierI::instance().getNoteSet(userId)->getCount(type, unreadOnly);
}

NoteSeq NotifierI::getAll(Ice::Int userId, Ice::Int type, bool unreadOnly,
		Ice::Int begin, Ice::Int limit, const Ice::Current&) {
	return NotifierI::instance().getNoteSet(userId)->getAll(type, unreadOnly, begin, limit);
}

NoteResultSetPtr NotifierI::getResultSet(Ice::Int userId, Ice::Int type,
		bool unreadOnly, Ice::Int begin, Ice::Int limit, const Ice::Current&) {
	return NotifierI::instance().getNoteSet(userId)->getResultSet(type, unreadOnly, begin, limit);
}

void NotifierI::setRead(Ice::Int userId, Ice::Int resource, Ice::Int type,
		const Ice::Current&) {
	return NotifierI::instance().getNoteSet(userId)->setRead(userId, resource, type);
}

void NotifierI::setAllRead(Ice::Int userId, const Ice::Current&) {
	return NotifierI::instance().getNoteSet(userId)->setAllRead(userId);
}

//---------------------------------------------------------------------------
Ice::Int NoteSet::getCount(Ice::Int type, bool unreadOnly, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(*this);
	evictNotes();
	int result = 0;
	for (size_t i = 0; i < notes.size(); ++i) {
		if (!isMatched(notes.at(i), type, unreadOnly)) {
			continue;
		}
		result++;
	}
	return result;
}

NoteSeq NoteSet::getAll(Ice::Int type, bool unreadOnly, Ice::Int begin,
		Ice::Int limit, const Ice::Current&) {
	MCE_DEBUG("getAll invoke start, type->" << type << ", unread->"
			<< unreadOnly);
	NoteSeq result;
	HintSeq hints;

	{
		IceUtil::Mutex::Lock lock(*this);
		evictNotes();
		Ice::Int offset = 0;
		begin = begin < 0 ? 0 : begin;
		limit = limit < 0 ? notes.size() : limit;
		MCE_DEBUG("getAll loop start size: "<< notes.size());

		for (HintQueue::iterator it = notes.begin(); it != notes.end(); ++it) {
			if (hints.size() >= (size_t)limit) {
				break;
			}
			HintPtr& ns = *it;
			MCE_TRACE("Note: " << ns->noteId << ", type->" << ns->type
					<< ", unread->" << ns->unread);
			if (!isMatched(ns, type, unreadOnly) || (offset++ < begin)) {
				continue;
			}
			hints.push_back(ns);
		}
	}
	NotifyCacheAdapter::instance().getNoteList(hints, result);
	return result;
}

NoteResultSetPtr NoteSet::getResultSet(Ice::Int type, bool unreadOnly,
		Ice::Int begin, Ice::Int limit, const Ice::Current&) {
	NoteResultSetPtr result = new NoteResultSet;
	result->totalCount = notes.size();
	result->unreadCount = 0;
	HintSeq hints;
	{
		IceUtil::Mutex::Lock lock(*this);
		evictNotes();
		Ice::Int offset = 0;
		begin = begin < 0 ? 0 : begin;
		limit = limit < 0 ? notes.size() : limit;

		MCE_DEBUG("getResultSet loop start size: "<< notes.size());
		for (HintQueue::iterator it = notes.begin(); it != notes.end(); ++it) {
			HintPtr& ns = *it;
			MCE_DEBUG("Note: " << ns->noteId << ", type->" << ns->type
					<< ", unread->" << ns->unread);
			if (hints.size() >= (size_t)limit) {
				break;
			}
			if (ns->unread) {
				++(result->unreadCount);
			}
			if (!isMatched(ns, type, unreadOnly) || (offset++ < begin)) {
				continue;
			}
			hints.push_back(ns);
		}
	}
	NotifyCacheAdapter::instance().getNoteList(hints, result->notes);

	return result;
}

void NoteSet::setRead(Ice::Int ownerId, Ice::Int resource, Ice::Int type,
		const Ice::Current& current) {
	MCE_DEBUG("MarkRead with type: " << type << ", resource: " << resource);
	IceUtil::Mutex::Lock lock(*this);
	HintQueue updates;
	for (HintQueue::iterator it = notes.begin(); it != notes.end(); ++it) {
		HintPtr& ns = *it;
		if (!isHomotypic(ns, type, resource)) {
			MCE_DEBUG("The homotype of notes mismatches with type:" << type
					<< " resource:" << resource);
			continue;
		}
		if (ns->unread) {
			ns->unread = false;
			// save toId db
			updates.push_back(ns);
		}
	}
	saveReads(updates, ownerId);
}

void NoteSet::setAllRead(Ice::Int ownerId, const Ice::Current& current) {
	IceUtil::Mutex::Lock lock(*this);
	HintQueue updates;
	for (size_t i = 0; i < notes.size(); ++i) {
		if ( !notes.at(i)->unread) {
			continue;
		}
		notes.at(i)->unread = false;
		updates.push_back(notes.at(i));
	}
	saveReads(updates, ownerId);
}

void NoteSet::add(const NotePtr& theNote, const Ice::Current& current) {
	IceUtil::Mutex::Lock lock(*this);
	HintPtr ns = new Hint();
	ns->noteId = theNote->noteId;
	ns->unread = theNote->unread;
	ns->resource = theNote->resource;
	ns->type = theNote->type;
	ns->timestamp = theNote->timestamp;

	HintQueue::iterator pos = notes.end();
	for (HintQueue::iterator it = notes.begin(); it != notes.end(); ++it) {
		if (ns->noteId > (*it)->noteId) {
			pos = it;
			break;
		}
	}
	notes.insert(pos, ns);

	evictNotes();
}

void NoteSet::add(const HintQueue& theNotes) {
	IceUtil::Mutex::Lock lock(*this);
	notes.insert(notes.begin(), theNotes.begin(), theNotes.end());
}

void NoteSet::evictNotes() {
	NotifierI& service = NotifierI::instance();
	// remove outdated
	time_t thresholdTime = time(NULL) - service.getNoteMaxTTL();
	while (!notes.empty()) {
		if (notes.size() <= (size_t)service.getNoteMaxSize()
				&& notes.back()->timestamp > thresholdTime) {
			break;
		}
		notes.pop_back();
		MCE_DEBUG("Remove outdated notes. pop up. Remains: " << notes.size());
	}

}
bool NoteSet::isHomotypic(const HintPtr& ns, Ice::Int type, Ice::Int resource) {
	if (ns->resource != resource) {
		return false;
	};
	if (ns->type == ns->type) {
		return true;
	};
	return NotifierI::instance().isHomotypic(ns->type, type);
}

bool NoteSet::isMatched(const HintPtr& ns, Ice::Int type, bool unreadOnly) {
	if (ns->noteId <= 0) {
		return false;
	}
	if (unreadOnly && !ns->unread) {
		return false;
	}
	if (ALLTYPE != type && ns->type != type) {
		return false;
	}
	NotifierI& service = NotifierI::instance();
	if (ns->timestamp < time(NULL) - service.getNoteMaxTTL()) {
		return false;
	}

	return true;
}

void NoteSet::saveReads(const HintQueue& notes, Ice::Int ownerId) {
	MCE_DEBUG("** saveReads count: " << notes.size() << " ownerId: " << ownerId);
	if (notes.empty()) {
		MCE_DEBUG("** no notes to saveReads, return.");
		return;
	}
	NotifierI& service = NotifierI::instance();
	string table = service.getNoteDispatchedTable(ownerId);
	MCE_DEBUG("** constructing SQL statements");
	ostringstream sql;
	sql << "update " << table << " set unread=" << 0 << " where ownerId="
			<< ownerId << " and noteId in (";
	for (size_t i = 0; i < notes.size(); ++i) {
		sql << notes.at(i)->noteId;
		if (notes.size() -1 != i) {
			sql << ", ";
		}
	}
	sql << ")";
	TaskManager::instance().execute(new SaveReadTask(table, sql.str()));
}

void SaveReadTask::handle() {
	MCE_DEBUG("** do database **");
	NotifierI& service = NotifierI::instance();
	ConnectionQuery(service.getDatabase(), CDbWServer).doUpdate(_sql);
}

//---------------------------------------------------------------------------
void NotifyCacheAdapter::getNoteList(HintSeq& hints, NoteSeq& notes) {
	MCE_DEBUG("NotifyCacheAdapter::getNoteList.size -> " << hints.size());
	IntSeq noteIds;
	for (size_t i = 0; i < hints.size(); ++i) {
		noteIds.push_back((Ice::Int)hints.at(i)->noteId);
	}
	ObjectResultPtr om = get(TYPE_NOTE, noteIds);
	for (size_t i = 0; i < hints.size(); ++i) {
		HintPtr& hint = hints.at(i);
		ObjectMap::iterator pos = om->data.find(hint->noteId);
		if (pos != om->data.end() && pos->second) {
			NotePtr note = NotePtr::dynamicCast(pos->second);
			note->unread = hint->unread;
			notes.push_back(note);
		} else {
			hint->noteId = -1;
		}
	}
}
