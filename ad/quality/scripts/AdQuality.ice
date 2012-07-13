#ifndef ADQUALITY_ICE
#define ADQUALITY_ICE

#include <Util.ice>
module xce{
module ad{ 

struct AdQuality{
	long groupId;
	double quality;
        bool isFresh;
};

dictionary<long, AdQuality> AdQualityMap;

struct AdStat {
  string userKey;
  long groupId;
  int pvCount;
  int clickCount;
  string time;
};
sequence<AdStat> AdStatSeq;
dictionary<string, AdStatSeq> AdStatMap;

interface AdQualityManager {
	AdQualityMap GetQualitys(string userkey); 
        idempotent void update(AdStatMap sm);
        idempotent void reload();
        string GetLastUpdateTime();
};

};
};

#endif
