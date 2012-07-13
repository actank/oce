#ifndef XCE_MONITOR_ICE
#define XCE_MONITOR_ICE
#include <Util.ice>
module xce
{
module monitor
{
	interface XiaoNeiMonitor
	{
		void report(MyUtil::Str2StrMap data);
		void reload();
		void disable(MyUtil::StrSeq subjects);
//		void reports(MyUtil::Str2StrMapSeq datas);
	};
};
};
#endif
