#ifndef __NEWBIEFEEDER_ICE__
#define __NEWBIEFEEDER_ICE__

#include <Feed.ice>

module com
{
	module xiaonei
	{
		module service
		{
			module feed
			{
				interface NewbieFeederManage
				{
					void publishFeed(mop::hi::svc::feed::FeedPublish data);
				};
			};
		};
	};
};


#endif
