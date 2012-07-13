#ifndef __XCE_ALERTI_h__
#define __XCE_ALERTI_h__

#include "XceAlert.h"
#include "ServiceI.h"
#include "Parser.h"
#include "Singleton.h"

namespace com {
namespace xiaonei {
namespace xce {
using namespace std;
using namespace MyUtil;

class XceAlertI: public XceAlert, public Singleton<XceAlertI>
{
public:
	
	virtual void reloadAlertMap(const Ice::Current& = Ice::Current());

	virtual void notifyAlert(const MyUtil::StrSeq& typeSeq, const string& msg, const Ice::Current& = Ice::Current());
	
	virtual void notifyAlertN(const MyUtil::StrSeq& typeSeq, const string& msg, int sendType, const Ice::Current& = Ice::Current());

	XceAlertI();

	~XceAlertI();

private:

	void generateInterestMap(const string& type, const string& groupTag, map<string, set<string> >& resultMap, 
							 const map<string, set<string> >& subjectGroupMap, const Ice::PropertiesPtr prop); 
	
	void sendEmail(const string& subject, const string& msg, const set<string>& emails);
 
	void sendMsg(const string& subject, const string& msg, const set<string>& phones);

	void sendEmail(const string& subject, const string& msg, const string& email);
	
	void sendMsg(const string& subject, const string& msg, const string& phone);

	string _fileName;

	IceUtil::Mutex _mutex;

	map<string, set<string> > mobileAlert_;

	map<string, set<string> > emailAlert_;
	
	set<string> defaultEmail_;

	set<string> defaultMobile_;	

	map<string, int> filter_;

	int interval_;
};

}
;
}
;
}
;

#endif
