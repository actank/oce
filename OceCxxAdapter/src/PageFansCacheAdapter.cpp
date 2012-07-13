#include "PageFansCacheAdapter.h"
#include "PageFans.h"
 
using namespace xce::buddy;

PageFansCacheAdapter::PageFansCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <PageFansManagerPrx>("ControllerPageFansCache", 120, 300){
}

void PageFansCacheAdapter::reload(const ::MyUtil::ObjectResultPtr& data )
{
		try {
			 	
				MyUtil::ObjectMap::iterator it = data->data.begin();
				if(it == data->data.end())
				{
						MCE_WARN("PageFansCacheAdapter::reload -> data.size()<0");
						return ;
				}
		        int id = it->first;	
				std::vector<PageFansManagerPrx> proxies = getAllProxySeq(id);
				if (proxies.size() <= 0) {
						MCE_WARN("PageFansCacheAdapter::addFan -> no proxy");
				}
				for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
						(*i)->setData(data);

				}
				return ;

		} catch (const Ice::Exception& e) {
				MCE_WARN("[PageFansCacheAdapter::addFan] Ice::Exception " << e);
		}
		return ;


}

int PageFansCacheAdapter::addFan(PageEntryPtr pe)
{
  try {
		   std::vector<PageFansManagerPrx> proxies = getAllProxySeq(pe->pageId);
		   if (proxies.size() <= 0) {
				   MCE_WARN("PageFansCacheAdapter::addFan -> no proxy");
		   }
		   for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
				   (*i)->addFan(pe);

		   }
		   return 1;

  } catch (const Ice::Exception& e) {
    MCE_WARN("[PageFansCacheAdapter::addFan] Ice::Exception " << e);
  }
	return 0;	
}

int PageFansCacheAdapter::removeFan(int pageId, int userid)
{
  try {
		  std::vector<PageFansManagerPrx> proxies = getAllProxySeq(pageId);
		  if (proxies.size() <= 0) {
				  MCE_WARN("PageFansCacheAdapter::removeFan -> no proxy");
		  }
		  for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
				  (*i)->removeFan(pageId,userid);
		  }
		  return 1;
  } catch (const Ice::Exception& e) {
    MCE_WARN("[PageFansCacheAdapter::removeFan] Ice::Exception " << e);
  }

	return 0;	
}
int PageFansCacheAdapter::updateFanWithState(PageEntryPtr pe)
{
  try {
		  std::vector<PageFansManagerPrx> proxies = getAllProxySeq(pe->pageId);
		  if (proxies.size() <= 0) {
				  MCE_WARN("PageFansCacheAdapter::updateFansWithState -> no proxy");
		  }
		  for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
				  (*i)->updateFanWithState(pe);
		  }
		  return 1;

  } catch (const Ice::Exception& e) {
    MCE_WARN("[PageFansCacheAdapter::updateFanWithState] Ice::Exception " << e);
  }

	return 0;	
}
int PageFansCacheAdapter::updateFansWithState(int pageId, int oriState, int tarState)
{
  try {
		  std::vector<PageFansManagerPrx> proxies = getAllProxySeq(pageId);
		  if (proxies.size() <= 0) {
				  MCE_WARN("PageFansCacheAdapter::updateFansWithState -> no proxy");
		  }
		  for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
				  (*i)->updateFansWithState(pageId, oriState, tarState);
		  }
		  return 1;
  } catch (const Ice::Exception& e) {
    MCE_WARN("[PageFansCacheAdapter::updateFansWithState] Ice::Exception " << e);
  }

	return 0;	
}

int PageFansCacheAdapter::addBlock(int pageId, int userId, int operatorId, int timestamp)
{
		
	try {
		std::vector<PageFansManagerPrx> proxies = getAllProxySeq(pageId);
		if (proxies.size() <= 0) {
				MCE_WARN("PageFansCacheAdapter::addBlock -> no proxy");
		}
		for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
				(*i)->addBlock(pageId, userId, operatorId, timestamp);
		}
		return 1;
	} catch (const Ice::Exception& e) {
			MCE_WARN("[PageFansCacheAdapter::addBlock] Ice::Exception " << e);
	}
	return 0;	
}
int PageFansCacheAdapter::removeBlock(int pageId, int userId)
{
  try {
		std::vector<PageFansManagerPrx> proxies = getAllProxySeq(pageId);
		if (proxies.size() <= 0) {
				MCE_WARN("PageFansCacheAdapter::removeBlock -> no proxy");
		}
		for (std::vector<PageFansManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
				(*i)->removeBlock(pageId,userId);
		}
		return 1;
  } catch (const Ice::Exception& e) {
    MCE_WARN("[PageFansCacheAdapter::removeBlock] Ice::Exception " << e);
  }

	return 0;	
}

