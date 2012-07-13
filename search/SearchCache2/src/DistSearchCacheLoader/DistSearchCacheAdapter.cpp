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
    namespace searchcache
    {
      using namespace xce::searchcache;
      using namespace com::xiaonei::xce;
      using namespace MyUtil;
      using namespace std;
      using namespace IceUtil;
      using namespace xce::dist::searchcache;
      using namespace xce::distcache;
      void DistSearchCacheAdapter::initialize()  {
        while(true)  {
          distCacheAdapterPtr = xce::distcache::DistCacheAdapter::connect("DistSearchCache");
          if(distCacheAdapterPtr.get() != NULL)  {
            MCE_WARN("connect right");
            break;
          }  else  {
            MCE_WARN("DistSearchCacheAdapter::initialize can not init DistCacheAdapter!");
          }
        }
      }

      void DistSearchCacheAdapter::logOut(const string& funStr, const SearchCacheMemDataPtr data, bool isDebug)  {
        stringstream os;
        os << funStr << " ";
        os << "Detail :" << " name = " << data->name() << " gender = " << data->gender()
          << " headurl = " << data->headurl() << " tinyurl = " << data->tinyurl()
          << " status = " << data->status() << " state = " << data->state()
          << " stage = " << data->stage() <<" safestatus = " << data->safestatus() << "browseconfig = " << data->browseconfig()
          << " statusconfig = " << data->statusconfig() << " basicconfig = " << data->basicconfig() 
          << " profileprivacy = " << data->profileprivacy();
        if (data->highschools_size() > 0)  {
          os << " highschool id = "<< data->highschools(0).id();
        }
        if( isDebug )
        {
          MCE_DEBUG(os.str());
        }else
        {
          MCE_INFO(os.str());
        }
      }

     bool DistSearchCacheAdapter::setData(long userId, const string& res) {
        //logOut("Set ", res, false);
        DataPtr tmp(new Data( res.begin(), res.end() ) );
        bool setOk = false;
        if(tmp != 0)  {
          setOk = distCacheAdapterPtr->set(userId, tmp);
       
        if(!setOk) {
          return setOk;
        }
       }  else {
         MCE_INFO("data is null!");
       } 
     }


      bool DistSearchCacheAdapter::setData(long userId, const SearchCacheMemDataPtr& res) {
        logOut("Set ", res, false);

        string dataStr;
        res->SerializeToString(&dataStr);
        DataPtr tmp(new Data( dataStr.begin(), dataStr.end() ) ); 
        bool setOk = distCacheAdapterPtr->set(userId, tmp);
        if(!setOk) {
          return setOk;
        }
      }

      vector<long> DistSearchCacheAdapter::setData(const map<long, SearchCacheMemDataPtr>& res) {
        map<long, DataPtr> result;
        vector<long> setIds;
        for(map<long, SearchCacheMemDataPtr>::const_iterator it = res.begin(); it != res.end(); ++it) {
          logOut("Set",it->second,false);
          string dataStr ;
          it->second->SerializeToString(&dataStr);
          DataPtr ptrTmp(new Data(dataStr.begin(), dataStr.end()));
          result.insert(make_pair(it->first, ptrTmp));
        }
        if(result.size() > 0)  {
          for(map<long, DataPtr>::const_iterator it = result.begin(); it != result.end(); ++it)  {
            distCacheAdapterPtr->set(it->first, it->second);
            setIds.push_back(it->first);
          }
        }
        return setIds;
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
          logOut("Get", data);
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
          if(it->second->size() <= 0) {
            MCE_WARN("DistSearchCacheAdapter::getDataWithCAS not in memcached userid = " << it->first);
            continue;
          }
          string dataStr(it->second->begin(), it->second->end());
          SearchCacheMemDataPtr data = new SearchCacheMemData;
          data->ParseFromString(dataStr);
          result.insert(make_pair(it->first,data));
          logOut("Get", data);
        }
        return result;
      }

    }
  }
}
