/*
 * DistSearchLogicAdapter.cpp
 *
 *  Created on: 2010-12-13
 *      Author: lijie
 */

#include "DistSearchLogicAdapter.h"

using namespace xce::searchcache;
using namespace MyUtil;

void DistSearchLogicAdapter::load(int id) {
	_invokeLoad.invoke(id);
}

void DistSearchLogicAdapter::load(const MyUtil::IntSeq& ids) {
	_invokeLoad.invoke(ids);
}

void DistSearchLogicAdapter::update(const int id, const std::string tableName, const std::map<string, string>& fieldPairMap)
{
    _invokeUpdate.invoke(id, tableName, fieldPairMap);
}

DistSearchLogicAdapter::DistSearchLogicAdapter():MyUtil::ReplicatedClusterAdapterI<SearchCacheManagerPrx>("ControllerDistSearchLogic", 120, 300){ 
	_invokeLoad.start(128 * 1024).detach();
	_invokeUpdate.start(128 * 1024).detach();
	_managers.resize(cluster() > 0 ? cluster() : 1);
}

void DistSearchLogicAdapter::doLoad(const MyUtil::IntSeq& ids) {
	if (ids.size() <= 0) {
		return;
	}
    SearchCacheManagerPrx proxy = getProxy(0);
    try  {
       proxy->load(ids);
    
    }  catch (Ice::Exception& e)  {
      MCE_WARN("Ice::Exception in DistSearchLogicAdapter::doLoad. prx=" << proxy << ". " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("std::exception in DistSearchLogicAdapter::doLoad. prx=" << proxy << "." << e.what())
    } catch (...) {
      MCE_WARN("unknown exception in DistSearchLogicAdapter::doLoad. prx=" << proxy << ".");
    }
}

void DistSearchLogicAdapter::doUpdate(const UpdateUnitSeq& updateUnits) {
  if (updateUnits.size() <= 0) {
    return;
  }
    SearchCacheManagerPrx proxy = getProxy(0);
    if(proxy)  {
      try{

        //MCE_DEBUG(proxy);
        proxy->update(updateUnits);

      } catch(Ice::Exception& e) {
        MCE_WARN("Exception in DistSearchLogicAdapter::doUpdate. prx=" << proxy << ". "<< e.what());
      } catch (std::exception& e) {
        MCE_WARN("std::exception in DistSearchLogicAdapter::doUpdate. prx=" << proxy << ". " << e.what());
      } catch (...) {
        MCE_WARN("unknown exception in DistSearchLogicAdapter::doUpdate. prx=" << proxy << ". ");
      }
    }
}

void DistSearchLogicAdapter::setValid(int service_mod, int index, bool newState)  {//设置mod为service_mod的第index份服务可用状态为newState, 
                                                                               //可用为true,不可用为false
 
  MCE_WARN("Service_mod is "<<service_mod<<" Index of proxy is: "<<index<<" NewState is: "<<newState);
  std::map<int, std::vector<SearchCacheManagerPrx> > map_proxies = getAvailableProxies();
  std::map<int, std::vector<SearchCacheManagerPrx> >::iterator iter_map = map_proxies.find(service_mod);
  if(iter_map != map_proxies.end())  {
    if(index >= (int)iter_map->second.size()) 
      return;
    SearchCacheManagerPrx proxy = iter_map->second.at(index);
    if (newState) {
      proxy->setValid(1);
    
      MCE_WARN("Invoked proxy is: "<<*proxy);
    } else {
      proxy->setValid(0);
  
      MCE_WARN("Stopped proxy is: "<<*proxy);
    }
  }
}

void DistSearchLogicAdapter::isValidTest(int service_mod, std::vector<SearchCacheManagerPrx>& proxies)  {//SearchPersistentCache服务HA框架，更新测试用
  std::map<int, std::vector<SearchCacheManagerPrx> > map_proxies = getAvailableProxies();
  std::map<int, std::vector<SearchCacheManagerPrx> >::iterator iter_map = map_proxies.find(service_mod);
  if(iter_map != map_proxies.end())  {
    for(std::vector<SearchCacheManagerPrx>::iterator iter =  iter_map->second.begin(); iter != iter_map->second.end(); iter++)  {
      if(SearchCacheManagerPrx::uncheckedCast(*iter)->isValid()) {
        MCE_INFO("proxy "<<*iter<<" still alive!");
        proxies.push_back(*iter);
      }
    }
  }
}



bool DistSearchLogicAdapter::isValid(const Ice::ObjectPrx& proxy){
	return SearchCacheManagerPrx::uncheckedCast(proxy)->isValid();
}



//***************************************************************************

void DistSearchLogicAdapter::SearchCacheLoadTask::handle() {
	DistSearchLogicAdapter::instance().doLoad(_ids);	
}

void DistSearchLogicAdapter::SearchCacheUpdateTask::handle() {
	DistSearchLogicAdapter::instance().doUpdate(_updateUnits);	
}
