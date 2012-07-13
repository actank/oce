#ifndef MY_TASK_H_
#define MY_TASK_H_

#include <map>
#include <vector>
#include <sstream>
#include <fstream>

#include "UtilCxx/src/TaskManager.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "item.pb.h"

#include "socialgraph/homereconnect/writereconnect/update/relationbatchhelper.h"
#include "socialgraph/homereconnect/writereconnect/update/writereconnectitem.h"

namespace xce {
namespace task {

static const int TASK_LEVEL_SET_CACHE = 1;
static const int TASK_LEVEL_CREATE_WRITE = 2;
static const int FRIENDS_NUM_THRESHOLD = 50;

//-----------------------------------------------------------------------------------------------------------
//批量向cache中写数据
class SetDistCacheByWriteReconnectTask : public MyUtil::Task {
public:
	SetDistCacheByWriteReconnectTask(int userid, const std::vector<int>& write_reconnect_list, ::Ice::Int level = TASK_LEVEL_SET_CACHE):
		MyUtil::Task(TASK_LEVEL_SET_CACHE), userid_(userid), write_reconnect_list_(write_reconnect_list) { }

	virtual void handle();

private:
	int userid_;
	std::vector<int> write_reconnect_list_;
};

//-----------------------------------------------------------------------------------------------------------
//创建数据线程
class CreateWriteReconnectTask : public MyUtil::Task {
public:
	CreateWriteReconnectTask(int userid, const std::vector<int>& friend_list, ::Ice::Int level = TASK_LEVEL_CREATE_WRITE):
		MyUtil::Task(level), userid_(userid), friend_list_(friend_list) { }

	virtual void handle();

	static WriteReconnectItem* g_writereconnect_ptr;

private:
	int userid_;
	std::vector<int> friend_list_;
};

}
}

#endif
