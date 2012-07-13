#ifndef __BLOGSEARCH_ICE__
#define __BLOGSEARCH_ICE__

#include <Util.ice>
module com
{
  module xiaonei
  {
    module search5
    {
      module logicserver
      {
         struct BlogIndexData  {
           long id;
           int owner;
           string name;
           string headurl;
           string tinyurl;
           string body;
           int commentCount;
           string title;
           string time;
           string uptime;
           int viewCount;
           string month;
           short blogControl;
           int cmdCount;
           int univ;
           int blogPicId;
           short draftFlag;
           int stage;
           int type;
           string originalUrl;
           int categoryId;
           short passwordProtected;
           string password;
           short operationType;//include three operation types :add,update and delete;
         };

         sequence<BlogIndexData> BlogIndexDataSeq;

         interface BlogUpdateLogic  {
           void update(BlogIndexData info);
           BlogIndexDataSeq  pushIndexData(int limit);
           void setValid(bool newState);
           bool isValid();
           void setRecovery(bool newState);
           bool isRecovery();
         };
      };
    };
  };
};
#endif
