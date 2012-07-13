#ifndef _NOTIFY_DISPATCHERN_ADAPTER_H_
#define _NOTIFY_DISPATCHERN_ADAPTER_H_

#include "AdapterI.h"
#include "Notify.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "TaskManager.h"


namespace xce {
namespace notify {


using namespace MyUtil;

class NotifyDispatcherNAdapter : public AdapterI,
									 public AdapterISingleton<MyUtil::NotifyChannel, NotifyDispatcherNAdapter>
{

protected:
	virtual string name() {
		return "NotifyDispatcherN";
	}
	virtual string endpoints() {
		return "@NotifyDispatcherN";
	}
	virtual size_t cluster() {
		return 5;
	}

public:
	NotifyDispatcherNAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}

	bool reloadSchema(){
    // TODO : 所有 gate reload schema
    for(size_t i = 0; i < cluster(); ++i)
    {
		  getManager(i)->reloadSchema();
    }
    return true;
	}

	void dispatchN(Ice::Long server_index, Ice::Long notify_id, bool store, const MyUtil::Str2StrMap& props){

	  getManagerOneway(int(server_index%100))->dispatchN(notify_id, store, props);
	}

	void QuitChat(int uid, const GroupIndexSeq & groups){
	  size_t s = cluster();
	  map<Ice::Long,GroupIndexSeq> gm;
	  for(unsigned i = 0; i != groups.size();++i){
	    Ice::Long source = groups.at(i).source;
	    gm[source % s].push_back(groups.at(i));
	  }
	  for(map<Ice::Long,GroupIndexSeq>::iterator it = gm.begin();it != gm.end();++it){

	    getManagerOneway(it->first)->QuitChat(uid,it->second);
	  }
	}

	void JoinChat(const vector<int> & uids, const GroupIndex & gindex){
		getManagerOneway(gindex.source % 1000)->JoinChat(uids,gindex);
	}
private:

	NotifyDispatcherNPrx getManager(int id) {
	  return locate<NotifyDispatcherNPrx> (_managers, "M", id, TWO_WAY);
  }

	NotifyDispatcherNPrx getManagerOneway(int id){
		return locate<NotifyDispatcherNPrx> (_managers, "M", id, TWO_WAY);
	}
	vector<NotifyDispatcherNPrx> _managersOneway;
	vector<NotifyDispatcherNPrx> _managers;
};

}
}

#endif // _NOTIFY_DISPATCHERN_ADAPTER_H_
