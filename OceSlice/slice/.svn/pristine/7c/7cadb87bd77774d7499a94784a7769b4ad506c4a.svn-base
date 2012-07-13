#ifndef _IPRECORD_ICE_
#define _IPRECORD_ICE_

#include <Util.ice>

module xce {
module iprecord {

	interface IPRecordManager {
		MyUtil::IntSeq getUsers(long ip,int limit);
		MyUtil::IntSeq getUsersWithMask(long ip,int limit,MyUtil::IntSeq mask);
		int getCount(long ip);
		void addUser(long ip,int userId);
		void removeUser(long ip,int userId);
		void reload(long ip);
		void updateToDB(long ip);
	};

};
};

#endif
