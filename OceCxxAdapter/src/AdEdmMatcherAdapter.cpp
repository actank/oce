#include "AdEdmMatcherAdapter.h"

using namespace MyUtil;
using namespace xce::ad;

AdNotifierPrx AdEdmMatcherAdapter::getManager(int id) {
  return locate<AdNotifierPrx> (_managers, "M", id, TWO_WAY);
}

AdNotifierPrx AdEdmMatcherAdapter::getManagerOneway(int id) {
  return locate<AdNotifierPrx> (_managersOneway, "M", id, ONE_WAY);
}


void AdEdmMatcherAdapter::Login(int uid) {
  return getManagerOneway(uid)->Login(uid);
}

void AdEdmMatcherAdapter::Block(int uid, int promotion) {
  return getManagerOneway(uid)->Block(uid, promotion);
}

void AdEdmMatcherAdapter::SetInterval(int uid, IntervalType time) {
  return getManagerOneway(uid)->SetInterval(uid, time);
}

void AdEdmMatcherAdapter::MarkReadByUid(int uid) {
  return getManagerOneway(uid)->MarkReadByUid(uid);
}

void AdEdmMatcherAdapter::LoadOnePromotion(int promotion) {
  int size = cluster();
  for (int i = 0; i < size; ++i) {
    return getManagerOneway(i)->LoadOnePromotion(promotion);
  }
}

void AdEdmMatcherAdapter::DelPromotion(int promotion) {
  int size = cluster();
  for (int i = 0; i < size; ++i) {
    return getManagerOneway(i)->DelPromotion(promotion);
  }
}

AdPartners AdEdmMatcherAdapter::GetAdPartners(int uid, MyUtil::IntSeq ads) {
  return getManager(uid)->GetAdPartners(uid,ads);
}

HotAdInfoSeq AdEdmMatcherAdapter::GetAdInfoSeq() {
  return getManager(0)->GetAdInfoSeq();
}

void AdEdmMatcherAdapter::Reload() {
  getManagerOneway(0)->Reload();
}

