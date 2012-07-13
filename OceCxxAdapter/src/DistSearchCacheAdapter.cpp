/*
 * =====================================================================================
 *
 *       Filename:  DistSearchCacheAdapter.cpp
 *
 *    Description:  Adapter for communicating with DistCache
 *
 *        Version:  1.0
 *        Created:  2010年12月04日 17时15分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "DistSearchCacheAdapter.h"
#include "ServiceI.h"
namespace xce
{
   namespace dist
    {
      namespace searchcacheadapter
      {
        using namespace xce::searchcache;
        using namespace com::xiaonei::xce;
        using namespace MyUtil;
        using namespace std;
        using namespace IceUtil;
        using namespace xce::distcache;
        void DistSearchCacheAdapter::initialize()  {
          while(true)  {
            distCacheAdapterPtr = xce::distcache::DistCacheAdapter::connect("DistSearchCache");
            if(distCacheAdapterPtr.get() != NULL)  {
              break;
            }  else  {
               MCE_WARN("DistSearchCacheAdapter::initialize can not init DistCacheAdapter!");
            }
          }
          last_oper_time_.resize(GROUP_SIZE);
          time_to_userid_.resize(GROUP_SIZE);
        }
     
        SearchCacheMemDataPtr DistSearchCacheAdapter::getData(long id) {
          DataPtr dataPair = distCacheAdapterPtr->get(id);
          if(dataPair.get() != NULL)  {
            if(dataPair->size() <= 0) {
              MCE_WARN("DistSearchCacheAdapter::getDataWithCAS not in memcached userid = " << id);
              SearchCacheMemDataPtr tmp = new SearchCacheMemData;
              return tmp;
            }
            string dataStr(dataPair->begin(), dataPair->end());
            SearchCacheMemDataPtr data = new SearchCacheMemData;
            data->ParseFromString(dataStr);
            return data;
          }  else  {
            SearchCacheMemDataPtr tmp = new SearchCacheMemData;
            return tmp;
          }
        }

        map<long, SearchCacheMemDataPtr> DistSearchCacheAdapter::getData(vector<long> ids) {
          map<long, SearchCacheMemDataPtr> result;
          map<long, DataPtr> propMap = distCacheAdapterPtr->get(ids);
          for(map<long, DataPtr>::const_iterator it = propMap.begin(); it != propMap.end(); ++it) {
            SearchCacheMemDataPtr data = new SearchCacheMemData;
            if(NULL != it->second.get())  { 
              string dataStr(it->second->begin(), it->second->end());
              data->ParseFromString(dataStr);
            } 
            result.insert(make_pair(it->first,data));
          }
          return result;
        }

        bool DistSearchCacheAdapter::setData(long userId, const SearchCacheMemDataPtr& res) {
          if(NULL == res.get())  {
            MCE_WARN("Data to set is empty!");
            return false;
          }
          string dataStr;
          res->SerializeToString(&dataStr);
          DataPtr tmp(new Data( dataStr.begin(), dataStr.end() ) );
          bool setOk = distCacheAdapterPtr->set(userId, tmp);
          return setOk;
        }

        void DistSearchCacheAdapter::setData(const map<long, SearchCacheMemDataPtr>& res)  {
          map<long, DataPtr > data;
          for(map<long, SearchCacheMemDataPtr>::const_iterator iter = res.begin(); iter != res.end(); iter++)  {
            if((iter->second).get() == NULL) {
              MCE_WARN("Batch data to set is empty! user_id = "<<iter->first);
              continue;
            }
            string dataStr;
            (iter->second)->SerializeToString(&dataStr);
            DataPtr tmp(new Data( dataStr.begin(), dataStr.end() ) );
            data[iter->first] = tmp;
          }
          distCacheAdapterPtr->set(data);
        }


        long DistSearchCacheAdapter::getTimeMicroSec() {
          struct timeval time_tv;
          memset(&time_tv, 0, sizeof(timeval));
          gettimeofday(&time_tv, NULL);
          long time = time_tv.tv_sec * 1000 + time_tv.tv_usec / 1000;
          return time;
        }

      }
    }
}
