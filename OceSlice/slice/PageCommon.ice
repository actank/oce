#ifndef _PAGECOMMON_ICE_
#define _PAGECOMMON_ICE_

#include <Util.ice>

module xce {
module buddy {

class PageEntry
{
	int id;
	int pageId;
	int userId;
	int state;
	int addTime;
	int operatorId;
};

sequence<PageEntry> PageEntryList;
dictionary<int, PageEntryList> PageEntryListMap;

class PageFansObj
{
    PageEntryList realfans;
    PageEntryList otherfans;
    PageEntryList blockfans;
};

class PageEntrySet {
  PageEntryList pageEntries;
};

struct CommonFans {
  MyUtil::IntSeq common;
  MyUtil::IntSeq uncommon;
};

dictionary<int, CommonFans> CommonFansMap; // key:pid

struct PageCommonFans {
  int uid;
  CommonFansMap dataMap;
};


};
};
#endif
