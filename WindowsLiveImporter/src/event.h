/*
 * event.h
 *
 *  Created on: 2008-11-30
 *      Author: bbc
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Refactor.h"
#include "WindowsLiveImporter.h"

namespace xce {
namespace importer {
namespace wlevent {

class Session: public xce::nio::AsioChannel {
public:
	Session(){
	}
	virtual ~Session() {
	}
	bool operator<(const Session& rhs) {
		return this < &rhs;
	}
protected:
	virtual void started() {
	}
	virtual void stopped() {
	}
	virtual bool messaged(const string& message);
	virtual void finishWrite() {
	}
private:
	std::string _cachedMessage;
};

class SessionFactoryI: public xce::nio::AsioChannelFactory {
public:
	virtual xce::nio::AsioChannelPtr create() {
		return new Session;
	}
};

class WLEvent : virtual public IceUtil::Shared {
public:
	WLEvent(int port) :
		port_(port) {
	}
	;
	void initialize();
private:
	::xce::nio::RefactorPtr refactor_;
	int port_;
};
typedef IceUtil::Handle<WLEvent> WLEventPtr;

/*
class AMI_WindowsLiveImporterManager_contactsI
	: public xce::importer::AMI_WindowsLiveImporterManager_contacts {
public:
	AMI_WindowsLiveImporterManager_contactsI(Session* session) :
		session_(session) {
	}
	virtual void ice_response(const ::xce::importer::WindowsLiveContactsPtr&);
	virtual void ice_exception(const ::Ice::Exception& e);
private:
	Session* session_;
};
typedef IceUtil::Handle<AMI_WindowsLiveImporterManager_contactsI>
		AMI_WindowsLiveImporterManager_contactsIPtr;
*/

}
}
}

#endif /* EVENT_H_ */
