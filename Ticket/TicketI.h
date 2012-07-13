#ifndef __TicketManagerI_h__
#define __TicketManagerI_h__

#include <Ticket.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/IceUtil.h>

#include "UtilCxx/src/Singleton.h"
#include <queue>
#include <ext/hash_map>
#include "hash_map_string.h"
namespace passport {

static const std::string MANAGER = "M";
static const int BUCKET_SIZE = 1000;

class TicketMap: virtual public IceUtil::Shared {
public:
	void set(int userId, const std::string& ticket);
	std::string get(int userId);
	int get(const std::string& ticket);
	bool del(int userId);
	bool del(const std::string& ticket);
private:
	__gnu_cxx::hash_map<int, std::string> id2ticket_;
	__gnu_cxx::hash_map<std::string, int> ticket2id_;
	IceUtil::Mutex mutex_;
};
typedef IceUtil::Handle<TicketMap> TicketMapPtr;

class TicketHolder: virtual public MyUtil::Singleton<TicketHolder> {
public:
	void initialize();
	void set(int userId, const std::string& ticket);
	std::string get(int userId);
	int get(const std::string& ticket);
	bool del(int userId);
	bool del(const std::string& ticket);
private:
	std::vector<TicketMapPtr> all_tickets_;
	TicketMapPtr& findTicketMap(const std::string& ticket);
	TicketMapPtr& findTicketMap(int userId);
};

class TicketManagerI: virtual public TicketManager {
public:
	TicketManagerI(Ice::Int, std::string);
	void initialize();
	virtual std::string createTicket(Ice::Int, const std::string&, const Ice::Current& = Ice::Current());
	virtual bool destroyTicket(const std::string&, const ::Ice::Current& = Ice::Current());
	virtual Ice::Int verifyTicket(const std::string& ticket, const std::string&, const Ice::Current& = Ice::Current());
	virtual std::string queryTicket(Ice::Int, const Ice::Current& = Ice::Current());
private:
	int duration_;
	std::string suffix_;
	std::string generateTicket(Ice::Int, const std::string&);
};
typedef IceUtil::Handle<TicketManagerI> TicketManagerIPtr;

/* Recycle */

struct ToBeRecycled {
	ToBeRecycled(int new_id, int new_time) :
		id(new_id), time(new_time) {
	}
	int id;
	int time;
	bool operator<(const ToBeRecycled& rhs) const{
		return rhs.time > time;
	}
};

class TicketRecycle: virtual public IceUtil::Shared {
public:
	TicketRecycle(int duration) :
		duration_(duration) {
	}
	void add(int userId);
	std::vector<int> get();
private:
	int duration_;
	IceUtil::Mutex mutex_;
	__gnu_cxx::hash_map<int,int> id2Time;
};
typedef IceUtil::Handle<TicketRecycle> TicketRecyclePtr;

class TicketCleaner: public MyUtil::Singleton<TicketCleaner> {
public:
	void initialize(int duration);
	void recycle(int userId);
private:
	int duration_;
	IceUtil::Monitor<IceUtil::Mutex> unscheduled_mutex_;
	void swap(std::vector<int>& empty_vector);
	void add(int userId);
	std::vector<int> unscheduled_;
	std::vector<TicketRecyclePtr> recycles_;

	class InputThread: virtual public IceUtil::Thread {
	public:
		InputThread(TicketCleaner& cleaner) :
			cleaner_(cleaner) {
		}
		virtual void run();
	private:
		TicketCleaner& cleaner_;
		std::vector<int> to_be_input_;
	};
	typedef IceUtil::Handle<InputThread> InputThreadPtr;
	class OutputThread: virtual public IceUtil::Thread {
	public:
		OutputThread(TicketCleaner& cleaner) :
			cleaner_(cleaner) {
		}
		virtual void run();
	private:
		TicketCleaner& cleaner_;
		std::vector<int> to_be_output_;
	};
	typedef IceUtil::Handle<OutputThread> OutputThreadPtr;

	InputThreadPtr input_;
	OutputThreadPtr output_;
};

}
#endif
