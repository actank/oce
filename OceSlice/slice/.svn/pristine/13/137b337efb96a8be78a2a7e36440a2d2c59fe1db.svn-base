#ifndef __TRACK_SUMMARY_ICE__
#define __TRACK_SUMMARY_ICE__

#include <Util.ice>

module xce
{
	module activetrack
	{
		interface TrackSummary {
			void access(int userId, string ip, string url);

			MyUtil::Str2StrMap get(int userId);
			void set(int userId, string key, string value);

			//void addAccessListener(MyUtil::Str2StrMap filter, AccessListener* listener);

			// ActionListener的注册条件还没有想好，暂时不做。
			//void addActionListener(string prefix, ActionListener* listener);
		};
	};
};
#endif

