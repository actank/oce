#ifndef _FEEDSTAT_ICE
#define _FEEDSTAT_ICE

module xce{
	module feed{
		class FeedStatManager{
			void dispatchRecord(int stype, string logmsg);
			void replyRecord(int stype, string logmsg);
			void deliverRecord(int stype, string logmsg, int count);
		};
	};
};

#endif

