#include "WindowsLiveImporterI.h"

#include "windows_live_importer.h"
#include "messenger_login.h"

#include "ServiceI.h"

using namespace ::MyUtil;
using namespace ::xce::importer;
using namespace ::com::live::schema;

//***************************************************************************

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(CONTACTS_DATA, "WLC",
			new ContactsDataFactoryI);
	int
			port =
					service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
							"Event.Port", 0);
	service.getAdapter()->add(new WindowsLiveImporterManagerI(port),
			service.createIdentity("M", ""));
	TaskManager::instance().config(TASK_LEVEL_LOGIN, ThreadPoolConfig(1, 1000));
	TaskManager::instance().config(TASK_LEVEL_DELEGATION, ThreadPoolConfig(1, 1000));
	//TaskManager::instance().config(TASK_LEVEL_PASSWORD, ThreadPoolConfig(1, 1000));
}

//***************************************************************************
WindowsLiveImporterManagerI::WindowsLiveImporterManagerI(int port) {
	if (port > 1024) {
		MCE_INFO("[WindowsLiveImporterManagerI] port=" << port << " WLEvent initializing ..." );
		event_ = new xce::importer::wlevent::WLEvent(port);
		event_->initialize();
	} else{
		MCE_INFO("[WindowsLiveImporterManagerI] port=" << port << " no WLEvent initializing" );
	}
}

void WindowsLiveImporterManagerI::requestByPassword(Ice::Int xnId,
		const std::string& email, const std::string& passwd,
		const Ice::Current& current) {
	MCE_INFO("[requestByPassword] < " << xnId << " " << email << " " << passwd << " > " );
	ContactsDataPtr contacts = ServiceI::instance() .locateObject<
		ContactsDataPtr> (CONTACTS_DATA, xnId);
	TaskManager::instance().execute(new MessengerLoginWorker<
			xce::importer::AMD_WindowsLiveImporterManager_contactsByPasswordPtr> (xnId,
				email, passwd));
}

void WindowsLiveImporterManagerI::requestByDelegationTokenAndLocationID(
		Ice::Int xnId, const std::string& email, const std::string& token,
		const std::string& location, const Ice::Current& current) {
	MCE_INFO("[requestByDelegationTokenAndLocationID] < " << xnId << " " << email << " " << token << " " << location << " > " );
	ContactsDataPtr contacts = ServiceI::instance() .locateObject<
		ContactsDataPtr> (CONTACTS_DATA, xnId);
	TaskManager::instance().execute(
			new DelegationTokenAndLocationIDWorker<
			xce::importer::AMD_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDPtr> (
				xnId, email, token, location));
}

void WindowsLiveImporterManagerI::contactsByPassword_async(
		const xce::importer::AMD_WindowsLiveImporterManager_contactsByPasswordPtr& callback,
		Ice::Int xnId, const std::string& email, const std::string& passwd,
		const Ice::Current& current) {
	MCE_INFO("[contactsByPassword_async] < " << xnId << " " << email << " " << passwd << " > " );
	ContactsDataPtr contacts = ServiceI::instance().locateObject<
		ContactsDataPtr> (CONTACTS_DATA, xnId);
	TaskManager::instance().execute(
			new MessengerLoginWorker<
			xce::importer::AMD_WindowsLiveImporterManager_contactsByPasswordPtr> (
				xnId, email, passwd, callback));
}

void WindowsLiveImporterManagerI::contactsByDelegationTokenAndLocationID_async(
		const xce::importer::AMD_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDPtr& callback,
		Ice::Int xnId, const std::string& email, const std::string& token, const std::string& location,
		const Ice::Current& current) {
	MCE_INFO("[contactsByDelegationTokenAndLocationID_async] < " << xnId << " " << email << " " << token << " " << location << " > " );
	ContactsDataPtr contacts = ServiceI::instance().locateObject<
		ContactsDataPtr> (CONTACTS_DATA, xnId);
	TaskManager::instance().execute(
			new DelegationTokenAndLocationIDWorker<
			xce::importer::AMD_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDPtr> (
				xnId, email, token, location, callback));
}

WindowsLiveContactsPtr xce::importer::WindowsLiveImporterManagerI::response(
		::Ice::Int xnId, const ::std::string& email,
		const Ice::Current& current) {
	ContactsDataPtr data = ServiceI::instance() .findObject<
		ContactsDataPtr> (CONTACTS_DATA, xnId);
	if (!data) {
		MCE_WARN("[response] < " << xnId << " " << email << " > object_cache_miss! return null");
		return 0;
	}
	WindowsLiveContactsPtr contacts = data->getWindowsLiveContacts();
	if (contacts->state == RETRIVED && contacts->contacts.Owner.WindowsLiveID
			!= email) {
		MCE_WARN("[response] < " << xnId << " " << email << " > requested_email="
				<< contacts->contacts.Owner.WindowsLiveID << " request_diff_requested! return null" );
		return 0;
	}
	MCE_INFO("[response] < " << xnId << " " << email << " > state=" << contacts->state << " contact_size=" << contacts->contacts.Contacts.size() );
	return contacts;
}

bool xce::importer::WindowsLiveImporterManagerI::verifyAccount(
		const std::string& email, const std::string& passwd,
		const Ice::Current&) {
	MCE_INFO("[Verify] < " << email << " " << passwd << " > start ...");
	WindowsLiveTokenProcessor token_processor(email, passwd);
	MCE_INFO("[Verify] < " << email << " " << passwd << " > initializing ...");
	token_processor.initialize();
	if (token_processor.is_ok()) {
		MCE_INFO("[Verify] < " << email << " " << passwd << " > success: " << token_processor.message());
		return true;
	} else {
		MCE_INFO("[Verify] < " << email << " " << passwd << " > failed: " << token_processor.message());
		return false;
	}
}

//***************************************************************************

Ice::ObjectPtr ContactsDataFactoryI::create(Ice::Int id) {
	ContactsDataPtr result = new ContactsData;
	result->setState(PENDING);
	return result;
}

//***************************************************************************

template<typename CT>
void MessengerLoginWorker<CT>::handle() {
	MCE_INFO("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > start ...");
	ContactsDataPtr data = ServiceI::instance().findObject<
		ContactsDataPtr> (CONTACTS_DATA, xnid_);
	if (!data) {
		MCE_WARN("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > obj_cache_miss_exception");
		if (callback_)
			callback_->ice_exception();
		return;
	}
	MCE_INFO("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > set_obj_cache: pending ...");
	data->setState(PENDING);

	{
		MessengerLoginProcessor processor(xnid_, email_, passwd_);
		MCE_INFO("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > initializing ...");
		processor.initialize();
		if (processor.is_ok()) {
			MCE_INFO("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > retrived");
			data->setContacts(processor.contacts());
			data->setState(RETRIVED);
		} else {
			MCE_INFO("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > retrive_failed");
			data->setContacts(processor.contacts());
			data->setState(RETRIVEFAILED);
		}
	}
	if (callback_){
		callback_->ice_response(data->getWindowsLiveContacts());
		MCE_INFO("[LoginWorker] < " << xnid_ << " " << email_ << " " << passwd_ << " > call_back");
	}
}

template<typename CT>
void DelegationTokenAndLocationIDWorker<CT>::handle() {
	MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > start ...");
	ContactsDataPtr data = ServiceI::instance().findObject<
		ContactsDataPtr> (CONTACTS_DATA, xnid_);
	if (!data) {
		MCE_WARN("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > obj_cache_miss_exception");
		if (callback_)
			callback_->ice_exception();
		return;
	}
	MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > set_obj_cache: authed ...");
	data->setState(AUTHED);

	{
		WindowsLiveContactsProcessor contacts_processor(email_, token_, location_);
		MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > initializing ...");
		data->setState(RETRIVING);
		contacts_processor.initialize();
		if (contacts_processor.is_ok()) {
			MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > retrived");
			data->setContacts(contacts_processor.contacts());
			data->setState(RETRIVED);
		} else {
			MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > retrive_failed");
			data->setState(RETRIVEFAILED);
			if (callback_){
				callback_->ice_response(data->getWindowsLiveContacts());
				MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > call_back");
			}
			return;
		}
	}
	if (callback_){
		callback_->ice_response(data->getWindowsLiveContacts());
		MCE_INFO("[D&LWorker] < " << xnid_ << " " << email_ << " " << token_ << " " << location_ << " > call_back");
	}
}

/*
template<typename CT>
void PasswordWorker<CT>::handle() {
	MCE_DEBUG("PasswordWorker started " << xnid_ << " " << email_ << " " << passwd_);
	ContactsDataPtr data = ServiceI::instance().findObject<
			ContactsDataPtr> (CONTACTS_DATA, xnid_);
	if (!data) {
		if (callback_)
			callback_->ice_exception();
		return;
	}
	MCE_DEBUG("PasswordWorker working " << xnid_ << " " << email_ << " " << passwd_);
	data->setState(PENDING);
	string token;
	{
		WindowsLiveTokenProcessor token_processor(email_, passwd_);
		data->setState(AUTHING);
		token_processor.initialize();
		if (token_processor.is_ok()) {
			token = token_processor.token();
			data->setState(AUTHED);
		} else {
			data->setState(AUTHFAILED);
			if (callback_)
				callback_->ice_response(data->getWindowsLiveContacts());
			return;
		}
	}
	MCE_DEBUG("PasswordWorker Auth done " << xnid_ << " " << email_ << " " << passwd_);

	{
		WindowsLiveContactsProcessor contacts_processor(email_, token);
		data->setState(RETRIVING);
		contacts_processor.initialize();
		if (contacts_processor.is_ok()) {
			data->setContacts(contacts_processor.contacts());
			data->setState(RETRIVED);
		} else {
			data->setState(RETRIVEFAILED);
			if (callback_)
				callback_->ice_response(data->getWindowsLiveContacts());
			return;
		}
	}
	if (callback_)
		callback_->ice_response(data->getWindowsLiveContacts());
	MCE_DEBUG("PasswordWorker Retrive done " << xnid_ << " " << email_ << " " << passwd_);
}
*/

