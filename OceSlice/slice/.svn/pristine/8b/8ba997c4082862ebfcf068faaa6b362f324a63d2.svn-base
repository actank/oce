#ifndef __BulletinSearch_ICE__
#define __BulletinSearch_ICE__

#include <Util.ice>

module com
{
module xiaonei
{
module bulletinsearch
{
	class UserClick 
	{
		int userId;
		MyUtil::IntSeq clickedIds;
	};

	interface BulletinManager
	{
		string bestBulletin(int userId);

		void userClicked(int userId, int bulletinId);
		
		void rebuildIndex();
	}; 
};
};
};

#endif

