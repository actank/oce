/*
 * FeedViewAdapter.cpp
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#include <cmath>
#include "FeedViewAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedViewPrx FeedViewAdapter::getManager(int id) {
	return locate<FeedViewPrx> (_managers, "feedview", id, TWO_WAY);
}

FeedViewPrx FeedViewAdapter::getManagerOneway(int id) {
	return locate<FeedViewPrx> (_managersOneway, "feedview", id, ONE_WAY);
}


void FeedViewAdapter::Test(int uid, string name, int render, int from, int count) {
  FeedViewPrx::uncheckedCast(getManagerOneway(uid)->ice_datagram())->Test(uid, name, render, from, count);
  return ;
}

FeedDataSeq FeedViewAdapter::GetByIds2View(int uid, const string& name, int render, const FeedIdSeq & fids, int view) {
  return FeedViewPrx::uncheckedCast(getManager(uid))->GetByIds2View(uid, name, render, fids, view);
}



