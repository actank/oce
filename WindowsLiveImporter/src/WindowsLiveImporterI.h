#ifndef __WindowsLiveImporterI_h__
#define __WindowsLiveImporterI_h__

#include <WindowsLiveImporter.h>

#include "ObjectCacheI.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "event.h"

namespace xce {

namespace importer {

static const int CONTACTS_DATA = 0;
static const int TASK_LEVEL_LOGIN = 1;
static const int TASK_LEVEL_DELEGATION = 2;
//static const int TASK_LEVEL_PASSWORD = 3;

class WindowsLiveImporterManagerI: virtual public WindowsLiveImporterManager {
public:
	WindowsLiveImporterManagerI(int port);

	virtual void requestByPassword(Ice::Int, const std::string&, const std::string&, const Ice::Current&);
	virtual void requestByDelegationTokenAndLocationID(Ice::Int, const std::string&, const std::string&, const std::string&, const Ice::Current&);

	virtual void contactsByPassword_async(const xce::importer::AMD_WindowsLiveImporterManager_contactsByPasswordPtr&, Ice::Int,
			const std::string&, const std::string&, const Ice::Current&);
	virtual void contactsByDelegationTokenAndLocationID_async(const xce::importer::AMD_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDPtr&, Ice::Int,
			const std::string&, const std::string&, const std::string&, const Ice::Current&);

	virtual ::xce::importer::WindowsLiveContactsPtr response(::Ice::Int,
			const ::std::string&, const Ice::Current&);

	virtual bool verifyAccount(const std::string& email,
			const std::string& passwd, const Ice::Current&);

private:
	xce::importer::wlevent::WLEventPtr event_;
};

class ContactsData : virtual public Ice::Object {
public:
	void setState(const ::xce::importer::ImportState& state){
		IceUtil::Mutex::Lock lock(mutex_);
		state_ = state;
	}
	void setContacts(const ::com::live::schema::TLiveContacts& contacts){
		IceUtil::Mutex::Lock lock(mutex_);
		contacts_ = contacts;
	}
	WindowsLiveContactsPtr getWindowsLiveContacts(){
		IceUtil::Mutex::Lock lock(mutex_);
		return new WindowsLiveContacts(contacts_, state_);
	}
private:
	::com::live::schema::TLiveContacts contacts_;
	::xce::importer::ImportState state_;
	IceUtil::Mutex mutex_;
};
typedef IceUtil::Handle<ContactsData> ContactsDataPtr;

class ContactsDataFactoryI: public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

template<typename CT>
class MessengerLoginWorker: public MyUtil::Task {
public:
	MessengerLoginWorker(
			int xnid,
			const std::string& email,
			const std::string& passwd,
			const CT& callback =
					NULL) :
		MyUtil::Task(TASK_LEVEL_LOGIN), xnid_(xnid), email_(email), passwd_(passwd), callback_(callback) {
	}
	virtual void handle();
private:
	int xnid_;
	string email_;
	string passwd_;
	CT callback_;
};

template<typename CT>
class DelegationTokenAndLocationIDWorker: public MyUtil::Task {
public:
	DelegationTokenAndLocationIDWorker(
			int xnid,
			const std::string& email,
			const std::string& token,
			const std::string& location,
			const CT& callback =
					NULL) :
		MyUtil::Task(TASK_LEVEL_DELEGATION), xnid_(xnid), email_(email), token_(token), location_(location), callback_(callback) {
	}
	virtual void handle();
private:
	int xnid_;
	string email_;
	string token_;
	string location_;
	CT callback_;
};

/*
template<typename CT>
class PasswordWorker: public MyUtil::Task {
public:
	PasswordWorker(
			int xnid,
			const std::string& email,
			const std::string& passwd,
			const CT& callback =
					NULL) :
		MyUtil::Task(TASK_LEVEL_PASSWORD), xnid_(xnid), email_(email), passwd_(passwd), callback_(callback) {
	}
	virtual void handle();
private:
	int xnid_;
	string email_;
	string passwd_;
	CT callback_;
};
*/

}
}

#endif
