#ifndef __FLUSHSEARCHDATA_ICE__
#define __FLUSHSEARCHDATA_ICE__

#include <ContentSearch.ice>
module com
{
  module xiaonei
  {
    module search5
    {
      module logicserver
      {
         interface FlushSearchDataLogic  {
           void update(IndexUpdateDataSeq data);
           void setValid(bool newState);
           bool isValid();
         };
      };
    };
  };
};
#endif
