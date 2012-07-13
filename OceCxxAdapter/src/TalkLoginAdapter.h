#ifndef TALKLOGINADAPTER_H_
#define TALKLOGINADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk{
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class TalkLoginAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkLoginAdapter> {
public:
	TalkLoginAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	PassportPtr verify(const string& email, const string& password);
	void bind(const JidPtr& j);
	void unbind(const JidPtr & j);

	size_t getCluster(){
		return _cluster;
	};

protected:
	virtual string name() {
		return "TalkLogin";
	}
	virtual string endpoints() {
		return "@TalkLogin";
	}
	virtual size_t cluster() {
		return 10;
	}

	LoginHandlerPrx getManager(int id);
	LoginHandlerPrx getManagerOneway(int id);

	vector<LoginHandlerPrx> _managersOneway;
	vector<LoginHandlerPrx> _managers;
};

}
;
}
;


#endif /*TALKLOGICADAPTER_H_*/
