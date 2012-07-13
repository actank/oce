/*
 * windows_live_importer.h
 *
 *  Created on: 2008-11-20
 *      Author: bbc
 */

#ifndef WINDOWS_LIVE_IMPORTER_H_
#define WINDOWS_LIVE_IMPORTER_H_

#include <WindowsLiveSchema.h>

#include <curl/curl.h>
#include "pugixml.hpp"

#include <iostream>
#include <string.h>

namespace xce {
namespace importer {

/**  for curl API **/
struct AuthData {
	const char *readptr;
	int sizeleft;
};

class WindowsLiveTokenProcessor {
public:
	WindowsLiveTokenProcessor(const std::string& email,
			const std::string& passwd);
	~WindowsLiveTokenProcessor();
	void initialize();
	std::string token();
	bool is_ok() {
		return is_ok_;
	}
	std::string message() {
		return message_;
	}
private:
	static std::string token_request(const std::string& email,
			const std::string& passwd, const std::string& appid =
					"your ApplicationID Here");
	std::string email_;
	std::string passwd_;

	std::string token_xml_;
	bool is_ok_;
	std::string message_;

	CURL *handle_;
	struct curl_slist *header_;
};

const static int MODE_WLID = 1;
const static int MODE_DELEGATION = 2;
const static int MODE_MESSENGER_LOGIN = 3;

class WindowsLiveContactsProcessor {
public:
	WindowsLiveContactsProcessor(const std::string& email,
			const std::string& token);
	WindowsLiveContactsProcessor(const std::string& email,
			const std::string& token, const std::string& location);
	~WindowsLiveContactsProcessor();
	void initialize();
	com::live::schema::TLiveContacts contacts();
	bool is_ok() {
		return is_ok_;
	}
	std::string message() {
		return message_;
	}
private:
	int mode_;
	std::string email_;
	std::string token_;
	std::string location_;

	std::string contacts_xml_;
	bool is_ok_;
	std::string message_;

	CURL *handle_;
	struct curl_slist *header_;

	static void setProfiles(const pugi::xml_node& xml,
			com::live::schema::TProfiles* obj);
	static void setEmails(const pugi::xml_node& xml,
			com::live::schema::TEmails* obj);
	static void setPhones(const pugi::xml_node& xml,
			com::live::schema::TPhones* obj);
	static void setLocations(const pugi::xml_node& xml,
			com::live::schema::TLocations* obj);
	static void setURIs(const pugi::xml_node& xml,
			com::live::schema::TURIs* obj);
};

}
}
#endif /* WINDOWS_LIVE_IMPORTER_H_ */
