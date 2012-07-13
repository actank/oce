#ifndef __FRIENDS_COMPUTE_H__
#define __FRIENDS_COMPUTE_H__

#include <vector>
#include <algorithm>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <Singleton.h>
#include "util/cpp/TimeCost.h"

namespace xce {
  namespace buddy { 


    class FriendsCompute : public MyUtil::Singleton<FriendsCompute>
    {
      public:
        FriendsCompute() {};
        virtual ~FriendsCompute() {};

        bool hasCommonFriend(vector<int>& friendsA, vector<int>& friendsB){
          ostringstream msg;
          msg << "[hasCommonFriend] friendsA.size=" << friendsA.size() << " friendsB.size=" << friendsB.size();
          MyUtil::TimeCost tc = MyUtil::TimeCost::create(msg.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);

          if (friendsA.empty() || friendsB.empty()) {
            return false;
          }

          if (friendsA.front() > friendsB.back() ||
              friendsB.front() > friendsA.back()) {
            return false;
          }

          if (friendsA.front() == friendsB.back() ||
              friendsA.back() == friendsB.front()) {
            return true;
          }

          return friendsA.front() > friendsB.front()? _hasCommonFriend(friendsA,friendsB) : _hasCommonFriend(friendsB,friendsA);
        }

        vector<int> getCommonFriend(vector<int>& friendsA, vector<int>& friendsB){
          ostringstream msg;
          msg << "[FriendsCompute::getCommonFriend] friendsA.size=" << friendsA.size() << " friendsB.size=" << friendsB.size();
          MyUtil::TimeCost tc = MyUtil::TimeCost::create(msg.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);

          if (friendsA.empty() || friendsB.empty()) {
            return vector<int>();
          }

          if (friendsA.front() > friendsB.back() ||
              friendsB.front() > friendsA.back()) {
        	return vector<int>();
          }

          if (friendsA.front() == friendsB.back() ) {
            return vector<int>(friendsA.front());
          }

          if (friendsA.back() == friendsB.front() ) {
            return vector<int>(friendsB.front());
          }

          return friendsA.front() > friendsB.front()? _getCommonFriend(friendsA,friendsB) : _getCommonFriend(friendsB,friendsA);
        }
      private:

        bool _hasCommonFriend(vector<int>& listA, vector<int>& listB){
          vector<int>::iterator iaEnd = listA.end();
          vector<int>::iterator ibEnd = listB.end();
          if (listA.back() > listB.back()) {
            iaEnd = lower_bound(listA.begin(),listA.end(),listB.back());
            if (iaEnd != listA.end() && *iaEnd == listB.back()) {
              return true;
            }
          } else {
            ibEnd = lower_bound(listB.begin(),listB.end(),listA.back());
            if (ibEnd != listB.end() && *ibEnd == listA.back()) {
              return true;
            }
          }
          vector<int>::iterator ia = lower_bound(listA.begin(),iaEnd,listB.front());
          if(ia == iaEnd){
            return false;
          }
          vector<int>::iterator ib = listB.begin();
          while(ia != iaEnd){
            ib = lower_bound(ib,ibEnd,*ia);
            if(ib != ibEnd && *ib == *ia){
              return true;
            } else if ( ib == ibEnd ){
              return false;
            }
            ia = lower_bound(ia,iaEnd,*ib);
            if( ia != iaEnd && *ia == *ib){
              return true;
            }
          }
          return false;
        }

        //_getCommonFriend 前提条件：
        //1. listA和listB增序
        //2. listA.front() >= listB.front()
        vector<int> _getCommonFriend(vector<int>& listA, vector<int>& listB){
          //头部对齐削减
          vector<int>::iterator iaGo = listA.begin();
          vector<int>::iterator ibGo = listB.begin();
          vector<int>::iterator iaEnd = listA.end();
          vector<int>::iterator ibEnd = listB.end();

          vector<int> result(2000, 0);
          vector<int>::iterator ir = result.begin();
          int count = 0;
          while (iaGo != iaEnd && ibGo != ibEnd && count < 2000)
          {
            if (*iaGo < *ibGo) {
              iaGo++;
            } else if (*ibGo < *iaGo) {
              ibGo++;
            }else {
              *ir = *iaGo;
              iaGo++;
              ibGo++;
              ir++;
              count++;
            }
          }
          result.resize(count);
          return result;
        }
    };

    typedef IceUtil::Handle<FriendsCompute> FriendsComputePtr;

  }
}

#endif
