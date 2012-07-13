#ifndef __APNSMANAGERI_H__
#define __APNSMANAGERI_H__

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

#include <string>
#include <string.h>
#include <IceUtil/Shared.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "Singleton.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include "ConnectionQuery.h"
#include "Date.h"

#include "APNSNotify.h"
#include "APNSPusher.h"

namespace xce {
namespace notify {
namespace apns {
using boost::multi_index_container;
using namespace boost::multi_index;
using namespace std;
using namespace MyUtil;
using namespace xce::notify;
using namespace com::xiaonei::xce;

struct Token {
	int userId;
	time_t timestamp;
	string token;
	Token(int uid, const string& t) {
		userId = uid;
		token = t;
		timestamp = time(NULL);
	}
};

typedef multi_index_container<Token, indexed_by<sequenced<> , hashed_unique<
		member<Token, int, &Token::userId> > , hashed_unique<member<Token,
		std::string, &Token::token> > > > TokenContainer;

typedef TokenContainer::nth_index<0>::type SequencedIndex;
typedef TokenContainer::nth_index<1>::type UserIdIndex;
typedef TokenContainer::nth_index<2>::type TokenIndex;
typedef APNSPusher* PusherPtr;

class UserPool: public IceUtil::Shared {
public:
	UserPool();
	void kick();
	bool getUserIfBlock(int userId);
	void setUserToBlock(int userId);
	void removeUserFromBlock(int userId);
	MyUtil::Int2StrMap locateTokenByUserIdSeq(const MyUtil::IntSeq& userIds);
	void addUser(int userId, const string& token);
	void removeUserByToken(const string& token);
private:
	IceUtil::RWRecMutex _mutex;
	TokenContainer _users;
	bool* _blockSet;
};

const string DB_INSTANCE = "notify";
const string DB_TABLE = "apns_uid_token";
const unsigned int USER_SIZE = 2000000;
const unsigned int PUSHER_SIZE = 10;
const int BLOCK_SIZE = 0x7fffffff / 8 + 1;

class APNSManagerI: public APNSManager, public MyUtil::Singleton<APNSManagerI> {
public:
	PusherPtr getPusher(int userId);
	void kick();
	void init();
	void setPemFile(const string& pf);
	void initPusher();
	virtual void addUser(Ice::Int userId, const string& token,
			const Ice::Current& = Ice::Current());
	virtual void addIphoneNotifyBySeq(const MyUtil::IntSeq& userIds,
			const string& msg, const Ice::Current& = Ice::Current());
	virtual void removeUserByToken(const string& token, const Ice::Current& =
			Ice::Current());
private:
	UserPool _pool;
	vector<PusherPtr> _pusher;
	string _pemFile;
};

class KickTask: public Timer {
public:
	KickTask() :
		Timer(10 * 60 * 1000) {
		;
	}
	virtual void handle();
};

class InitPusherTask: public Task {
public:
	InitPusherTask() {
	}
	virtual void handle();
};

}
;
}
;
}
;

#endif 
