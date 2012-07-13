#ifndef __MUC_TALKRECORD_ICE__
#define __MUC_TALKRECORD_ICE__

#include <MucCommon.ice>
module com
{
module xiaonei
{
module talk
{
module muc
{
	class MucTalkRecordManager{
		void RecordTalkMsg(int userid, int groupid, string msg);
		MucTalkMessageSeq GetRecentlyMsg(int groupid);
	};
};
};
};
};

#endif
