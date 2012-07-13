/*
 * event.cc
 *
 *  Created on: 2008-11-30
 *      Author: bbc
 */

#include "event.h"

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "WindowsLiveImporterAdapter.h"

using namespace xce::importer::adapter;
using namespace xce::importer;
using namespace xce::nio;
using namespace std;
using namespace boost;

using namespace xce::importer::wlevent;

static const std::string expect_header = "POST /event HTTP";
bool Session::messaged(const string& message) {
	cout << "*********************Message Begin **************************" <<endl;
	cout << message << endl;
	cout << "*********************Message End*******************" <<endl;
	if (message.substr(0, expect_header.length()) != expect_header
		&& _cachedMessage.empty())
	{
		deliver("HTTP/1.1 400 Bad Request\r\n\r\n");
		return true;
	}
	_cachedMessage.append(message);
	size_t header_edge = _cachedMessage.find("\r\n\r\n");
	if (header_edge == string::npos)
	{
		cout << "**** header_edge not ready ***" << endl;
		return true;
	}

	int content_length = -1;
	size_t content_length_begin=_cachedMessage.find("Content-Length: ");
	if (content_length_begin != string::npos)
	{
		size_t content_length_end = _cachedMessage.find("\r\n", content_length_begin);
		if (content_length_end == string::npos)
		{
			deliver("HTTP/1.1 400 Bad Request\r\n\r\n");
			return true;
		}
		size_t content_data_begin = _cachedMessage.find_first_of("1234567890", content_length_begin);
		cout << "content_data_begin " << content_data_begin << endl;
		size_t content_data_end = _cachedMessage.find_first_not_of("1234567890", content_data_begin);
		cout << "content_data_end " << content_data_end << endl;
		if (content_data_end != content_length_end)
		{
			deliver("HTTP/1.1 400 Bad Request\r\n\r\n");
			return true;
		}
		cout << "**** content length is " <<_cachedMessage.substr(content_data_begin, content_data_end - content_data_begin) << endl;
		content_length = boost::lexical_cast<int>(_cachedMessage.substr(content_data_begin, content_data_end - content_data_begin));
	}
	cout << "**** content length is " << content_length << endl;
	
	cout << "** header_edge + content_length = " << header_edge + strlen("\r\n\r\n") + content_length << endl;
	cout << "** _cachedMessage.length() = " << _cachedMessage.length() << endl;
	if ( content_length > 0 && header_edge + strlen("\r\n\r\n") + content_length < _cachedMessage.length() )
	{
		cout << "**** Content length not enough *** header_edge + content_length = " << header_edge + content_length << endl;
		cout << "**** Content length not enough *** _cachedMessage.length() = " << _cachedMessage.length() << endl;
		return true;
	}

	size_t uid_begin=0, uid_end=0,
		email_begin=0, email_end=0,
		password_begin=0, password_end=0;
	uid_begin = _cachedMessage.find("userid=", header_edge) + strlen("userid=");
	uid_end = _cachedMessage.find("&email=", uid_begin);
	email_begin = uid_end + strlen("&email=");
	email_end = _cachedMessage.find("&password=");
	password_begin = email_end + strlen("&password=");
	password_end = _cachedMessage.length();
	if (uid_begin==string::npos || uid_end==string::npos
		|| email_begin==string::npos || email_end==string::npos
		|| password_begin==string::npos || password_end==string::npos)
	{
		deliver("HTTP/1.1 400 Bad Request\r\n\r\n");
		return true;
	}
	if (uid_begin < uid_end
		&& uid_end < email_begin
		&& email_begin < email_end
		&& email_end < password_begin
		&& password_begin < password_end)
	{
		cout << "*** MES: " << _cachedMessage << endl;
		cout << "*** POS: " << header_edge << " " << uid_begin << " " << email_begin << " " << password_begin
			<< " " << uid_end << " " << email_end << " " << password_end << endl;

		string suid=_cachedMessage.substr(uid_begin, uid_end-uid_begin);
		if (suid.find_first_not_of("1234567890") != string::npos)
		{
			deliver("HTTP/1.1 400 Bad Request\r\n\r\n");
			return true;
		}
		int uid=boost::lexical_cast<int>(suid);
		string email=_cachedMessage.substr(email_begin, email_end-email_begin);
		string password=_cachedMessage.substr(password_begin, password_end-password_begin);

		cout << "*** STR: " << "uid: " << uid << "\n" 
			<< "email: " << email << "\n"
			<< "passwd: " << password << "\n" << endl;

		//deliver("HTTP/1.1 200 OK\r\n\r\n\r\n");
/*
		AMI_WindowsLiveImporterManager_contactsIPtr callback =
				new AMI_WindowsLiveImporterManager_contactsI(this);
		WindowsLiveImporterAdapter::instance().getWindowsLiveImporterManager(uid)->contacts_async(
				callback, uid, email, password);
*/
	}else{
		deliver("HTTP/1.1 400 Bad Request\r\n\r\n");
		return true;
	}
	return true;
}

void WLEvent::initialize() {
	::xce::nio::RefactorPtr refactor_ = new Refactor(port_, 0,
			new SessionFactoryI, 10, "");
	refactor_->start();
}

/*
void AMI_WindowsLiveImporterManager_contactsI::ice_response(
		const ::xce::importer::WindowsLiveContactsPtr& obj) {
	ostringstream body;
	body << obj->state;
	body << " Contact Owner: " << obj->contacts.Owner.WindowsLiveID << "\r\n";
	body << obj->contacts.Contacts.size() << " Contacts";

	ostringstream out;
	out << "HTTP/1.1 200 OK\r\n" << "Cache-Control: no-cache\r\n"
			<< "Pragma: no-cache\r\n" << "Expires: 0\r\n"
			<< "Content-Type: text/html\r\n" << "Content-Length: "
			<< body.str().length() << "\r\n\r\n";
	session_->deliver(out.str() + body.str());
}

void AMI_WindowsLiveImporterManager_contactsI::ice_exception(const ::Ice::Exception& e) {
	ostringstream body;
	body << "ice_exception \r\n" << e;
	
	ostringstream out;
	out << "HTTP/1.1 200 OK\r\n" << "Cache-Control: no-cache\r\n"
			<< "Pragma: no-cache\r\n" << "Expires: 0\r\n"
			<< "Content-Type: text/html\r\n" << "Content-Length: "
			<< body.str().length() << "\r\n\r\n";
	session_->deliver(out.str() + body.str());
}
*/

