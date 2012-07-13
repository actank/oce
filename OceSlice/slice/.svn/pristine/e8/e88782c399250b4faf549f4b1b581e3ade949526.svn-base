#ifndef UESR_DANGER_ICE
#define USER_DANGER_ICE

module xce
{
module userdanger
{
  const int TEST = 0;
  const int NOTFEED = 1;
  const int NOTACCEPTBUDDYAPPLICATION = 2;
  const int ONLYSEENBYFRIEND = 3;
  const int ONLYSEENBYSELF = 4;
  const int UGCONLYSEENBYSELF = 5;
  const int UGCALLCHECK = 6;
  const int SEARCHSHIELDING = 7;
  const int MAILSUBSCRIPTIONSHIELDING = 8;
  const int UGCAUDITFIRST = 9;
  const int MINIGROUPHIDE = 10;
  const int MINISITEHIDE = 11;
 
  dictionary<int, bool> Int2Bool;
  ["java:type:java.util.ArrayList"] sequence<int> IntSeq;
  dictionary<int, IntSeq> Int2IntSeq;
  interface UserDangerManager
  {
    void setTypeOn(int userid, int type);
    void setTypeOff(int userid, int type);
    bool isType(int userid, int type);
    Int2Bool isTypes(IntSeq userids, int type);
    IntSeq getUserIds(int type);
    Int2IntSeq getClientCache();

    void setValid(bool valid);
    bool isValid();
  };

};
};


#endif
