#ifndef __CONTENTSEARCHRMI_ICE__
#define __CONTENTSEARCHRMI_ICE__
#include <ContentSearch.ice>
#include <Util.ice>
module com
{
  module xiaonei
  {
    module search5
    {
      module logicserver
      {
        interface RmiLogic  {
          void update(int mod, IndexUpdateDataSeq infos);
        };
      };
    };
  };
};
#endif

