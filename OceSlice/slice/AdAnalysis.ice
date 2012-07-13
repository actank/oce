#ifndef _AD_ANALYSIS_ICE_
#define _AD_ANALYSIS_ICE_

#include <Util.ice>

module xce{
module ad{
	class AdPvParameter{
		long groupid;
		long creativeid;
		string http;
		string ip;
		string referer;
		long bitunitid;
	};
	class AdClickParameter{
		//int clickpos;
		long groupid;
		long creativeid;
		int clickprice;
		long bitunitid;
		int userid;
		string ip;
		string referer;
		string ubc;
		string clickurl;
		string pvtime;
	};
	sequence<AdPvParameter> AdPvParameterSeq;
	
	class AdAnalysisManager{
		void Pv(AdPvParameterSeq paraseq);
		void Click(AdClickParameter para);
		void Pv2(AdPvParameterSeq paraseq, MyUtil::StrSeq appendstr);
		void Click2(AdClickParameter para, string appendstr);
		void ReloadGroupById(long groupid);
		void ReloadMemberById(long memberid);
		void ReloadCampaignById(long planid);
		void ReloadCreativeById(long creativeid);
	};
};
};

#endif
