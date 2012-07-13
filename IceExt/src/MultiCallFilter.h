#ifndef __MULTI_CALL_FILTER_H__
#define __MULTI_CALL_FILTER_H__

#include <ext/hash_set>
#include <ServiceI.h>

using namespace std;
using namespace __gnu_cxx;

namespace xce
{
namespace multicallfilter
{
class Filter : public Ice::Object
{
	public:

		bool filter(long key)
		{
			IceUtil::Mutex::Lock lock(_setMutex);
			return _filterSet.insert(key).second;
		}

		vector<long> filter(vector<long> keys, vector<long>& finalIds)
		{
			vector<long> filterIds;
			{
				IceUtil::Mutex::Lock lock(_setMutex);
				for(vector<long>::const_iterator it = keys.begin(); it != keys.end(); ++it)
				{
					if(!_filterSet.insert(*it).second)
					{
						filterIds.push_back(*it);
					}else
					{
						finalIds.push_back(*it);
					}
				}
			}
			return filterIds;
		}
		
		vector<int> filter(vector<int> keys, vector<int>& finalIds)
		{
			vector<int> filterIds;
			{
				IceUtil::Mutex::Lock lock(_setMutex);
				for(vector<int>::const_iterator it = keys.begin(); it != keys.end(); ++it)
				{
					if(!_filterSet.insert(*it).second)
					{
						filterIds.push_back(*it);
					}else
					{
						finalIds.push_back(*it);
					}
				}
			}
			return filterIds;
		}

		void remove(long key)
		{
			IceUtil::Mutex::Lock lock(_setMutex);
			_filterSet.erase(key);
		}
		
		void remove(vector<long> keys)
		{
			IceUtil::Mutex::Lock lock(_setMutex);
			for(vector<long>::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				_filterSet.erase(*it);
			}
		}
	
	private:

		hash_set<long> _filterSet;
		IceUtil::Mutex _setMutex;

};

typedef IceUtil::Handle<Filter> FilterPtr;

class MultiCallFilter : public Ice::Object
{
	public:

		MultiCallFilter(const int filterSize)
		{
			_filterSize = filterSize;

			for(int i = 0; i < filterSize; ++i)
			{
				_filterVec.push_back(new Filter);
			}
		}

	public:

		bool filter(long key)
		{
			return _filterVec.at(key % _filterSize)->filter(key);
		}
		
		vector<long> filter(vector<long> keys, vector<long>& finalIds)
		{
			vector<long> filterIds;
			vector<long> distIds[_filterSize];
			for(vector<long>::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				distIds[*it % _filterSize].push_back(*it);
			}
			
			for(int i = 0; i< _filterSize; ++i)
			{
				vector<long> tempIds = _filterVec.at(i)->filter(distIds[i], finalIds);
				if(tempIds.size() > 0)
				{
					filterIds.insert(filterIds.end(),tempIds.begin(), tempIds.end());
				}
			}

			return filterIds;
		}

		vector<int> filter(vector<int> keys, vector<int>& finalIds)
		{
			vector<int> filterIds;
			vector<int> distIds[_filterSize];
			for(vector<int>::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				distIds[*it % _filterSize].push_back(*it);
			}
			
			for(int i = 0; i< _filterSize; ++i)
			{
				vector<int> tempIds = _filterVec.at(i)->filter(distIds[i], finalIds);
				if(tempIds.size() > 0)
				{
					filterIds.insert(filterIds.end(),tempIds.begin(), tempIds.end());
				}
			}

			return filterIds;
		}

		void remove(long key)
		{
			return _filterVec.at(key % _filterSize)->remove(key);
		}
		
		void remove(vector<long> keys)
		{
			vector<long> distIds[_filterSize];
			for(vector<long>::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				distIds[*it % _filterSize].push_back(*it);
			}
			
			for(int i = 0; i< _filterSize; ++i)
			{
				_filterVec.at(i)->remove(distIds[i]);
			}
		}
		
		void remove(vector<int> keys)
		{
			vector<long> distIds[_filterSize];
			for(vector<int>::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				distIds[*it % _filterSize].push_back(*it);
			}
			
			for(int i = 0; i< _filterSize; ++i)
			{
				_filterVec.at(i)->remove(distIds[i]);
			}
		}

	private:

		int _filterSize;
		vector<FilterPtr> _filterVec;
};

typedef IceUtil::Handle<MultiCallFilter> MultiCallFilterPtr;

}
}
#endif
