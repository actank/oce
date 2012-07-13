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
         
        sequence<string> StrSeq;

        dictionary<string, string> Str2StrMap;

        enum  BusinessType  {
          Share, Blog, State, Feed, Letter
        };
        
        // enum Operation {
        //  Add, Update, Delete
        // };

        struct IndexUpdateData  {
          BusinessType type;
          Str2StrMap index;
          short operationType; //include three operation types : 0:add, 1:update and 2:delete
        };
        
        sequence<IndexUpdateData> IndexUpdateDataSeq;

         interface IndexUpdateLogic  {
           void update(IndexUpdateData info);
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
