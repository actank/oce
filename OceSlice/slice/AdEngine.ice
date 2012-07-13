#ifndef ADENGINE_ICE
#define ADENGINE_ICE

#include <Util.ice>
module xce{
module ad{ 

struct AdInfo{
	long creativeid;
	long groupid;
	int updateTime;
	string text;
};
struct ZoneInfo{
  long zoneid;
  int needcount;
  string tag;
};
dictionary<long, ZoneInfo> ZoneInfoMap;
sequence<AdInfo> AdInfoSeq;

dictionary<long, AdInfoSeq> AdResultMap;

dictionary<long, int> AdZoneMap;
struct AdCreativeInfo {
	long creativeid;
	long groupid;
	long widgetid;
	int position;
	string text;
	string extra;
};
sequence<AdCreativeInfo> AdCreativeSeq;
dictionary<long, AdCreativeSeq> AdZone2Creatives;

class AdMatchManager{

	AdResultMap GetAds(string userProfile, string referer, int rotateindex); 
	AdZone2Creatives GetAdsByZone(string userProfile, string referer, int rotateindex); 
  AdZone2Creatives GetAdsByZoneInfo(string userProfile, string referer, int rotateindex, ZoneInfoMap zonemap);
	void Click(int uid , string ip,string text,string referer,string clickurl); 
	void Close(int uid, long creativeid);
	AdResultMap GetAdsTest(int uid, int stage,int gender,int age,string currentArea,string ipArea,long school ,int grade,int count, string referer, int rotateindex); 
	AdZone2Creatives GetAds4Monitor(int uid, int stage,int gender,int age,string currentArea,string ipArea,long school ,int grade,int count, string referer, int rotateindex); 
	
	void BindUser(int userid, long creativeid, long zoneid);
	void UnbindUser(int userid, long zoneid);
	void NotifyBindUser(int userid, long creativeid, long zoneid);
	void NotifyUnbindUser(int userid, long zoneid);
};

interface AdMatchMonitor {
  string MemberStatus(long id);
  string CampaignStatus(long id);
  string GroupStatus(long id);
  string CompeteRef(long id);
  string RotationCache(long zoneid);
};

interface AdFirstFlag {
  bool Clear();
};

};
};

#endif
