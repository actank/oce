#ifndef _XCE_PERSONALINFO_PERSONAL_INFO_ICE_
#define _XCE_PERSONALINFO_PERSONAL_INFO_ICE_

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module personalinfo
{
  const string CPIID = "ID";
  const string CPILINKSTATUS = "LINKSTATUS";
  const string CPILINK = "LINK";
  const string CPIINVITECODE = "INVITECODE";
  const string CPISPACEALL = "SPACE_ALL";
  const string CPILASTIPLONG = "LASTIP_LONG";
  const string CPISOURCE = "SOURCE";

  //只用于setData
  class LinkViewData
  {
    int userId;
    int linkStatus;
    string link;
  };

  //只用于setData
  class LinkData
  {
    int userId;
    string link;
  };

  //只缓存link和linkStatus
  class LinkViewReader
  {
    MyUtil::Str2StrMap getLinkView(int userId); //找不到异步reload
    MyUtil::Str2StrMap getLinkViewSync(int userId); //找不到同步reload

    void setLinkView(int userId, MyUtil::Str2StrMap props);
    void setData(MyUtil::ObjectResult userData);
    bool isValid();
    void setValid(bool newState);
  };

  //整个表的写都要走此接口，以保证数据同步
  //如果写表数据有link项，同时更新Link2UserIdReader和LinkViewInfoReader
  //如果写表数据有linkStatus而没有link，只更新LinkViewInfoReader
  //reload同时更新Link2UserIdReader和LinkViewInfoReader
  class PersonalInfoWriter
  {
    void setPersonalInfo(int userId, MyUtil::Str2StrMap props);
    void createPersonalInfo(MyUtil::Str2StrMap props);

    void reloadLinkView(int userId);
    bool isValid();
    void setValid(bool newState);
  };

  //缓存有link到id的映射关系
  class Link2UserIdReader
  {
    int find(string link); //只从缓存找，找不到也不查DB
    int locate(string link); //如果miss，从DB取userID
    MyUtil::Str2StrMap getLinkView(string link); //找不到不查DB
    MyUtil::Str2StrMap getLinkViewSync(string link); //找不到同步reload

    //以下接口只提供给中间层内部服务使用
    void add(string link, int userId);
    void remove(string link);
    void setData(MyUtil::ObjectResult userData);
    bool isValid();
    void setValid(bool newState);
  };

};
};
#endif
