#include "StateKeyWordsAdapter.h"

StateKeyWordsAdapter::StateKeyWordsAdapter():MyUtil::ReplicatedClusterAdapterI<IndexUpdateLogicPrx>("ControllerStateKeyWords", 120, 300)  {
}

void StateKeyWordsAdapter::update(IndexUpdateData info)  {
  IndexUpdateLogicPrx proxy = getProxy(0);
  cout<<"proxy is "<< proxy<<endl;
  try  {
  proxy->update(info);
  }  catch (Ice::Exception& e)  {
      MCE_WARN("Ice::Exception in StateKeyWordsAdapter::update prx=" << proxy << ". " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("std::exception in StateKeyWordsAdapter::update prx=" << proxy << "." << e.what())
    } catch (...) {
      MCE_WARN("unknown exception in StateKeyWordsAdapter::update prx=" << proxy << ".");
    }
}

void StateKeyWordsAdapter::setValid(bool newState)  {

}

void StateKeyWordsAdapter::setRecovery(bool newState)  {

}

bool StateKeyWordsAdapter::isRecovery()  {

}

bool StateKeyWordsAdapter::isValid(const Ice::ObjectPrx& proxy)  {

}

