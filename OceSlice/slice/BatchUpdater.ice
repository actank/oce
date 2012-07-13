#ifndef BATCHED_UPDATER_ICE
#define BATCHED_UPDATER_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{

module batchtraits
{
	const string UserViewName="USERVIEWCOUNTER";
	const string NewMessageName="NEWMESSAGECOUNTER";
	const string LoginName="LOGINCOUNTER";
	const string MessageInName="MESSAGEINCOUNTER";
	const string FriendName="FRIENDCOUNTER";
	const string FriendDistName="FRIENDDISTCOUNTER";
	const string ExpName="EXPCOUNTER";
	const string TribeName="TRIBECOUNTER";
	const string TribeRequestName="TRIBEREQUESTCOUNTER";
	const string MessageOutName="MESSAGEOUTCOUNTER";
	const string TribePageViewName="TRIBEPAGEVIEWCOUNTER";
	const string FlyerUserAdName="FLYERUSERADCOUNTER";
	const string LeavewordName="LEAVEWORDNAME";
	const string GuestRequestName="GUESTREQUESTCOUNTER";
	const string BlogViewName="BLOGVIEWCOUNTER";
	const string PhotoViewName="PHOTOVIEWCOUNTER";
	const string PhotoViewNName="PHOTOVIEWNCOUNTER";
	const string FeedCommentName="FEEDCOMMENTCOUNTER";
	const string GeneralUserCountName="GUN";
	const string GeneralUserApiCountName="GUAN";
	const string UgcUserScoreName="UGCUSERSCORE";
	const string UserScoreName="USERSCORE";
	const string BulletinClickName="BULLETINCLICK";
	const string GeneralUserSetterName="GUS";
	const string GeneralUserApiSetterName="GUAS";
	const string LastLoginName="LASTLOGIN";
	const string LastLogoutName="LASTLOGOUT";
	const string LastModifyName="LASTMODIFY";

	const string ShareVideoViewName="SHAREVIDEOVIEWCOUNTER";
	const string NotifyCountName="NOTIFYCOUNTER";
	const string NotifySetterName="NOTIFYSETTER";

};
module model
{
	interface BatchUpdaterManager
	{
		void inc(MyUtil::IntSeq keyvalue,int count,string type);
		void dec(MyUtil::IntSeq keyvalue,int count,string type);
		void set(MyUtil::IntSeq keyvalue,string value,string type);

		void incLong(MyUtil::LongSeq keyvalue,int count,string type);
                void decLong(MyUtil::LongSeq keyvalue,int count,string type);
                void setLong(MyUtil::LongSeq keyvalue,string value,string type);


		void incUserViewCount(int id, int count);
		void incLoginCount(int id, int count);

		void setLastLoginTime(int id, string time);
		void setLastLogoutTime(int id, string time);
		void setLastModifyTime(int id, string time);

		void incFriendDist(int userId, int uniId, int count);
		void decFriendDist(int userId, int uniId, int count);

		// users table:
		void incExp(int userId, int count);
		void decExp(int userId, int count);
		void incTribeCount(int userId, int count);
		void decTribeCount(int userId, int count);
		void incTribeRequestCount(int userId, int count);
		void decTribeRequestCount(int userId, int count);
		void incNewMessageCount(int userId, int count);
		void decNewMessageCount(int userId, int count);
		
		void incMessageInCount(int userId, int count);
		void decMessageInCount(int userId, int count);
		void incMessageOutCount(int userId, int count);
		void decMessageOutCount(int userId, int count);
		void incFriendCount(int id, int count);
		void decFriendCount(int id, int count);
		void incGuestRequestCount(int id, int count);
		void decGuestRequestCount(int id, int count);
		void incLeavewordCount(int id, int count);
		void decLeavewordCount(int id, int count);
	    
		void incBlogViewCount(int id, int userId, int count);
		void decBlogViewCount(int id, int userId, int count);
	    void incPhotoViewCount(int id, int userId, int count);
		void decPhotoViewCount(int id, int userId, int count);
        void incTribePageViewCount(int tribeId,int count);
        void incFlyerUserAdCount(int userId,int adId, int count);

    };
};
};
};
};
#endif
