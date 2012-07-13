#include "XceAlertI.h"
#include "Date.h"
#include "util/cpp/String.h"
#include <boost/lexical_cast.hpp>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&XceAlertI::instance(),
			service.createIdentity("XA", ""));
	
	//	--------------------------------HA--------------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("XceAlert.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("XceAlert.Interval",5);
	::xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerXceAlert",&XceAlertI::instance(),mod,interval);
}

//---------------------------------------------------------------------------
XceAlertI::XceAlertI() {
	
	ServiceI& service = ServiceI::instance();

	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	_fileName = props->getPropertyWithDefault("Service.Config",
			"/data/xce/XiaoNei/etc/templates/XceAlert.config");

	interval_ = 900;
	
	reloadAlertMap();
}

XceAlertI::~XceAlertI() {
}


void XceAlertI::reloadAlertMap(const Ice::Current& current)
{
  InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	Ice::PropertiesPtr prop = Ice::createProperties();
	prop->load(_fileName);

	interval_ = prop->getPropertyAsIntWithDefault("Interval", 900);
	map<string, set<string> > mobileAlert;
	map<string, set<string> > emailAlert;
	map<string, set<string> > subjectGroups;
	set<string> defaultEmail;
	set<string> defaultMobile;

	stringstream mobilestream(prop->getProperty("DefaultMobile")); 
	string mobile;
	while (mobilestream >> mobile) {
		defaultMobile.insert(mobile);
		MCE_INFO("XceAlertI::reloadAlertMap add defaultmobile : " << mobile);
	}
	
	stringstream emailstream(prop->getProperty("DefaultEmail")); 
	string email;
	while (emailstream >> email) {
		defaultEmail.insert(email);
		MCE_INFO("XceAlertI::reloadAlertMap add defaultemail : " << email);
	}

	Str2StrMap subjectGroupMap = prop->getPropertiesForPrefix("SubjectGroup");
	for(Str2StrMap::const_iterator it = subjectGroupMap.begin(); it != subjectGroupMap.end(); ++it) {
		string subjectGroup = it->first.substr(13);
		stringstream stream(it->second);
		string subject;
		while(stream >> subject) {
			subjectGroups[subjectGroup].insert(subject);
			MCE_INFO("XceAlertI::reloadAlertMap SubjectGroup : " << subjectGroup << " addSubject :" << subject);
		}
	}

	string groupTag = "SGroup", mobileType = "Mobile", emailType = "Email";
	generateInterestMap(mobileType, groupTag, mobileAlert, subjectGroups, prop);
	generateInterestMap(emailType, groupTag, emailAlert, subjectGroups, prop);

	{
		IceUtil::Mutex::Lock lock(_mutex); 
		defaultEmail_ = defaultEmail;
		defaultMobile_ = defaultMobile;
		mobileAlert_ = mobileAlert;
		emailAlert_ = emailAlert;
	}
	
	MCE_INFO("XceAlertI::reloadAlertMap end");
}

void XceAlertI::generateInterestMap(const string& type, const string& groupTag, map<string, set<string> >& resultMap, 
									const map<string, set<string> >& subjectGroupMap, const Ice::PropertiesPtr prop) {
	Str2StrMap contentMap = prop->getPropertiesForPrefix(type);
	for(Str2StrMap::const_iterator it = contentMap.begin(); it != contentMap.end(); ++it) {
		string typeName = it->first.substr(type.length() + 1);
		stringstream stream(it->second);
		string subject;
		while (stream >> subject) {
			if(subject.substr(0, groupTag.length()) == groupTag) {
				string subjectGroup = subject.substr(groupTag.length() + 1);
				map<string, set<string> >::const_iterator itm = subjectGroupMap.find(subjectGroup);
				if(itm != subjectGroupMap.end()) {
					for(set<string>::const_iterator its = itm->second.begin(); its != itm->second.end(); ++its) {
						resultMap[*its].insert(typeName);
						MCE_INFO("XceAlertI::reloadAlertMap subject : " << *its << " in group " << subjectGroup << " add" << type << " : " << typeName);
					}
				}	
			}
			else {
				resultMap[subject].insert(typeName);	
				MCE_INFO("XceAlertI::reloadAlertMap subject : " << subject << " add " << type << " : " << typeName);
			}
		}
	}
}

void XceAlertI::notifyAlert(const MyUtil::StrSeq& typeSeq, const string& msg, const Ice::Current& current)
{
  InvokeClient::create(current, msg, InvokeClient::INVOKE_LEVEL_INFO);
	for(MyUtil::StrSeq::const_iterator it = typeSeq.begin(); it != typeSeq.end(); ++it)
	{
		map<string, int>::iterator fi = filter_.find(*it);
		if (fi == filter_.end()) {
			filter_.insert(make_pair(*it, time(0)));
		} else {
			int tim = time(0);
			if (tim - filter_[*it] < interval_) {
				MCE_WARN("XceAlertI::notifyAlert send too often subject = " << *it << " msg = " << msg);
				continue;
			}
			filter_[*it] = tim;
		}
		
		stringstream warnMsg;	
		try {
			warnMsg << "Name: " << (*it) << " " << " MSG: " << msg;
			IceUtil::Mutex::Lock lock(_mutex); 
				
			map<string, set<string> >::iterator findIt = mobileAlert_.find((*it));
		
			if ( findIt == mobileAlert_.end()) {
				sendMsg(*it, warnMsg.str(), defaultMobile_);
			} else {
				sendMsg(*it, warnMsg.str(), findIt->second);
			}
	
		} catch(...) {
			MCE_WARN("XceAlertI::notifyAlert Unknow Exception! Message : " << warnMsg);
		}
	}	
	MCE_WARN("XceAlertI::notifyAlert end");
}

void XceAlertI::notifyAlertN(const MyUtil::StrSeq& typeSeq, const string& msg, int sendType, const Ice::Current& current) {
  InvokeClient::create(current, msg, InvokeClient::INVOKE_LEVEL_INFO);
	if(sendType != SENDMSG && sendType != SENDEMAIL && sendType != SENDALL) {
		MCE_WARN("XceAlertI::notifyAlertN sendType error sendType : " << sendType);
		return;
	}	

	for(MyUtil::StrSeq::const_iterator it = typeSeq.begin(); it != typeSeq.end(); ++it)
	{
		map<string, int>::iterator fi = filter_.find(*it);
		if (fi == filter_.end()) {
			filter_.insert(make_pair(*it, time(0)));
		} else {
			int tim = time(0);
			if (tim - filter_[*it] < interval_) {
				MCE_WARN("XceAlertI::notifyAlertN send too often subject = " << *it << " msg = " << msg);
				continue;
			}
			filter_[*it] = tim;
		}
		
		stringstream warnMsg;	
		try {
			warnMsg << msg;
			if(sendType & SENDMSG) {
				IceUtil::Mutex::Lock lock(_mutex); 
				map<string, set<string> >::iterator findIt = mobileAlert_.find((*it));	
				if ( findIt == mobileAlert_.end()) {
					sendMsg(*it, warnMsg.str(), defaultMobile_);
				} else {
					sendMsg(*it, warnMsg.str(), findIt->second);
				}
			}
			if(sendType & SENDEMAIL) {
				IceUtil::Mutex::Lock lock(_mutex); 
				map<string, set<string> >::iterator findIt = emailAlert_.find((*it));
				if (findIt == emailAlert_.end()) {
					sendEmail(*it, warnMsg.str(), defaultEmail_);
				} else {
					sendEmail(*it, warnMsg.str(), findIt->second);
				}
			}	
		} catch(...) {
			MCE_WARN("XceAlertI::notifyAlertN Unknow Exception! Message : " << warnMsg);
		}
	}
	
	MCE_WARN("XceAlertI::notifyAlertN end");
}

void XceAlertI::sendEmail(const string& subject, const string& msg, const set<string>& emails) {
	MCE_INFO("XceAlertI::sendEmail msg " << msg << " send email " << emails.size() << " times");
	for(set<string>::const_iterator it = emails.begin(); it != emails.end(); ++it) {
		sendEmail(subject, msg, *it);
	}
}

void XceAlertI::sendMsg(const string& subject, const string& msg, const set<string>& mobiles) {
	MCE_INFO("XceAlertI::sendMsg msg " << msg << " send mobile " << mobiles.size() << " times");
	for(set<string>::const_iterator it = mobiles.begin(); it != mobiles.end(); ++it) {
		sendMsg(subject, msg, *it);
	}
}

void XceAlertI::sendEmail(const string& subject, const string& msg, const string& email) {	
	ostringstream cmd;
	cmd << "cd ../bin; sh sendmail.sh \"" << subject << "\" " << email << " \"" << msg << "\"";
	system(cmd.str().c_str());
	MCE_INFO("XceAlertI::sendEmail subject = " << subject << " msg = " << msg << " email = " << email);
}

void XceAlertI::sendMsg(const string& subject, const string& msg, const string& mobile) {
	ostringstream cmd;
	cmd << "wget -q -O /dev/null \"http://10.22.198.81:2000/receiver?number=" << mobile << "&message=" << msg << "\"";
	system(cmd.str().c_str());
	MCE_INFO("XceAlertI::sendMsg subject = " << subject << " msg = " << msg << " mobile = " << mobile);
}
