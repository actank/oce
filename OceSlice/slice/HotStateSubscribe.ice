#ifndef _HOTSTATESUBSCRIBE_ICE__
#define _HOTSTATESUBSCRIBE_ICE_
#include <Util.ice>
#include <ContentSearch.ice>

module com  {
  module xiaonei  {
    module search  {
      module state  {
        
        interface HotStateSubscribe  {
          void update(com::xiaonei::search5::logicserver::IndexUpdateData info);
          bool test(string pattern, string inputState);
          ["java:type:java.util.ArrayList"] MyUtil::Str2StrMapSeq getNewState(string type, int limit); 
          bool subscribe(string type,string pattern,int maxSize, string passwd);//注册订阅业务类型
          bool unSubscribe(string type);
				  ["java:type:java.util.ArrayList"] MyUtil::Str2StrMapSeq showTypeInfos(); 
	  			MyUtil::Str2StrMap showTypeInfo(string type); 
	
        }; 
 
      };
    };
  };
};

#endif
