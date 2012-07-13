#ifndef __TEMP_MUTEX_H__
#define __TEMP_MUTEX_H__

#include <ext/hash_set>
#include <ServiceI.h>

using namespace std;
using namespace __gnu_cxx;

namespace xce
{
namespace tempmutex
{
	class TempMutex : public IceUtil::Mutex
	{
		public:

			TempMutex() : _refCount(0)
			{
			}
		
		public:

			void inc()
			{
				IceUtil::Mutex::Lock lock(_mutex);
				++_refCount;
			}

			int dec()
			{
				IceUtil::Mutex::Lock lock(_mutex);
				return --_refCount;
			}
		
		private:

			IceUtil::Mutex _mutex;
			int _refCount;
	}; 
	
	typedef TempMutex* TempMutexPtr;

	class TempMutexHolder : public Ice::Object
	{
		public:

			TempMutexPtr getMutex(long key)
			{
				IceUtil::Mutex::Lock lock(_mutexMapMutex);
        TempMutexPtr tMutex = new TempMutex;
				pair<hash_map<long, TempMutexPtr>::iterator, bool> rs = _mutexMap.insert(make_pair(key, tMutex));
        if(!rs.second)
        {
            if(tMutex)
            {
                delete tMutex;
            }
        }
				rs.first->second->inc();
				return rs.first->second;
			}
			
			void releaseMutex(long key)
			{
				IceUtil::Mutex::Lock lock(_mutexMapMutex);
				hash_map<long, TempMutexPtr>::iterator rs = _mutexMap.find(key);
				if(rs != _mutexMap.end())
				{
					if(rs->second->dec() == 0)
					{
						TempMutexPtr dPtr = rs->second;
						_mutexMap.erase(rs);
						delete dPtr;
					}
				}
			}
		
		private: 
		
			IceUtil::Mutex _mutexMapMutex;
			
			hash_map<long, TempMutexPtr> _mutexMap;
	};
	
	typedef IceUtil::Handle<TempMutexHolder> TempMutexHolderPtr;

	class TempMutexManager : public Ice::Object
	{
		public:

			TempMutexManager(const int holderSize = 10) : _holderSize(holderSize)
			{
				for(int i = 0; i < _holderSize; ++i)
				{
					_holderVec.push_back(new TempMutexHolder);
				}
			}

		public:

			TempMutexPtr getMutex(long key)
			{
				return _holderVec.at(key % _holderSize)->getMutex(key);
			}
			
			void releaseMutex(long key)
			{
				_holderVec.at(key % _holderSize)->releaseMutex(key);
			}


		private:

			int _holderSize;
			vector<TempMutexHolderPtr> _holderVec;
	};

	typedef IceUtil::Handle<TempMutexManager> TempMutexManagerPtr;
	
	class Lock
	{
		public:

			Lock(const TempMutexManagerPtr& manager, long key) : _manager(manager), _key(key),_lock(*(_manager->getMutex(_key)))
			{
			}
			
			~Lock()
			{
				_lock.release();
				_manager->releaseMutex(_key);
			}

		private:
			
			TempMutexManagerPtr _manager;
			long _key;
			IceUtil::Mutex::Lock _lock;
	};

}
}

#endif
