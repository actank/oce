#ifndef __TEMP_MUTEXT_H__
#define __TEMP_MUTEXT_H__

#include <ext/hash_set>
#include <ServiceI.h>

using namespace std;
using namespace __gnu_cxx;

namespace xce
{
namespace tempmutext
{
	class TempMutexT : public IceUtil::Mutex
	{
		public:

			TempMutexT() : _refCount(0)
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
	
	typedef TempMutexT* TempMutexTPtr;

  template<typename T>
	class TempMutexHolderT : public Ice::Object
	{
		public:

			TempMutexTPtr getMutex(T key)
			{
				IceUtil::Mutex::Lock lock(_mutexMapMutex);
        TempMutexTPtr tMutex = new TempMutexT;
				pair<typename hash_map<T, TempMutexTPtr>::iterator, bool> rs = _mutexMap.insert(make_pair(key, tMutex));
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
			
			void releaseMutex(T key)
			{
				IceUtil::Mutex::Lock lock(_mutexMapMutex);
				typename hash_map<T, TempMutexTPtr>::iterator rs = _mutexMap.find(key);
				if(rs != _mutexMap.end())
				{
					if(rs->second->dec() == 0)
					{
						TempMutexTPtr dPtr = rs->second;
						_mutexMap.erase(rs);
						delete dPtr;
					}
				}
			}
		
		private: 
		
			IceUtil::Mutex _mutexMapMutex;
			
			hash_map<T, TempMutexTPtr> _mutexMap;
	};

  template<typename T>
  class TempMutexManagerT : public Ice::Object
	{
		public:

			TempMutexManagerT(const int holderSize = 10) : _holderSize(holderSize)
			{
				for(int i = 0; i < _holderSize; ++i)
				{
					_holderVec.push_back(new TempMutexHolderT<T>);
				}
			}

		public:

			TempMutexTPtr getMutex(T key)
			{
          return _holderVec.at(getLong(key) % _holderSize)->getMutex(key);
			}
			
			void releaseMutex(T key)
			{
				_holderVec.at(getLong(key ) % _holderSize)->releaseMutex(key);
			}

    private:

      long getLong(T key) {
          return atol(key.c_str());
      }

		private:

			int _holderSize;
			vector<TempMutexHolderT<T>*> _holderVec;
	};


  template<typename T>
	class Locks
	{
		public:

			Locks(TempMutexManagerT<T>* manager, const std::vector<T> keys, std::vector<T>& lockedKeys, std::vector<T>& failedKeys) : _manager(manager), _keys(keys)
			{
          for(typename std::vector<T>::const_iterator it = _keys.begin(); it != _keys.end(); ++it) {
              IceUtil::Mutex::TryLock* lock = new IceUtil::Mutex::TryLock(*(_manager->getMutex(*it)));
              if(lock->acquired()) {
                  _locksMap.insert(make_pair(*it, lock));
                  lockedKeys.push_back(*it);
              }else{
                  if(lock) {
                      delete lock;
                  }
                  failedKeys.push_back(*it);
              }
          }
			}
			
			~Locks()
			{
          for(typename std::map<T, IceUtil::Mutex::TryLock*>::const_iterator it = _locksMap.begin(); it != _locksMap.end(); ++it) {
              it->second->release();
              _manager->releaseMutex(it->first);
              if(it->second) {
                  delete it->second;
              }
          }
			}

		private:
			
			TempMutexManagerT<T>* _manager;
      std::vector<T> _keys;
      std::map<T, IceUtil::Mutex::TryLock*> _locksMap;
	};

}
}

#endif
