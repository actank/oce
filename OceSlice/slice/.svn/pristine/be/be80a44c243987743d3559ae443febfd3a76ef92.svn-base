#ifndef __SHARESEARCH_ICE__
#define __SHARESEARCH_ICE__

#include <Util.ice>
module xce
{
module sharesearch
{
	struct UrlInfo
	{
		string urlmd5;
		int userid;
		long shareid;
	};
	sequence<UrlInfo> UrlInfoSeq;
	
	struct ShareInfo
	{
		int userid;
		long shareid;
	};
	sequence<ShareInfo> ShareInfoSeq;

	struct UrlDetailInfo
	{
		string urlmd5;
		string url;
		int userid;
		long shareid;
		string title;
		string summary;
		int viewcount;
		int sharecount;
		long creationdate;
	};
	sequence<UrlDetailInfo> UrlDetailInfoSeq;

  struct ShareIndexInfo  {
    string md5;          //md5
    string url;          //url
    int type;            //type
    long resourceId;     //resource_id
    int resourceUserId;  //resource_user_id
    string thumbUrl;     //thumb_url
    string title;        //title
    string summary;      //summary
    int commentCount;    //comment_count
    int viewCount;       //view_count
    int zanCount;        //zan_count
    int shareCount;      //share_count
    int collectionCount; //collection_count
    int status;          //status
    long creationData;   //creation_date
    long shareId;       //id in share table
    int shareUserId;    //user_id in share table
  };
  sequence<ShareIndexInfo> ShareIndexInfoSeq;
  interface ShareIndexLogicManager
  {
    void update(ShareIndexInfo info, bool isNew, bool newState);
    bool isValid();
    void setValid(bool newState);
    void setRecovery(bool newState);//  ice file not add
    bool isRecovery();
    void setRebuild(bool newState);
    bool isRebuild(); 
  };

	class ShareLogic
	{
		void create(UrlDetailInfo info);
		void share(UrlInfo info);
        bool isValid();
        void setValid(bool newState);
	};
    class ShareCache
    {	
		ShareInfoSeq getAll(string urlmd5,int begin,int limit);
		ShareInfoSeq getCommonFriends(string urlmd5, int userId);			
		void setBatch(UrlInfoSeq urlSeq);
        bool isValid();
        void setValid(bool newState);
		void sortAll();
    };
};
};
#endif

