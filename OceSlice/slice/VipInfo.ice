#ifndef _VIP_INFO_ICE
#define _VIP_INFO_ICE

#include <Util.ice>

#include <ObjectCache.ice>
module xce
{
module vipinfo
{

	class VipMemberData
	{
		int id;
		int type;
		int status;
		int iconId;
		int level;
		int intInfo1;
		int intInfo2;
		string charInfo1;
		string charInfo2;
	};

	//相比VipMemberData，增加hatId
	class VipMemberDataN
	{
		int id;
		int type;
		int status;
		int iconId;
		int hatId;
		int level;
		int intInfo1;
		int intInfo2;
		string charInfo1;
		string charInfo2;
	};

	class VipFullData
	{
		int id;
		VipMemberData memberData;
		string hideUrl;
	};

	class VipFullDataN
	{
		int id;
		VipMemberDataN memberData;
		string iconUrl;
		string hatUrl;
		int hatStatus;
	};

	//旧的IconData，包括memberInfo和iconInfo
	class VipIconData
	{
		int id;
		int type;
		int iconId;
		string hideUrl;
		int level;
		int status;
	};

	class Icon2UrlData
	{
		int iconId;
		string hideUrl;
	};

	//Vip用户基本信息，相对VipMemberData较小，较常用
	class VipMemberMetaData
	{
		int id;
		int type;
		int level;
		int status;
	};

	class HatData
	{
		int hatId;
		int status;
		string hideUrl;
	};

	//VIP用户Feed信息，提取共同信息到VipMemberMetaData中
	class VipFeedData {
		VipMemberMetaData member;
		Icon2UrlData icon;
		HatData hat;
	};

	sequence<VipMemberData> VipMemberDataSeq;
	sequence<VipMemberDataN> VipMemberDataNSeq;
	sequence<VipIconData> VipIconDataSeq;
	sequence<VipFeedData> VipFeedDataSeq;
	sequence<VipFullData> VipFullDataSeq;
	sequence<VipFullDataN> VipFullDataNSeq;
	dictionary<int,VipMemberDataSeq> VipMemberDataSeqMap;
	dictionary<int,VipMemberDataNSeq> VipMemberDataNSeqMap;
	dictionary<int,VipMemberData> VipMemberDataMap;
	dictionary<int,VipMemberDataN> VipMemberDataNMap;
	dictionary<int,VipFullDataSeq> VipFullDataSeqMap;
	dictionary<int,VipFullDataNSeq> VipFullDataNSeqMap;
	dictionary<int,VipIconDataSeq> VipIconDataSeqMap;
	dictionary<int,VipFeedDataSeq> VipFeedDataSeqMap;
	dictionary<int,Icon2UrlData> Icon2UrlDataMap;
	dictionary<int,HatData> HatDataMap;

	class VipMemberMemData
	{
		int id;
		VipMemberDataMap data;
	};

	class VipMemberMemDataN
	{
		int id;
		VipMemberDataNMap data;
	};

	class VipMemberDataReader
	{
		VipMemberDataSeqMap getVipMemberDataSeqMap( MyUtil::IntSeq ids , MyUtil::IntSeq types );
		VipMemberDataNSeqMap getVipMemberDataNSeqMap( MyUtil::IntSeq ids , MyUtil::IntSeq types );
		void setVipMemberData( int id, MyUtil::Str2StrMap props );
		void setData( MyUtil::ObjectResult data );
		void setValid( bool flag );
	};

	class VipMemberDataWriter
	{
		void createVipMemberData( MyUtil::Str2StrMap props );
		void setVipMemberData( int id, MyUtil::Str2StrMap props );
		void reload( MyUtil::IntSeq ids );
		void setValid( bool flag );
	};

	class Icon2UrlDataReader
	{
		MyUtil::Int2StrMap getIcon2UrlDataMap( MyUtil::IntSeq iconIds );
		void setIcon2UrlData( int id, MyUtil::Str2StrMap props );
		void setData( MyUtil::ObjectResult data );
		void setValid( bool flag );
	};

	class Icon2UrlDataWriter
	{
		void createIcon2UrlData( MyUtil::Str2StrMap props );
		void setIcon2UrlData( int id, MyUtil::Str2StrMap props );
		void reload( MyUtil::IntSeq ids );
		void setValid( bool flag );
	};

	class HatDataWriter
	{
		void createHatData( MyUtil::Str2StrMap props );
		void setHatData( int id, MyUtil::Str2StrMap props );
		void reload( MyUtil::IntSeq ids );
		void setValid( bool flag );
	};

	class HatDataReader
	{
		HatDataMap getHatDataMap( MyUtil::IntSeq iconIds );
		void setHatData( int id, MyUtil::Str2StrMap props );
		void setData( MyUtil::ObjectResult data );
		void setValid( bool flag );
	};

	class VipIconDataReader
	{
		//旧接口，只取到icon信息
		VipIconDataSeqMap getVipIconDataSeqMap( MyUtil::IntSeq ids, MyUtil::IntSeq types );
		//新接口，取到icon和hat信息，并把用户信息抽取出来
		VipFeedDataSeqMap getVipFeedDataSeqMap( MyUtil::IntSeq ids, MyUtil::IntSeq types );
	};

	class VipFullDataReader
	{
		VipFullDataSeqMap getVipFullDataSeqMap( MyUtil::IntSeq ids , MyUtil::IntSeq types);
		VipFullDataNSeqMap getVipFullDataNSeqMap( MyUtil::IntSeq ids , MyUtil::IntSeq types );
	};

};
};
#endif
