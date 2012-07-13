#ifndef ADRECOMMENDATIONHANDLER_ICE
#define ADRECOMMENDATIONHANDLER_ICE
#include <Util.ice>
module xce {
  module ad {
    dictionary<long, float> AdResult;
    dictionary<long, double> AdResultE;
    dictionary<long, double> AdKNN;
    sequence<long> AdSeq;
  sequence<int>UserSeq;

    class AdRecommendationHandler {
        AdResult GetAds(int userid); 
        AdResultE GetRecommend(int userid); 
        void Click(int userid, long groupid);
        void ClickNew(int userid, long groupid, bool flag);
  AdSeq GetUserItem(int userid); 
  double GetUserDistance(int userid1, int userid2);
  UserSeq GetGidUser(long groupid);
  bool AddUserListL(int userid,int sindex);
  bool AddUserListH(int userid,int sindex);
  void CleanUserClick(int userid);
  AdKNN GetKNN(long groupid);
    };
  };
};
#endif
