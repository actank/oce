#include "PhotoMemoryAdapter.h"

using namespace MyUtil;
using namespace xce::feed;
using namespace std;


PhotoMemoryManagerPrx PhotoMemoryAdapter::getManager(int id) {
  return locate<PhotoMemoryManagerPrx> (_managers, "M", id, TWO_WAY);
}

PhotoMemoryManagerPrx PhotoMemoryAdapter::getManagerOneway(int id) {
  return locate<PhotoMemoryManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}


PhotoContentSeq PhotoMemoryAdapter::GetPhotoContentSeq(int userid, int begin, int end) {
  return getManager(userid)->GetPhotoContentSeq(userid, begin, end);
}

void PhotoMemoryAdapter::SetRead(int userid, long photoid) {
  getManagerOneway(userid)->SetRead(userid, photoid);
}





