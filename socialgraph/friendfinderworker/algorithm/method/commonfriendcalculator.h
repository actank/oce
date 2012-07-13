#ifndef COMMON_FRIEND_CALCULATOR_H_
#define COMMON_FRIEND_CALCULATOR_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include "base/ptime.h"
#include "base/logging.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/friendfinderworker/algorithm/method/friendutils.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace socialgraph {

struct CommonFriend {
  CommonFriend() {       
    userId = 0;
    weight = 0;
    sharesCount = 0;     
  }

  int userId;
  int weight;            
  int sharesCount;
  std::vector<int> shares;  
};

typedef std::vector<CommonFriend> CommonFriendSeq;

//--------------------------------------------------------------------

class CommonFriendCalculator {
public:
	CommonFriendCalculator() {
		userid_ = 0;
	}
	virtual ~CommonFriendCalculator() {}

	//获取二度好友的入口
	virtual void GetComm(int userid, CommonFriendSeq& comm);

	virtual bool Hook() {
		return true;
	}
	virtual void CalComm(CommonFriendSeq& comm) = 0;
	virtual void Display(const CommonFriendSeq& comm) const = 0;

protected:
	int userid_;
	std::vector<int> friendlist_;
	std::vector<int> applylist_;
	std::vector<int> blocklist_;
	std::vector<int> commonblock_;

	std::map<int, std::vector<int> > ffmap_;
	std::map<int, std::vector<int> > afmap_;

private:
	void Initialize(int userid);
};

typedef boost::shared_ptr<CommonFriendCalculator> CommonFriendCalculatorPtr;

}
}

#endif
