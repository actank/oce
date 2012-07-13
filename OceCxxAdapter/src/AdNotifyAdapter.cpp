/*
 * AdNotifyAdapter.cpp
 *
 *  Created on: Jul 2, 2010
 *      Author: yejingwei
 */

#include "AdNotifyAdapter.h"

using namespace MyUtil;
using namespace xce::ad;

AdNotifierPrx AdNotifierAdapter::getManager(int id) {
  return locate<AdNotifierPrx> (_managers, "M", id, TWO_WAY);
}

AdNotifierPrx AdNotifierAdapter::getManagerOneway(int id) {
  return locate<AdNotifierPrx> (_managersOneway, "M", id, ONE_WAY);
}


void AdNotifierAdapter::Login(int uid) {
  return getManagerOneway(uid)->Login(uid);
}

void AdNotifierAdapter::Block(int uid, int promotion) {
  return getManagerOneway(uid)->Block(uid, promotion);
}

void AdNotifierAdapter::SetInterval(int uid, IntervalType time) {
  return getManagerOneway(uid)->SetInterval(uid, time);
}

void AdNotifierAdapter::MarkReadByUid(int uid) {
  return getManagerOneway(uid)->MarkReadByUid(uid);
}

void AdNotifierAdapter::LoadOnePromotion(int promotion) {
  int size = cluster();
  for (int i = 0; i < size; ++i) {
    return getManagerOneway(i)->LoadOnePromotion(promotion);
  }
}

void AdNotifierAdapter::DelPromotion(int promotion) {
  int size = cluster();
  for (int i = 0; i < size; ++i) {
    return getManagerOneway(i)->DelPromotion(promotion);
  }
}

AdPartners AdNotifierAdapter::GetAdPartners(int uid, MyUtil::IntSeq ads) {
  return getManager(uid)->GetAdPartners(uid,ads);
}

HotAdInfoSeq AdNotifierAdapter::GetAdInfoSeq() {
  return getManager(0)->GetAdInfoSeq();
}

void AdNotifierAdapter::Reload() {
  getManagerOneway(0)->Reload();
}

