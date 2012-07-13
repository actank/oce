#ifndef VIEWCOUNT_ICE
#define VIEWCOUNT_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	exception ViewCountException extends MyUtil::MceException{};

	interface ViewCountManager
    {
		void incThreadCount(int threadId, int count);

		void incBlogCount(int blogId, int count);
		
		void incPhotoCount(int photoId, int count);

        void incUserCount(int viewCount, int count);
    };
};
};
};
};
#endif
