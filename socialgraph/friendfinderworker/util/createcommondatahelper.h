#ifndef CREATECOMMONDATAHELPER_H_
#define CREATECOMMONDATAHELPER_H_

#include <vector>

#include "socialgraph/friendfinderworker/algorithm/commonfriendexecuter.h"
#include "socialgraph/friendfinderworker/algorithm/commonfactory.h"
#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"
#include "item.pb.h"

namespace xce {
namespace socialgraph {

const static int CF_SAVE_THRESHOLD_DISTCACHE = 24;
const static int CACHE_SHARES_SIZE = 10;

//----------------------------------------------------------------------------------------------------

class CreateCommonDataHelper {
public:
  static void GetFromDb(int userid, std::vector<unsigned char>& binary_data);
  static void CalCommonFriends(int userid, std::vector<unsigned char>& binary_data);
  static void GenerateCacheData(const CommonFriendSeq& comm_list, std::vector<unsigned char>& binary_data);
  static void Print(const CommonFriendSeq& comm_list);
  static void Print(const std::vector<unsigned char>& binary_data);
  static void Test(int hostid);
	static long GetCreateDataTime(int hostid);
};

}
}

#endif
