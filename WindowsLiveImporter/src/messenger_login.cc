
#include <WindowsLiveSchema.h>

#include <iostream>
#include <string.h>

#include "messenger_login.h"
#include "ImporterMSNWorkerAdapter.h"
    
using namespace std;
using namespace com::live::schema;
using namespace xce::importer;

AMI_ImporterMSNWorker_contactsI::AMI_ImporterMSNWorker_contactsI(MessengerLoginProcessor& processor)
	: processor_(processor)
{
}

void
AMI_ImporterMSNWorker_contactsI::ice_response(const ::com::live::schema::TLiveContacts& contacts) {
	MCE_INFO("[AMI_ImporterMSNWorker_contactsI] ice_response: contacts.Owner.WindowsLiveId = " << contacts.Owner.WindowsLiveID
		 << " contacts.Contacts.size() = " << contacts.Contacts.size());
	processor_.finish_contacts(true, contacts);
}

void
AMI_ImporterMSNWorker_contactsI::ice_exception(const ::Ice::Exception& e) {
	MCE_WARN("[AMI_ImporterMSNWorker_contactsI] ice_exception: " << e)
	processor_.finish_contacts(false, ::com::live::schema::TLiveContacts());
}

MessengerLoginProcessor::MessengerLoginProcessor(int xnId, const std::string& email, const std::string& passwd)
	: xnId_(xnId), email_(email), passwd_(passwd), is_done_(false), is_ok_(false) {
}


MessengerLoginProcessor::~MessengerLoginProcessor() {
}

void
MessengerLoginProcessor::initialize() {	
	/*
	MCE_DEBUG("MessengerLoginProcessor::initialize " << xnId_ << " " << email_ << " " << passwd_ << " " << __LINE__);
	AMI_ImporterMSNWorker_contactsPtr callback = new AMI_ImporterMSNWorker_contactsI(*this);
	MCE_DEBUG("MessengerLoginProcessor::initialize " << xnId_ << " " << email_ << " " << passwd_ << " " << __LINE__);
	xce::importer::adapter::ImporterMSNWorkerAdapter::instance().getImporterMSNWorker(xnId_)->contacts_async(callback, email_, passwd_);
	MCE_DEBUG("MessengerLoginProcessor::initialize " << xnId_ << " " << email_ << " " << passwd_ << " " << __LINE__);
	int retry = 30;
	while(retry-->0) {
		MCE_DEBUG("MessengerLoginProcessor::initialize " << xnId_ << " " << email_ << " " << passwd_ << " " << retry << " " << __LINE__);
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			if (is_done_) break;
		}
		sleep(1);
	}
	*/

	struct timeval tvStart,tvEnd;
	gettimeofday( &tvStart, NULL );
	double linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);

	try{
		MCE_INFO("[MessengerLoginProcessor] < " << xnId_ << " " << email_ << " " << passwd_ << " > ImporterMSNWorker initializing ...");

		com::live::schema::TLiveContacts result = 
			xce::importer::adapter::ImporterMSNWorkerAdapter::instance().
				getImporterMSNWorker(xnId_)->contacts(email_, passwd_);

		gettimeofday( &tvEnd, NULL );
		double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
		double linEndTime = linEnd - linStart;
		linEndTime = linEndTime/1000000;

		MCE_INFO("[MessengerLoginProcessor] < " << xnId_ << " " << email_ << " " << passwd_ << " > ImporterMSNWorker contact_success (" << linEndTime << "s)");
		finish_contacts(true, result);
	}catch(...){
		gettimeofday( &tvEnd, NULL );
		double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
		double linEndTime = linEnd - linStart;
		linEndTime = linEndTime/1000000;

		MCE_WARN("[MessengerLoginProcessor] < " << xnId_ << " " << email_ << " " << passwd_ << " > ImporterMSNWorker contact_failed (" << linEndTime << "s)");
		finish_contacts(false, com::live::schema::TLiveContacts());
	}
}

com::live::schema::TLiveContacts
MessengerLoginProcessor::contacts() {
	IceUtil::RWRecMutex::RLock lock(mutex_);
	return contacts_;
}

void
MessengerLoginProcessor::finish_contacts(bool is_ok, const com::live::schema::TLiveContacts& contacts) {
	MCE_INFO("[MessengerLoginProcessor] < " << xnId_ << " " << email_ << " " << passwd_ << " > ImporterMSNWorker finish_contacts " << is_ok 
		<< " contacts.Owner.WindowsLiveID = " << contacts.Owner.WindowsLiveID 
		<< " contacts.Contacts.size() = " << contacts.Contacts.size() );
	IceUtil::RWRecMutex::WLock lock(mutex_);
	is_ok_ = is_ok;
	is_done_ = true;
	contacts_ = contacts;
}

