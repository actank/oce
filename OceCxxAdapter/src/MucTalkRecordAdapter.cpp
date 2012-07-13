#include "MucTalkRecordAdapter.h"
#include <boost/lexical_cast.hpp>
#include <string>
using namespace MyUtil;
using namespace com::xiaonei::talk::muc;
using namespace boost;
using namespace std;
MucTalkRecordManagerPrx MucTalkRecordAdapter::getManager(int id) {
  return locate<MucTalkRecordManagerPrx> (_managers, "M", id, TWO_WAY);
}

MucTalkRecordManagerPrx MucTalkRecordAdapter::getManagerOneway(int id) {
  return locate<MucTalkRecordManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

void MucTalkRecordAdapter::RecordTalkMsg(int userid, int groupid, const string& msg){
	getManagerOneway(groupid)->RecordTalkMsg(userid, groupid, msg);
}
MucTalkMessageSeq MucTalkRecordAdapter::GetRecentlyMsg(int groupid){
	return getManager(groupid)->GetRecentlyMsg(groupid);
}

