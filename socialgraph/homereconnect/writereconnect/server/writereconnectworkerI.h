#ifndef _WRITERECONNECTWORKER_H_
#define _WRITERECONNECTWORKER_H_

#include "socialgraph/socialgraphutil/socialgraphworkerI.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <set>           

#include "IceExt/src/ServiceI.h"    
#include "DbCxxPool/src/QueryRunner.h"
#include "UtilCxx/src/TaskManager.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "item.pb.h"

namespace xce {
namespace socialgraph {

const static int LEVEL_REMOVE = 4;

//---------------------------------------------------------------------------------------------------------------

class WriteReconnectWorkerI : virtual public SocialGraphWorkerI,
		virtual public MyUtil::Singleton<WriteReconnectWorkerI> {
public:
	WriteReconnectWorkerI() {}

  virtual void Report(const Info&, const Ice::Current&);
  virtual void PreLoad(int, bool, const Ice::Current&);
  virtual void Rebuild(int, const Ice::Current&);

private:
	friend class MyUtil::Singleton<WriteReconnectWorkerI>;
};

//---------------------------------------------------------------------------------------------------------------
//删除线程
class RemoveTask: virtual public MyUtil::Task {
public: 
  RemoveTask(Ice::Int userId, Ice::Int friendId, const MyUtil::IntSeq& blockIds, Ice::Int level=LEVEL_REMOVE):
    Task(level), _userId(userId), _friendId(friendId), _blockIds(blockIds) { };
  virtual void handle();

private:
	void RemoveDistCache();
	
  Ice::Int _userId;
  Ice::Int _friendId; 
  MyUtil::IntSeq _blockIds; 
};

//---------------------------------------------------------------------------------------------------------------

class TableName {
public:
	static std::string recommend_friends_table(int id) {
		ostringstream oss;
		oss << "recommend_friends_" << (id % 10);	
		return oss.str();
	}
	
	static std::string recommend_block_table(int id) {
		return "recommend_friends_block";	
	}
};

}
}

#endif
