#include <Util.ice>

#ifndef __SEARCHRELATIONLOGIC_
#define __SEARCHRELATIONLOGIC_

module com
{
  module xiaonei
  {
    module search5
    {
      module searchrelation
      {

        enum Operation {
          Add, Delete
        };

        interface SearchRelationLogic  {
        
           void pushFriendShip(int fromUserId, int toUserId, Operation op);

        };
      };
    };
  };
};
#endif

