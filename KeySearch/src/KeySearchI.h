#ifndef __KEY_SEARCH_I_H__
#define __KEY_SEARCH_I_H__

#include <IceUtil/RWRecMutex.h>
#include <map>
#include "ServiceI.h"
#include "KeySearch.h"
#include "SearchStat.h"
#include "TaskManager.h"

namespace demo{
	using namespace std;
	class KeySearchManagerI: public KeySearchManager,virtual public MyUtil::Singleton<KeySearchManagerI>
	{
	public:
		KeySearchManagerI();
		virtual ~KeySearchManagerI();
		SearchStatPtr getTimer(){return _scheduleTimer;}
        	Ice::Int size(const Ice::Current& = Ice::Current());
        	Ice::Int lookup(const string& Key,const Ice::Current& = Ice::Current());
		bool insert(const string& Key, Ice::Int Value,const Ice::Current& = Ice::Current());
		bool remove(const string& Key,const Ice::Current& = Ice::Current());
	private:	
		SearchStatPtr _scheduleTimer;
		map<string,Ice::Int> _dataStorage;
		IceUtil::RWRecMutex	_dataMutex;  
	};


} 
#endif
