#ifndef _BASICCLIENTCONDITION_ICE
#define _BASICCLIENTCONDITION_ICE

#include <Util.ice>

module com {
  module xiaonei {
    module search5 {
      module query {
        module client {
          enum Scope {
          	MYSELF, FRIENDS, MYSELFFRIENDS, ALL
          };
	        // -1 : Test
          // 0 : Blog
          // 1 : State
          // 2 : Share
          // 3 : NewsFeed
          // 4 : Letter
          // 5 : LBS
          struct BasicClientCondition {
          	Scope who;
          	int userId;// 搜索执行者
          	int openDay; //搜索几天内的数据
            int searchType;// 业务类型
            MyUtil::Str2StrMap 	condition;// 与条件
           	MyUtil::StringArray requiredFields;// 需要返回的数据字段名
            MyUtil::IntSeq friends;//好友ID
            string queryCacheKey;
            string personalCacheKey;
            bool isNeededSort;
          };
        };
      };
    };
  };
}; 
#endif
