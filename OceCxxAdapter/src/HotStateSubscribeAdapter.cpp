#include "HotStateSubscribeAdapter.h"

HotStateSubscribeAdapter::HotStateSubscribeAdapter():MyUtil::ReplicatedClusterAdapterI<HotStateSubscribePrx>("ControllerStateKeyWords", 120, 300)  {
}

void HotStateSubscribeAdapter::update(IndexUpdateData info)  {
  HotStateSubscribePrx proxy = getProxy(0);
  try  {
    proxy->update(info);
  }  catch (Ice::Exception& e)  {
    MCE_WARN("Ice::Exception in HotStateSubscribeAdapter::update prx=" << proxy << ". " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("std::exception in HotStateSubscribeAdapter::update prx=" << proxy << "." << e.what())
  } catch (...) {
    MCE_WARN("unknown exception in HotStateSubscribeAdapter::update prx=" << proxy << ".");
  }
}

bool HotStateSubscribeAdapter::test(const string & pattern, const string& inputState)  {

}
  
vector<map<string, string> >  HotStateSubscribeAdapter::getNewState(const string& type, int& limit)  {
  HotStateSubscribePrx proxy = getProxy(0);
  try  {
    vector<map<string, string> > data = proxy->getNewState(type, limit);
    return data;
  }  catch (Ice::Exception& e)  {
      MCE_WARN("Ice::Exception in HotStateSubscribeAdapter::update prx=" << proxy << ". " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("std::exception in HotStateSubscribeAdapter::update prx=" << proxy << "." << e.what())
    } catch (...) {
      MCE_WARN("unknown exception in HotStateSubscribeAdapter::update prx=" << proxy << ".");
    }
}


bool  HotStateSubscribeAdapter::subscribe(const string& type, const string& pattern, int& maxSize, string& passwd)  {
  HotStateSubscribePrx proxy = getProxy(0);
  try  {
  proxy->subscribe(type, pattern, maxSize, passwd);
  }  catch (Ice::Exception& e)  {
      MCE_WARN("Ice::Exception in HotStateSubscribeAdapter::update prx=" << proxy << ". " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("std::exception in HotStateSubscribeAdapter::update prx=" << proxy << "." << e.what())
    } catch (...) {
      MCE_WARN("unknown exception in HotStateSubscribeAdapter::update prx=" << proxy << ".");
    }

}

bool  HotStateSubscribeAdapter::unSubscribe(const string& type)  {
HotStateSubscribePrx proxy = getProxy(0);
  try  {
  proxy->unSubscribe(type);
  }  catch (Ice::Exception& e)  {
      MCE_WARN("Ice::Exception in HotStateSubscribeAdapter::update prx=" << proxy << ". " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("std::exception in HotStateSubscribeAdapter::update prx=" << proxy << "." << e.what())
    } catch (...) {
      MCE_WARN("unknown exception in HotStateSubscribeAdapter::update prx=" << proxy << ".");
    }
}

vector<map<string, string> >  HotStateSubscribeAdapter::showTypeInfos()  {

}

map<string, string>  HotStateSubscribeAdapter::showTypeInfo(const string& type)  {

}

