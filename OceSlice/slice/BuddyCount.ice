#ifndef __BUDDY_COUNT_ICE__
#define __BUDDY_COUNT_ICE__

#include <Util.ice>

module xce
{
module buddy 
{
  class BuddyCounter
  {
    MyUtil::IntSeq types;
    MyUtil::IntSeq values;
    int timestamp;
  };
	
  interface BuddyCountManager
  {
    void inc(int userId, int type, int step);
    void dec(int userId, int type, int step);
    int get(int userId, int type);
    void set(int userId, int type, int value);
    BuddyCounter getAll(int userId);
    void reload(int userId);
  };
 
};
};

#endif

