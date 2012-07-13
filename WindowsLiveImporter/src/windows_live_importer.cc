/*
 * windows_live_importer.cc
 *
 *  Created on: 2008-11-20
 *      Author: bbc
 */

#include "windows_live_importer.h"

using namespace std;
using namespace com::live::schema;
using namespace xce::importer;
using namespace pugi;

namespace xce {
namespace importer {
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *data) {
	struct AuthData *pooh = (struct AuthData *) data;
	if (size * nmemb < 1)
		return 0;
	size_t wrote = 0;
	while (pooh->sizeleft && wrote < size * nmemb) {
		*(((char*) ptr) + wrote) = pooh->readptr[0];
		wrote++;
		pooh->readptr++;
		pooh->sizeleft--;
	}
	return wrote;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
	string *buff = (string *) data;
	buff->append((char *) ptr, size * nmemb);
	return size * nmemb;
}
}
}
;

WindowsLiveTokenProcessor::WindowsLiveTokenProcessor(const std::string& email,
		const std::string& passwd) :
	email_(email), passwd_(passwd), header_(NULL) {
	handle_ = curl_easy_init();
}

WindowsLiveTokenProcessor::~WindowsLiveTokenProcessor() {
	curl_slist_free_all(header_);
	curl_easy_cleanup(handle_);
}

void WindowsLiveTokenProcessor::initialize() {
	struct AuthData post_content;
	string request_str = token_request(email_, passwd_);
	post_content.readptr = request_str.c_str();
	post_content.sizeleft = strlen(post_content.readptr);
	string result;
	header_ = curl_slist_append(header_, "Content-Type: application/soap+xml");
	curl_easy_setopt(handle_, CURLOPT_URL,
			"https://dev.login.live.com/wstlogin.srf");
	curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, header_);
	curl_easy_setopt(handle_, CURLOPT_TIMEOUT_MS, 20000L);
	curl_easy_setopt(handle_, CURLOPT_POST, 1L);
	curl_easy_setopt(handle_, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(handle_, CURLOPT_READDATA, &post_content);
	curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &result);
	//#ifdef VERBOSE_MODE
	curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1L);
	//#endif
	curl_easy_setopt(handle_, CURLOPT_POSTFIELDSIZE,
			(curl_off_t) post_content.sizeleft);
	curl_easy_setopt(handle_, CURLOPT_NOSIGNAL, 1L);
	CURLcode success = curl_easy_perform(handle_);
	is_ok_ = (success == CURLE_OK) ? true : false;
	message_ = curl_easy_strerror(success);
	if (!is_ok_)
		return;
	long response_code = 0;
	success
			= curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &response_code);
	is_ok_ = (success == CURLE_OK && response_code == 200L) ? true : false;
	message_ = curl_easy_strerror(success);
	if (!is_ok_)
		return;
	token_xml_ = result;
	is_ok_
			= (token_xml_.find("RequestSecurityTokenResponse") != string::npos) ? true
					: false;
	if (!is_ok_) {
		string::size_type msg_begin = token_xml_.find("<psf:text>");
		string::size_type msg_end = token_xml_.find("</psf:text>");
		if (msg_begin == string::npos || msg_end == string::npos || msg_begin
				>= msg_end)
			return;
		message_ = token_xml_.substr(msg_begin + strlen("<psf:text>"), msg_end
				- msg_begin - strlen("<psf:text>"));
	} else {
		message_ = "OK";
	}
}

string WindowsLiveTokenProcessor::token() {
	string::size_type token_start = token_xml_.find('>', token_xml_.find(
			"BinarySecurityToken", 0)) + 1;
	string::size_type token_end = token_xml_.find(string(
			"</wsse:BinarySecurityToken>"), token_start);
	//cout << "start: " << token_start << " end: " << token_end << endl;
	return token_xml_.substr(token_start, token_end - token_start);
}

string WindowsLiveTokenProcessor::token_request(const string& email,
		const string& passwd, const string& appid) {
	return "<s:Envelope xmlns:s = \"http://www.w3.org/2003/05/soap-envelope\""
		"           xmlns:wsse = \"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\""
		"           xmlns:saml = \"urn:oasis:names:tc:SAML:1.0:assertion\""
		"           xmlns:wsp = \"http://schemas.xmlsoap.org/ws/2004/09/policy\""
		"           xmlns:wsu = \"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\""
		"           xmlns:wsa = \"http://www.w3.org/2005/08/addressing\""
		"           xmlns:wssc = \"http://schemas.xmlsoap.org/ws/2005/02/sc\""
		"           xmlns:wst = \"http://schemas.xmlsoap.org/ws/2005/02/trust\">"
		"   <s:Header>"
		"       <wlid:ClientInfo xmlns:wlid = \"http://schemas.microsoft.com/wlid\">"
		"           <wlid:ApplicationID>" + appid
			+ "</wlid:ApplicationID>"
				"       </wlid:ClientInfo>"
				"       <wsa:Action s:mustUnderstand = \"1\">http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue</wsa:Action>"
				"       <wsa:To s:mustUnderstand = \"1\">https://dev.login.live.com/wstlogin.srf</wsa:To>"
				"       <wsse:Security>"
				"               <wsse:UsernameToken wsu:Id = \"user\">"
				"                   <wsse:Username>" + email
			+ "</wsse:Username>"
				"                   <wsse:Password>" + passwd
			+ "</wsse:Password>"
				"               </wsse:UsernameToken>"
				"       </wsse:Security>"
				"   </s:Header>"
				"   <s:Body>"
				"       <wst:RequestSecurityToken Id = \"RST0\">"
				"           <wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>"
				"               <wsp:AppliesTo>"
				"                   <wsa:EndpointReference>"
				"                       <wsa:Address>http://live.com</wsa:Address>"
				"                   </wsa:EndpointReference>"
				"               </wsp:AppliesTo>"
				"           <wsp:PolicyReference URI = \"MBI\"></wsp:PolicyReference>"
				"       </wst:RequestSecurityToken>"
				"   </s:Body>"
				"</s:Envelope>";
}

/**********************************************************************************
 * WindowsLiveContactsProcessor
 **********************************************************************************/

#define PARSE_NODE(obj_name, xml_name, prop_name) {\
	obj_name.prop_name = xml_name.child(#prop_name).first_child().value();\
}

WindowsLiveContactsProcessor::WindowsLiveContactsProcessor(
		const std::string& email, const std::string& token) :
	email_(email), token_(token), location_(""), header_(NULL) {
	mode_ = MODE_WLID;
	handle_ = curl_easy_init();
}

WindowsLiveContactsProcessor::WindowsLiveContactsProcessor(
		const std::string& email, const std::string& token,
		const std::string& location) :
	email_(email), token_(token), location_(location), header_(NULL) {
	mode_ = MODE_DELEGATION;
	handle_ = curl_easy_init();
}

WindowsLiveContactsProcessor::~WindowsLiveContactsProcessor() {
	curl_slist_free_all(header_);
	curl_easy_cleanup(handle_);
}

void WindowsLiveContactsProcessor::initialize() {
	switch (mode_) {
	case MODE_WLID:
		header_ = curl_slist_append(header_, string(
				"Authorization: WLID1.0 t=\"" + token_ + "\"").c_str());
		curl_easy_setopt(handle_, CURLOPT_URL,
				string("https://cumulus.services.live.com/" + email_
						+ "/LiveContacts").c_str());
		break;
	case MODE_DELEGATION:
		header_ = curl_slist_append(header_, string(
				"Authorization: DelegatedToken  dt=\"" + token_ + "\"").c_str());
		curl_easy_setopt(handle_, CURLOPT_URL, string(
					"https://livecontacts.services.live.com/users/@L@"+location_+"/rest/LiveContacts").c_str());
		break;
	default:
		is_ok_ = false;
		return;
	}
	string result;
	header_ = curl_slist_append(header_,
			"Content-Type: application/soap+xml");
	//curl_easy_setopt(handle, CURLOPT_URL, string(
	//		"https://livecontacts.services.live.com/users/"+email+"/rest/LiveContacts/contacts").c_str());
	curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, header_);
	curl_easy_setopt(handle_, CURLOPT_TIMEOUT_MS, 20000L);
	curl_easy_setopt(handle_, CURLOPT_POST, 0L);
	curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &result);
	//#ifdef VERBOSE_MODE
	curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1L);
	//#endif
	//	curl_easy_setopt(handle_, CURLOPT_POSTFIELDSIZE, (curl_off_t)pooh.sizeleft);
	curl_easy_setopt(handle_, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(handle_, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(handle_, CURLOPT_SSL_VERIFYHOST, 0L);

	CURLcode success = curl_easy_perform(handle_);
	is_ok_ = success == CURLE_OK ? true : false;
	message_ = curl_easy_strerror(success);
	contacts_xml_ = result;
}

com::live::schema::TLiveContacts WindowsLiveContactsProcessor::contacts() {
	// cout << contacts_xml_ << endl;
	xml_document doc;
	doc.load(contacts_xml_.c_str());

	TLiveContacts result;
	xml_node docroot = doc.child("LiveContacts");
	xml_node xml_owner = docroot.child("Owner");
	if (xml_owner) {
		PARSE_NODE(result.Owner, xml_owner, CID);
		// result.Owner.WindowsLiveID = xml_owner.child("WindowsLiveID").first_child().value();
		PARSE_NODE(result.Owner, xml_owner, WindowsLiveID);
		WindowsLiveContactsProcessor::setProfiles(xml_owner.child("Profiles"),
				&result.Owner.Profiles);
		WindowsLiveContactsProcessor::setEmails(xml_owner.child("Emails"),
				&result.Owner.Emails);
		WindowsLiveContactsProcessor::setLocations(
				xml_owner.child("Locations"), &result.Owner.Locations);
		WindowsLiveContactsProcessor::setURIs(xml_owner.child("URIs"),
				&result.Owner.URIs);
	}
	xml_node xml_contacts = docroot.child("Contacts");
	if (xml_contacts) {
		for (xml_node xml_contact = xml_contacts.child("Contact"); xml_contact; xml_contact
				= xml_contact.next_sibling("Contact")) {
			TContact contact;
			PARSE_NODE(contact, xml_contact, ID);
			PARSE_NODE(contact, xml_contact, CID);
			PARSE_NODE(contact, xml_contact, WindowsLiveID);
			PARSE_NODE(contact, xml_contact, AutoUpdateEnabled);
			PARSE_NODE(contact, xml_contact, AutoUpdateStatus);
			WindowsLiveContactsProcessor::setProfiles(xml_contact.child(
					"Profiles"), &contact.Profiles);
			WindowsLiveContactsProcessor::setEmails(
					xml_contact.child("Emails"), &contact.Emails);
			WindowsLiveContactsProcessor::setLocations(xml_contact.child(
					"Locations"), &contact.Locations);
			WindowsLiveContactsProcessor::setURIs(xml_contact.child("URIs"),
					&contact.URIs);
			result.Contacts.push_back(contact);
		}
	}
	xml_node xml_tags = docroot.child("Tags");
	if (xml_tags) {

	}

	return result;
}

void WindowsLiveContactsProcessor::setProfiles(const xml_node& xml,
		TProfiles* obj) {
	//	xml.print(cout);
	xml_node xml_personal = xml.child("Personal");
	if (xml_personal) {
		PARSE_NODE(obj->Personal, xml_personal, NameToFileAs);
		PARSE_NODE(obj->Personal, xml_personal, NameTitle);
		PARSE_NODE(obj->Personal, xml_personal, FirstName);
		PARSE_NODE(obj->Personal, xml_personal, MiddleName);
		PARSE_NODE(obj->Personal, xml_personal, LastName);
		PARSE_NODE(obj->Personal, xml_personal, Suffix);
		PARSE_NODE(obj->Personal, xml_personal, YomiFirstName);
		PARSE_NODE(obj->Personal, xml_personal, YomiLastName);
		PARSE_NODE(obj->Personal, xml_personal, UniqueName);
		PARSE_NODE(obj->Personal, xml_personal, SortName);
		PARSE_NODE(obj->Personal, xml_personal, NickName);
		PARSE_NODE(obj->Personal, xml_personal, DisplayName);
		PARSE_NODE(obj->Personal, xml_personal, Birthdate);
		PARSE_NODE(obj->Personal, xml_personal, Anniversary);
		PARSE_NODE(obj->Personal, xml_personal, Gender);
		PARSE_NODE(obj->Personal, xml_personal, TimeZone);
		PARSE_NODE(obj->Personal, xml_personal, SpouseName);
	}
	xml_node xml_professional = xml.child("Professional");
	if (xml_professional) {
		PARSE_NODE(obj->Professional, xml_professional, JobTitle);
		PARSE_NODE(obj->Professional, xml_professional, Profession);
		PARSE_NODE(obj->Professional, xml_professional, Manager);
		PARSE_NODE(obj->Professional, xml_professional, Assistant);
	}
}

void WindowsLiveContactsProcessor::setEmails(const pugi::xml_node& xml,
		com::live::schema::TEmails* obj) {
	for (xml_node xml_email = xml.child("Email"); xml_email; xml_email
			= xml_email.next_sibling("Email")) {
		TEmail obj_email;
		PARSE_NODE(obj_email, xml_email, ID);
		PARSE_NODE(obj_email, xml_email, EmailType);
		PARSE_NODE(obj_email, xml_email, Address);
		PARSE_NODE(obj_email, xml_email, IsIMEnabled);
		PARSE_NODE(obj_email, xml_email, IsDefault);
		obj->push_back(obj_email);
	}
}
void WindowsLiveContactsProcessor::setPhones(const pugi::xml_node& xml,
		com::live::schema::TPhones* obj) {
	for (xml_node xml_phone = xml.child("Phone"); xml_phone; xml_phone
			= xml_phone.next_sibling("Phone")) {
		TPhone obj_phone;
		PARSE_NODE(obj_phone, xml_phone, ID);
		PARSE_NODE(obj_phone, xml_phone, PhoneType);
		PARSE_NODE(obj_phone, xml_phone, Number);
		PARSE_NODE(obj_phone, xml_phone, IsIMEnabled);
		PARSE_NODE(obj_phone, xml_phone, IsDefault);
		obj->push_back(obj_phone);
	}
}

void WindowsLiveContactsProcessor::setLocations(const pugi::xml_node& xml,
		com::live::schema::TLocations* obj) {
	for (xml_node xml_location = xml.child("Location"); xml_location; xml_location
			= xml_location.next_sibling("Location")) {
		TLocation obj_location;
		PARSE_NODE(obj_location, xml_location, ID);
		PARSE_NODE(obj_location, xml_location, LocationType);
		PARSE_NODE(obj_location, xml_location, Office);
		PARSE_NODE(obj_location, xml_location, Department);
		PARSE_NODE(obj_location, xml_location, CompanyName);
		PARSE_NODE(obj_location, xml_location, YomiCompanyName);
		PARSE_NODE(obj_location, xml_location, StreetLine);
		PARSE_NODE(obj_location, xml_location, StreetLine2);
		PARSE_NODE(obj_location, xml_location, PrimaryCity);
		PARSE_NODE(obj_location, xml_location, SecondaryCity);
		PARSE_NODE(obj_location, xml_location, SubDivision);
		PARSE_NODE(obj_location, xml_location, PostalCode);
		PARSE_NODE(obj_location, xml_location, CountryRegion);
		PARSE_NODE(obj_location, xml_location, Latitude);
		PARSE_NODE(obj_location, xml_location, Longitude);
		PARSE_NODE(obj_location, xml_location, IsDefault);
		obj->push_back(obj_location);
	}
}

void WindowsLiveContactsProcessor::setURIs(const pugi::xml_node& xml,
		com::live::schema::TURIs* obj) {
	for (xml_node xml_uri = xml.child("URI"); xml_uri; xml_uri
			= xml_uri.next_sibling("URI")) {
		TURI obj_uri;
		PARSE_NODE(obj_uri, xml_uri, ID);
		PARSE_NODE(obj_uri, xml_uri, URIType);
		PARSE_NODE(obj_uri, xml_uri, Name);
		PARSE_NODE(obj_uri, xml_uri, Address);
		obj->push_back(obj_uri);
	}
}
