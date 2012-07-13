#ifndef ADFILTER_ICE
#define ADFILTER_ICE

#include <Util.ice>
module xce{
module ad{ 



class AdFilter{

	string GetAds(string userProfile); //返回的string包括 该服务是哪一份，匹配广告的gid,素材id，坐标 
	void Click(string sendedAdInfo); //AdView调用,sended_ad_info为原来发送的广告的信息，和广告的坐标
	void NotifyRunOut(long groupId);//AdAnalysis调用，说明group没钱了
	
};


};
};

#endif
