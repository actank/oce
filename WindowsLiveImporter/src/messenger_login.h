/*
 * windows_live_importer.h
 *
 *  Created on: 2008-11-20
 *      Author: bbc
 */

#ifndef MESSENGER_LOGIN_PROCESSOR_H_
#define MESSENGER_LOGIN_PROCESSOR_H_

#include <WindowsLiveSchema.h>

#include <iostream>
#include <string.h>

#include <ImporterMSNWorker.h>
#include <IceUtil/IceUtil.h>


namespace xce {
namespace importer {

class MessengerLoginProcessor {
public:
	MessengerLoginProcessor(int xnId, const std::string& email,
			const std::string& passwd);
	~MessengerLoginProcessor();
	void initialize();
	com::live::schema::TLiveContacts contacts();
	void finish_contacts(bool is_ok, const com::live::schema::TLiveContacts& contacts);
	bool is_ok() {
		return is_ok_;
	}
private:
	const int xnId_;
	const std::string email_;
	const std::string passwd_;

	IceUtil::RWRecMutex mutex_;
	bool is_done_;
	com::live::schema::TLiveContacts contacts_;

	bool is_ok_;
};

class AMI_ImporterMSNWorker_contactsI : virtual public AMI_ImporterMSNWorker_contacts
{
public:
	AMI_ImporterMSNWorker_contactsI(MessengerLoginProcessor& processor);
    virtual void ice_response(const ::com::live::schema::TLiveContacts&);
    virtual void ice_exception(const ::Ice::Exception&);
private:
	MessengerLoginProcessor& processor_;
};

}
}
#endif /* MESSENGER_LOGIN_PROCESSOR_H_ */
