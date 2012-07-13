#ifndef __ACTIVE_TRACK_ICE__
#define __ACTIVE_TRACK_ICE__

#include <Util.ice>

module xce
{
	module activetrack
	{
		interface ActiveTrackFacade {
			void access(int userId, string ip, string url);
			void action(int userId, MyUtil::StrSeq paths, string actionP, string objectP);
		};

		interface AccessListener {
			["amd"] void access(int userId, string url);
		};

		interface ActionListener {
			["amd"] void action(int userId, MyUtil::StrSeq paths, string actionP, string objectP);
		};
		
		interface ActiveMotionFacade {
			void motion(MyUtil::StrSeq datas);
		};

	};
};
#endif

