#include "socialgraph/friendfinderworker/util/createcommondatahelper.h"
#include "socialgraph/friendfinderworker/util/commondbhelper.h"

#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "util/cpp/InvokeMonitor.h"
#include "util/cpp/TimeCost.h"

namespace xce {
namespace socialgraph {

void CreateCommonDataHelper::GetFromDb(int userid, std::vector<unsigned char>& binary_data) {
  using namespace com::xiaonei::xce;

  CommonFriendSeq comm_friends;
  CommonDbHelper::GetCommonsFromDb(userid, comm_friends);   //从common_friends表中获取该用户数据
  
  if (comm_friends.empty()) {   //为空即返回
    return;
  }
  
  //生成二进制数据
  GenerateCacheData(comm_friends, binary_data);
}

void CreateCommonDataHelper::CalCommonFriends(int userid, std::vector<unsigned char>& binary_data) {
  using namespace com::xiaonei::xce;

  CommonFriendSeq comm;
  CommonFriendExecuter executer;
  executer.SetCommonFriendInstance(CommonFactory::GetInstance().Create(KMerge));
  executer.GetComm(userid, comm);

  if (comm.empty()) {   //计算结果为空，则直接返回
    return;
  }
  CommonDbHelper::SetCommonsToDb(userid, comm);   //向common_friends表中插入数据

 //生成二进制数据
 GenerateCacheData(comm, binary_data);
}

void CreateCommonDataHelper::GenerateCacheData(const CommonFriendSeq& comm_list, std::vector<unsigned char>& binary_data) {
  ::xce::socialgraph::Items result;
  for (CommonFriendSeq::const_iterator iter = comm_list.begin(); (iter != comm_list.end())
      && (iter < comm_list.begin() + CF_SAVE_THRESHOLD_DISTCACHE); ++iter) {
    ::xce::socialgraph::Item* item = result.add_items();

    item->set_id(iter->userId);
    item->set_field(iter->sharesCount);
    for (MyUtil::IntSeq::const_iterator it = iter->shares.begin();
        it != iter->shares.end(); ++it) {
      if (item->fields_size() >= CACHE_SHARES_SIZE) {
        break;
      }
      item->add_fields(*it);
    }
  }

  result.set_timestamp(time(NULL));

  int byte_size = result.ByteSize();
  binary_data.resize(byte_size);
  result.SerializeToArray(binary_data.data(), byte_size);
}

long CreateCommonDataHelper::GetCreateDataTime(int hostid) {
	return CommonDbHelper::GetCreateDataTime(hostid);
}

void CreateCommonDataHelper::Print(const CommonFriendSeq& comm_list) {
  for (CommonFriendSeq::const_iterator iter = comm_list.begin();
      iter != comm_list.end(); ++iter) {
		MCE_INFO("userId(" << iter->userId << ") weight(" << iter->weight << ") sharesCount(" << iter->sharesCount << ")");
    std::ostringstream oss;
    for (MyUtil::IntSeq::const_iterator it = iter->shares.begin();
        it != iter->shares.end(); ++it) {
      oss << *it << " ";
    }
		MCE_INFO(oss.str() << "\n");
  }
}
void CreateCommonDataHelper::Print(const std::vector<unsigned char>& binary_data) {
  xce::socialgraph::Items result;
  result.ParseFromArray(binary_data.data(), binary_data.size());
  for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator iter = result.items().begin();
      iter != result.items().end(); ++iter) {
		MCE_INFO("userId(" << iter->id() << ") sharesCount(" << iter->field() << ") flieds size(" << iter->fields_size() << ")");

    std::ostringstream oss;
    for (::google::protobuf::RepeatedField< ::google::protobuf::int32 >::const_iterator it = iter->fields().begin();
        it != iter->fields().end(); ++it) {
			oss << *it << " ";
    }
		MCE_INFO(oss.str() << "\n");
  }
}

void CreateCommonDataHelper::Test(int hostid) {
  using namespace com::xiaonei::xce;

  CommonFriendSeq commFriends;
  CommonDbHelper::GetCommonsFromDb(hostid, commFriends);    //从db中取二度好友数据

	MCE_INFO("[CreateCommonDataHelper] Test hostid:" << hostid << " commFriends size:" << commFriends.size());
  Print(commFriends);
}

}
}
