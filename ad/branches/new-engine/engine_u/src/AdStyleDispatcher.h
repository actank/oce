#ifndef AD_STYLE_DISPATCHER_H_
#define AD_STYLE_DISPATCHER_H_

#include <IceUtil/Shared.h>
#include <boost/algorithm/string/regex.hpp>
#include "AdEngine.h"
#include "AdStruct.h"
#include "AdConfig.h"
#include "../../util/src/AdCommonUtil.h"
//#include "AdEffect.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace boost;


class AdStyleDispatcher : public MyUtil::Singleton<AdStyleDispatcher> {
public:
	AdStylePtr Dispatch(Ice::Long zoneId,UserProfile &user );
	/**
	 *对象生成后首先调用，定时更新AdExperiment的数据
	 */
	void Init();
	class UpdateCheckTimer : public MyUtil::Timer {
	  public:
	    UpdateCheckTimer() :
	      Timer(1 * 60 * 1000) {
	    }
	    ;
	    virtual void handle();
	  };
private:
	map<Ice::Long,vector<Ice::Long> >  Update();

	/**
	 *vector<Ice::Long>保存的是广告位样式预处理的结果，vector的大小固定为100
	 */
	map<Ice::Long,vector<Ice::Long> > zone2style_;
  IceUtil::RWRecMutex mutex_;
};
}
}
#endif // AD_STYLE_DISPATCHER_H_
