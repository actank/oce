#ifndef _CARD_CACHE_ICE
#define _CARD_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
  module cardcache
  {
    const byte FIELDADDRESS = 0;
    const byte FIELDQQ = 1;
    const byte FIELDMOBILE = 2;
    const byte FIELDEMAIL = 3;
    const byte FIELDMSN = 4;
    const byte FIELDEXTRA = 5;
    const byte FIELDPRIVACY = 6;

    const int ADD = 0;
    const int RECEIVE = 1;
    const int ACCEPT =2;
    const int REMOVE = 3;

    class CardData
    {
      string qq;
      string mobile;
      string address;
      string email;
      string msn;
      string extra;
    };

    class CardInfo extends CardData
    {
      int userId;
      string privacy;
    };
    sequence<CardInfo> CardInfoSeq;

    class PartOfCardInfo
    {
      int userId;
      string qq;
      string mobile;
      string privacy;
    };
    sequence<PartOfCardInfo> PartOfCardInfoSeq;

    class PartInfo extends PartOfCardInfo
    {
      string name;
    };
    sequence<PartInfo> PartInfoSeq;

    class PartNewInfo extends PartInfo
    {
      int type;
    };
    sequence<PartNewInfo> PartNewInfoSeq;

    class RelationInfo 
    {
      int userId;
      int friendId;
      int requestFlag;
    };
    sequence<RelationInfo> RelationInfoSeq;

    class RelationInfoNew
    {
      int userId;
      int friendId;
      int requestFlag;
      int recordTime;
    };
    sequence<RelationInfoNew> RelationInfoNewSeq;

    class ImportValue 
    {
      byte importWay;
      string importDetail;
      string name;
    };
    sequence<ImportValue> ImportValueSeq;

    class ImportValueNew extends ImportValue
    {
      int id;
    };
    sequence<ImportValueNew> ImportValueNewSeq;

    class ImportValueII extends ImportValueNew
    {
      byte subscribeFlag;
    };
    sequence<ImportValueII> ImportValueIISeq;

    class ImportInfo extends ImportValue
    {
      int userId;
      string msn;
      string email;
      string mobile;
      string qq;
      string address;
      string extra;
      int requestCount;
    };
    sequence<ImportInfo> ImportInfoSeq;

    class ImportIdImportDetail
    {
      int id;
      string importDetail;
    };
    sequence<ImportIdImportDetail> ImportIdImportDetailSeq;

    class ImportIdSubscribeFlag
    {
      int id;
      byte subscribeFlag;
    };
    sequence<ImportIdSubscribeFlag> ImportIdSubscribeFlagSeq;

    class ImportInfoNew extends ImportInfo
    {
      int id;
      byte subscribeFlag;
    };
    sequence<ImportInfoNew> ImportInfoNewSeq;

    class ImportEmail
    {
      int id;
      int userId;
      string email;
    };
    sequence<ImportEmail> ImportEmailSeq;

    dictionary<byte, string> Byte2StrMap;

    class AllRelation
    {
      MyUtil::IntSeq acceptList;
      MyUtil::IntSeq receiveList;
      MyUtil::IntSeq addList;
    };

    interface CardInfoCache
    {
      CardInfoSeq get(MyUtil::IntList userIds);
      PartOfCardInfoSeq getPart(MyUtil::IntList userIds);
      bool set(CardInfoSeq CardDatas);
      void setProperty(int userId, MyUtil::Str2StrMap props);
      void remove(MyUtil::IntSeq userIds);
      string getPrivacy(int userId);
      bool setPrivacy(int userId, string privacy);
      bool isValid();
      void setValid(bool newState);

    };

    interface CardRelationCache
    {
      MyUtil::IntSeq get(int userId, int begin, int limit, int requestFlag);
      int getSize(int userId, int requestFlag);
      int set(RelationInfo relation);
      void setBatch(RelationInfoSeq relationInfos);
      void remove(int userId, int friendId);
      int getRelation(int userId, int friendId);
      AllRelation getAllRelation(int userId);

      MyUtil::IntSeq getReceive(int userId, int begin, int limit);
      bool ignoreReceive(int userId, MyUtil::IntSeq friendIds);
      bool ignoreAll(int userId);

      MyUtil::Int2IntSeqMap getAccept(int time, int lastId, int limit);
    };

    interface CardImportCache
    {
      ImportInfoSeq get(int userId, int limit);
      ImportInfo getDetail(int userId,byte importWay, string importDetail);
      ImportValueSeq getList(int userId);
      bool set(ImportInfoSeq importInfos);
      bool load(ImportInfoSeq importInfos);
      void remove(ImportInfoSeq importInfos);
      bool isContact(int userid, string email, int etype);
      ImportInfoNewSeq getNew(int userId, int limit);
      ImportInfoNew getDetailNew(int userId,byte importWay, string importDetail);
      ImportValueNewSeq getListNew(int userId);
      bool setNew(ImportInfoNewSeq importInfos);
      bool loadNew(ImportInfoNewSeq importInfos);
      ImportInfoNewSeq loadII(ImportInfoNewSeq importInfos);
      ImportIdImportDetailSeq getIdImportDetail(int userId);
      bool setSubscribeFlag(ImportInfoNewSeq infos);

      ImportValueIISeq getListII(int userId);
      MyUtil::IntSeq getImportUser(int userId);
      ImportEmailSeq getImportEmail(int offset, int limit);
      bool isImported(int userId);
      MyUtil::IntSeq friendsImported(MyUtil::IntSeq friendIds);
      ImportInfoNew getDetailByEmail(int userId, string email);
      MyUtil::IntSeq getImportUserId(int begin, int limit);
    };
  };
};

#endif
