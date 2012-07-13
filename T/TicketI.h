#ifndef __TicketManagerI_h__
#define __TicketManagerI_h__

/*
 * =====================================================================================
 *
 *       Filename:  Ticket.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2009年09月03日 14时44分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tanbokan (), bokan.tan@opi-corp.com
 *        Company:  opi-corp.com
 *
 * =====================================================================================
 */
#include <Ticket.h>
#include "Evictor.h"
#include "IceUtil/Mutex.h"
#include "IceUtil/IceUtil.h"
namespace passport {
using std::string;
using std::map;
using std::vector;
const std::string MANAGER = "M";

class TicketMap {
	public :
		void set(int userId, const string& ticket) {
		}
		string getTicket(int userId) {
			map<int,string>::iterator it;
			if((it=id2ticket.find(userId)!=id2ticket.end())){
				return it->second;
			}else
			  return string();

		}
		int getId(const string& ticket) {
			map<string,int>::iterator it;
			if((it = ticket2id.find(ticket)!=ticket2id.end())){
				return it->second;
			}else
			  return -1;
		}
		bool delId(int userId);
		//bool delTicket(const string& ticket);
	private:
		map< int, string > id2ticket;
		map< string, int > ticket2id;
		IceUtil::Mutex _mutex;
};

typedef IceUtil::Handle<TicketMap> TicketMapPtr;

class TicketClearer : public MyUtil::Singleton<TicketCleaner>{
	public :
		void post(int userId);
	private :
		vector<LastUsed> lastUseds;
		vector<PutterPtr>  putter;
		vector<ClearerPtr>  clearers;
}
class LastUsed : public IceUtil::Shared {
	public :
		vector<int> userIds;
		void swap(vector<int>& _userIds){
			IceUtil::Mutex::Lock lock(_mutex);
			userIds.swap(_userIds);
			userIds.clear();
		}
		void put(int userId){
			IceUtil::Mutex::Lock lock(_mutex);
			userIds.push_back(userId);
		}
	private:
		IceUtil::Mutex _mutex;

}; 
typedef IceUtil::Handle<LastUsed> LastUsedPtr;

class Putter : public IceUtil::Thread {
	public :
		Putter(LastUsedPtr& _lup)
			:lup(_lup){
			}
		LastUsedPtr& lup;
		ClearPtr& cp;
		vector<int> userIds;
		virtual run();
}
class Clearer : public IceUtil::Thread {
	public :
		virtual run();
		void put(int userId){
			map<int,deque<pair<int,int> >::iterator >::iterator it; 
			IceUtil::Mutex::Lock lock(_mutex);	
			if((it=idMap.find(id))!=idMap.end()){
				idTime.erase(it->second);
				idTime.push_front(make_pair(id,time(NULL)));
				it->second = idTime.begin();
			}else{
				idTime.push_front(make_pair(id,time(NULL)));
				idMap[id]=idTime.begin();
			}   

		}
	private:
		map<int,deque<pair<int,int> >::iterator> idMap;
		deque<pair<int,int> > idTime; 
		IceUtil::Mutex _mutex;
}
class TicketManagerI: public TicketManager {
public:
	TicketManagerI(Ice::Int, std::string);

	virtual std::string createTicket(Ice::Int, const std::string&,
			const Ice::Current& = Ice::Current());
	virtual bool destroyTicket(const std::string&, const ::Ice::Current& =
			Ice::Current());
	virtual Ice::Int verifyTicket(const std::string& ticket,
			const std::string&, const Ice::Current& = Ice::Current());
	virtual std::string queryTicket(Ice::Int, const Ice::Current& =
			Ice::Current());
private:
	//std::string generateTicket(Ice::Int,const Str2StrMap&);
	TicketMapPtr& findTicketMap(const string& ticket);
	TicketMapPtr& findTicketMap(int userId);

	int _duration;
	std::string _suffix;
    
	vector<TicketMapPtr> _ticketMaps;
	static const size_t SEGMENT_SIZE = 1000;
};
typedef IceUtil::Handle<TicketManagerI> TicketManagerIPtr;
}
#endif
