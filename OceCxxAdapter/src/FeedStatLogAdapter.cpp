/*
 * FeedStatLogAdapter.cpp
 *
 *  Created on: Nov 1, 2011
 *      Author: qun.liu
 */

#include "FeedStatLogAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedStatLogPrx FeedStatLogAdapter::getManager() {
	return locate<FeedStatLogPrx> ("M", TWO_WAY);
}

FeedStatLogPrx FeedStatLogAdapter::getManagerOneway() {
	return locate<FeedStatLogPrx> ("M", ONE_WAY); 
}

void FeedStatLogAdapter::AddLog(MyUtil::StrSeq category, string log) {
  return FeedStatLogPrx::uncheckedCast(getManagerOneway()->ice_datagram())->AddLog(category, log);
}

