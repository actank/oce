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
              break;
            }  else  {
               MCE_INFO("DistSearchCacheAdapter::initialize can not init DistCacheAdapter!");
            }
          }
          last_oper_time_.resize(GROUP_SIZE);
          time_to_userid_.resize(GROUP_SIZE);
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
          /*  if( isDebug )
          {
            MCE_DEBUG(os.str());
          }else
          {
            MCE_INFO(os.str());
          }*/
        }

        bool DistSearchCacheAdapter::resetData(long userId) {
          MCE_DEBUG("[DistSearchCacheAdapter::resetData] userid = " << userId);
          MyUtil::LongSeq tmp;
          tmp.push_back(userId); 
          return UpdateTask::instance().UpdateSqlBatch(tmp);
        }

        void DistSearchCacheAdapter::resetData(const vector<long> userIds) {
          UpdateTask::instance().UpdateSqlBatch(userIds);
        }

        bool DistSearchCacheAdapter::checkConflict(long userId, const SearchCacheMemDataPtr& res, long oper_time, bool& need_reset) {
          timeval start_time;
          gettimeofday(&start_time, NULL);
          int groupId = userId % GROUP_SIZE;
          hash_map<int, long>::iterator it = last_oper_time_[groupId].find(userId);
          if(it == last_oper_time_[groupId].end()) {
            pair<hash_map<int, long>::iterator, bool> tmp_pair = last_oper_time_[groupId].insert(make_pair(userId, oper_time));
            if(tmp_pair.second) {
              time_to_userid_[groupId].insert(make_pair(oper_time, userId));
              return true;
            } else {
              return false;
            }
          } else {
            if(oper_time - it->second > OPER_INTERVAL_TIME) {
              multimap<long, int>::iterator itr;
              for(itr = time_to_userid_[groupId].begin(); itr->first <= it->second && itr != time_to_userid_[groupId].end(); ++itr) {
                last_oper_time_[groupId].erase(itr->second);
              }
              time_to_userid_[groupId].erase(time_to_userid_[groupId].begin(), itr);
              pair<hash_map<int, long>::iterator, bool> tmp_pair = last_oper_time_[groupId].insert(make_pair(userId, oper_time));
              if(tmp_pair.second) {
                time_to_userid_[groupId].insert(make_pair(oper_time, userId));
                return true;
              } else {
                return false;
              }
            } else {
              it->second = oper_time;
              need_reset = true;
              return false;
            }
          }
        }


        bool DistSearchCacheAdapter::setData(long userId, const SearchCacheMemDataPtr& res) {
         // logOut("Set ", res, false);
          if(res.get() == NULL)  {
            MCE_INFO("Data to set is empty!");
            //string dataStr = "";
            //DataPtr tmp(new Data(dataStr.begin(), dataStr.end()));
            //bool setOk = distCacheAdapterPtr->set(userId, tmp);
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
              MCE_INFO("Batch data to set is empty! user_id = "<<iter->first);
             // string dataStr = "";
             // DataPtr tmp(new Data(dataStr.begin(), dataStr.end()));
             // data[iter->first] = tmp;
              continue;
            }
            string dataStr;
            (iter->second)->SerializeToString(&dataStr);
            DataPtr tmp(new Data( dataStr.begin(), dataStr.end() ) );
            data[iter->first] = tmp;
          }
          distCacheAdapterPtr->set(data);
        }


        bool DistSearchCacheAdapter::setPageData(long userId, const SearchCacheMemPageDataPtr& res) {
          string dataStr;
          res->SerializeToString(&dataStr);
          DataPtr tmp(new Data( dataStr.begin(), dataStr.end() ) );
          bool setOk = distCacheAdapterPtr->set(userId, tmp);
          return setOk;
        }

        void DistSearchCacheAdapter::setPageData(const map<long, SearchCacheMemPageDataPtr>& res)  {
          map<long, DataPtr> data;
          for(map<long, SearchCacheMemPageDataPtr>::const_iterator iter = res.begin(); iter != res.end(); iter++)  {
            string dataStr;
            (iter->second)->SerializeToString(&dataStr);
            DataPtr tmp(new Data( dataStr.begin(), dataStr.end() ) );
            data[iter->first] = tmp;
          }
          distCacheAdapterPtr->set(data);
        }


        SearchCacheMemDataPtr DistSearchCacheAdapter::getData(long id) {
          DataPtr dataPair = distCacheAdapterPtr->get(id);
          if(dataPair.get() != NULL)  {
            if(dataPair->size() <= 0) {
              MCE_INFO("DistSearchCacheAdapter::getDataWithCAS not in memcached userid = " << id);
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
            //if(it->second->size() <= 0) {
            if(NULL == it->second.get())  {
              MCE_INFO("DistSearchCacheAdapter::getDataWithCAS not in memcached userid = " << it->first);
              continue;
            }
            string dataStr(it->second->begin(), it->second->end());
            SearchCacheMemDataPtr data = new SearchCacheMemData;
            data->ParseFromString(dataStr);
            result.insert(make_pair(it->first,data));
          }
          return result;
        }
     
        SearchCacheMemPageDataPtr DistSearchCacheAdapter::getPageData(long id) {
          DataPtr dataPair = distCacheAdapterPtr->get(id);
          if(dataPair.get() != NULL)  {
            if(dataPair->size() <= 0) {
              MCE_INFO("DistSearchCacheAdapter::getDataWithCAS not in memcached pageid = " << id);
              SearchCacheMemPageDataPtr tmp = new SearchCacheMemPageData;
              return tmp;
            }
            string dataStr(dataPair->begin(), dataPair->end());
            SearchCacheMemPageDataPtr data = new SearchCacheMemPageData;
            data->ParseFromString(dataStr);
            return data;
          }  else  {
            SearchCacheMemPageDataPtr tmp = new SearchCacheMemPageData;
            return tmp;
          }
        }


        map<long, SearchCacheMemPageDataPtr> DistSearchCacheAdapter::getPageData(vector<long> ids) {
          map<long, SearchCacheMemPageDataPtr> result;
          
          map<long, DataPtr> propMap = distCacheAdapterPtr->get(ids);
           
          for(map<long, DataPtr>::const_iterator it = propMap.begin(); it != propMap.end(); ++it) {
            if(it->second->size() <= 0) {
              MCE_INFO("DistSearchCacheAdapter::getDataWithCAS not in memcached userid = " << it->first);
              continue;
            }
            string dataStr(it->second->begin(), it->second->end());
            SearchCacheMemPageDataPtr data = new SearchCacheMemPageData;
            data->ParseFromString(dataStr);
            result.insert(make_pair(it->first,data));
          }
          return result;
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
