#include "UgcGossipAdapter.h"

using namespace xce::ucgcomment;

GossipLogicPrx UgcGossipAdapter::getManager(int id) {
  return locate<GossipLogicPrx>(_managers, "M", id, TWO_WAY);
}

GossipLogicPrx UgcGossipAdapter::getManagerOneway(int id) {
  return locate<GossipLogicPrx>(_managersOneway, "M", id, ONE_WAY);
}

int UgcGossipAdapter::send(int host, int owner, const string& message){
  return getManager(host)->send(host, owner, message);
}
