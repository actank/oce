#ifndef __LoginCacheI_h__
#define __LoginCacheI_h__

#include <Login.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/IceUtil.h>
#include "DbCxxPool/src/QueryRunner.h"
#include "UtilCxx/src/Singleton.h"
#include <queue>
#include "UtilCxx/src/hash_map_string.h"
namespace passport {

static const std::string MANAGER = "M";
static const int BUCKET_SIZE = 1000;

class LoginMap: virtual public IceUtil::Shared {
public:
	void set(int userId, const std::string& ticket,int second);
	std::string get(int userId);
	int get(const std::string& ticket);
	bool del(int userId);
	bool del(const std::string& ticket);
private:
	__gnu_cxx::hash_map<int, pair<std::string,int> > id2ticket_;
	__gnu_cxx::hash_map<std::string, pair<int,int> > ticket2id_;
	IceUtil::Mutex mutex_;
};
typedef IceUtil::Handle<LoginMap> LoginMapPtr;

class LoginHolder: virtual public MyUtil::Singleton<LoginHolder> {
public:
	void initialize();
	void set(int userId, const std::string& ticket,int second);
	std::string get(int userId);
	int get(const std::string& ticket);
	bool del(int userId);
	bool del(const std::string& ticket);
private:
	std::vector<LoginMapPtr> all_tickets_;
	LoginMapPtr& findLoginMap(const std::string& ticket);
	LoginMapPtr& findLoginMap(int userId);
};

class LoginCacheI: virtual public LoginCache {
public:
	LoginCacheI(Ice::Int, std::string, std::string);
	void initialize();
	virtual void set(Ice::Int uid, const string& ticket, int second,
			const Ice::Current&);
	virtual bool destroyByTicket(const string& ticket, const Ice::Current& =
			Ice::Current());
	virtual bool destroyById(Ice::Int, const Ice::Current& = Ice::Current());

	virtual ::Ice::Int ticket2Id(const string& ticket, const Ice::Current& =
			Ice::Current());

	virtual string id2Ticket(::Ice::Int, const Ice::Current& = Ice::Current());
	
	virtual bool getStatus(Ice::Int,const Ice::Current& = Ice::Current());

	virtual void setStatus(Ice::Int,const Ice::Current& = Ice::Current());
private:
	Ice::Int loadPersistent(const string&);
	string loadPersistent(int);
	void savePersistent(const string&, int);
	void removePersistent(const string&);
	void removePersistent(int userId);

	IceUtil::Mutex _mutex;
	void saveCross(int userId);
	bool loadCross(int userId);
	std::set<int> userStatus;
	int duration_;
	std::string suffix_;

	std::string DB_DESC_PASSPORT;
	std::string TABLENAME;
};
typedef IceUtil::Handle<LoginCacheI> LoginCacheIPtr;

class LoadPersistentByTicketHandler: virtual public com::xiaonei::xce::ResultHandler {
	public:
		LoadPersistentByTicketHandler(int& id) :
			_id(id) {
				;   
			}   
	protected:
		virtual bool handle(mysqlpp::Row& res) const {
			_id = (int) res["userid"];
			return true;
		}   
	private:
		int& _id;
};

class LoadPersistentByIdHandler: virtual public com::xiaonei::xce::ResultHandler {
	public:
		LoadPersistentByIdHandler(std::string& ticket) :
			_ticket(ticket) {
				;   
			}   
	protected:
		virtual bool handle(mysqlpp::Row& res) const {
			_ticket = res["ticket"].c_str();
			return true;
		}   
	private:
		string& _ticket;
};

class LoadCrossHandler: virtual public com::xiaonei::xce::ResultHandler {
	public:
		LoadCrossHandler() {
			;   
		}   
	protected:
		virtual bool handle(mysqlpp::Row& res) const {
			return true;
		}   
	private:
};

};
#endif


