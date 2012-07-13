#ifndef ADUNION_ICE
#define ADUNION_ICE

#include <Util.ice>
module xce{
module ad{ 

class AdUnionGate {
  bool Reload();
  bool ReloadById(int id);
};

class AdUnionAdmin {
  bool Reload();
};

};
};

#endif
