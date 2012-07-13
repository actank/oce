#ifndef ADDEMOQUALITY_ICE
#define ADDEMOQUALITY_ICE

module xce{
module ad{ 
struct CpmAdgroup {
  long groupid;
  long pv;
  int charge;
  double cpm;
};
dictionary<long, double> FreshAdQualitySet;
dictionary<string, FreshAdQualitySet> FreshAdQualityMap;
dictionary<string, double> AdQualityMap;
dictionary<string, double> AdPosMap;


sequence<int> PosSeq;
dictionary<string, PosSeq> AdSortPosMap;

dictionary<string, double> AdQualityScoreMap;

dictionary<string,CpmAdgroup> CpmAdgroupMap;
dictionary<long,int> LimitedAdGroupMap;


interface AdDemoQualityManager {
  AdQualityMap GetDemoPool(); 
  AdQualityMap GetAppPool(); 
  FreshAdQualityMap GetFreshAdMap();
  FreshAdQualityMap GetDisplayLessAds();
  AdPosMap GetAverageEcpm();
  AdSortPosMap GetAppSortPos(); 

  AdQualityScoreMap  GetAdsQuality();  //insert by shuang  

  CpmAdgroupMap GetCpmAdGroupMap();
  LimitedAdGroupMap GetLimitedAdGroupMap();
};

};
};

#endif
