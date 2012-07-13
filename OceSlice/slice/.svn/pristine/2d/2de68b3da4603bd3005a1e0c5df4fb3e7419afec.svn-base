#ifndef USER_LOGIC_ICE
#define USER_LOGIC_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
    interface UserLogicManager
    {
		bool isValid();
		void setValid(bool state);

		void updateUser(int userId, MyUtil::Str2StrMap props);
		void updateUserDesc(int userId, MyUtil::Str2StrMap props);
		void updateUserNetwork(int userId, MyUtil::Str2StrMap props);

		//针对具体表作更新通知
		//user_born
		void insertUserBorn(int userId, MyUtil::Str2StrMap props);		
		void updateUserBorn(int userId, MyUtil::Str2StrMap props);

		//user_stage
		void insertUserStage(int userId, MyUtil::Str2StrMap props);		
		void updateUserStage(int userId, MyUtil::Str2StrMap props);

		//user_doing
		void insertUserDoing(int userId, MyUtil::Str2StrMap props);		
		void updateUserDoing(int userId, MyUtil::Str2StrMap props);

		//user_state
		void insertUserState(int userId, MyUtil::Str2StrMap props);		
		void updateUserState(int userId, MyUtil::Str2StrMap props);		

		//user_names
		void insertUserNames(int userId, MyUtil::Str2StrMap props);
		void updateUserNames(int userId, MyUtil::Str2StrMap props);

		//user_passport
		void insertUserPassport(int userId, MyUtil::Str2StrMap props);
		void updateUserPassport(int userId, MyUtil::Str2StrMap props);

		//user_config
		void insertUserConfig(int userId, MyUtil::Str2StrMap props);
		void updateUserConfig(int userId, MyUtil::Str2StrMap props);

		//user_url
		void insertUserUrl(int userId, MyUtil::Str2StrMap props);
		void updateUserUrl(int userId, MyUtil::Str2StrMap props);

		//user_right
		void insertUserRight(int userId, MyUtil::Str2StrMap props);
		void updateUserRight(int userId, MyUtil::Str2StrMap props);

		//user_time
		void insertUserTime(int userId, MyUtil::Str2StrMap props);
		void updateUserTime(int userId, MyUtil::Str2StrMap props);

		//user_contact
		void insertUserContact(int userId, MyUtil::Str2StrMap props);
		void updateUserContact(int userId, MyUtil::Str2StrMap props);

		//user_fav
		void insertUserFav(int userId, MyUtil::Str2StrMap props);
		void updateUserFav(int userId, MyUtil::Str2StrMap props);

		void updateUserRegion(int userId, MyUtil::Str2StrMap props);
		void updateUserWorkplace(int userId, MyUtil::Str2StrMap props);
		void updateUserElementarySchool(int userId, MyUtil::Str2StrMap props);
		void updateUserJuniorHighSchool(int userId, MyUtil::Str2StrMap props);
		void updateUserHighSchool(int userId, MyUtil::Str2StrMap props);
		void updateUserCollege(int userId, MyUtil::Str2StrMap props);
		void updateUserUniversity(int userId, MyUtil::Str2StrMap props);
    };
};
};
};
};
#endif

