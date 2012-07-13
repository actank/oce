#ifndef ADMATCH_ICE
#define ADMATCH_ICE

#include <Util.ice>
module xce{
module ad{ 

struct AdInfo{
	long creative;
	int updateTime;
	string text;
};

sequence<AdInfo> AdInfoSeq;

struct DefaultAd{
	long groupId;
	long crtvId;
	long bidId;
	long adzoneId;
	string refer;

};
sequence<DefaultAd> DefaultSeq;

class AdMatch{

	AdInfoSeq GetAds(string userProfile,int count,string referer); 
	void Click(int uid , string ip,string text,string referer,string clickurl); 
	//void Suspend(long groupId);
	//void Resume(long groupId);
	void LoadOneGroup(long gid);
	void ReloadOneGroup(long gid);
	void DelGroup(long gid);
	
	void ReportDefault(int uid,string ip,int adCount,DefaultSeq ads);

	bool GetAvailableAds(MyUtil::LongSeq ids);
	
	//bool SetClickInfo(long gid,int key,int click,int pv);

	
	AdInfoSeq GetAdsTest(int uid, int stage,
	int gender,int age,string currentArea,string ipArea,long school ,int grade,int count, string referer); //返回的string包括 该服务是哪一份，匹配广告的gid,素材id，坐标 
	
};


};
};

#endif
