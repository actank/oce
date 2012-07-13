#include "ClusterControllerI.h"
// #include "AdminConsoleI.h"
#include "ServiceI.h"
#include <MonitorAdapter.h>
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

using namespace std;
using namespace MyUtil;
using namespace xce::clustercontroller;
using namespace xce::serverstate;
using namespace xce::clusterstate;
using namespace monitor;
// using namespace xce::adminconsole;

void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();

	// 注册SubjectObserver
	service.getAdapter()->add(&SubjectObserverI::instance(), service.createIdentity("SO", ""));

	// 注册AdminConsole
	// AdminConsoleManagerI::instance().registerComponent("ClusterController", "CC", &ControllerManagerI::instance());
	// service.getAdapter()->add(&AdminConsoleManagerI::instance(), service.createIdentity("ACM", ""));

	// 注册Controller
	service.getAdapter()->add(&ControllerManagerI::instance(), service.createIdentity("M", ""));

	// Controller初始化&开始运行
	Ice::PropertiesPtr prop = service.getCommunicator()->getProperties();
	int intervalFetch =  prop->getPropertyAsIntWithDefault("Fetch.Interval", 5);
	int intervalNotify =  prop->getPropertyAsIntWithDefault("Notify.Interval", 120);
	int cluster =  prop->getPropertyAsInt("Server.Cluster");	// cluster改为在controller设置 -- 090727 by zhanghan

	ControllerManagerI::instance().initialize(cluster, intervalFetch, intervalNotify);
  ServiceMonitorManager::instance().start();
}


void ControllerManagerI::initialize(int cluster, int intervalFetch, int intervalNotify) {

	// 设定cluster
	_cluster = cluster;

	// 设定时间间隔
	_intervalFetch = intervalFetch;
	_intervalNotify = intervalNotify;
	_lastFetch = time(NULL);
	_lastNotify = time(NULL);

	// 初始化结构体
	_clusterState = new ClusterStateInfo();
	_clusterStateOld = new ClusterStateInfo();

	// 取得默认的服务对应关系
	// GetDefaultMap();

	// 开始运行
	start(65535).detach();
}

void ControllerManagerI::run() {

	// 启动时先sleep, 尽量减少服务未注册足够时, 造成收集列表不完整
	sleep(_intervalFetch);

	while ( true ) {
		
		// sleep(_interval);
		sleep(1);

		bool bClusterChanged = false;	// 服务信息是否改动过
		if ( (time(NULL) - _lastFetch) >= _intervalFetch ) {
			try {
				FetchServerState();		
			} catch (...) {
				MCE_WARN("[ControllerManagerI]: Get Server State Exception");
			}
			_lastFetch = time(NULL);
			bClusterChanged = ClusterChanged();
			MCE_INFO("[ControllerManagerI::run] Fetched ServerState");
		}

		if ( (bClusterChanged) || ((time(NULL) - _lastNotify)>=_intervalNotify) ) {
			try { 
				NotifyClusterState();
			} catch (...) {
				MCE_WARN("[ControllerManagerI]: Notify Cluster State Exception");
			}
			MCE_INFO("[ControllerManagerI::run] Notified ClusterState");
			_lastNotify = time(NULL);
		}
	}
}

ClusterStateInfoPtr ControllerManagerI::GetServerState(const Ice::Current& crt) {
  InvokeClient::create(crt);

	return _clusterState; 
}

void ControllerManagerI::FetchServerState(const Ice::Current& crt) {
  InvokeClient::create(crt, "", InvokeClient::INVOKE_LEVEL_DEBUG);
	// 获取每个服务的状态
	ServerStateSeq receives(ServerStateListener::instance().fetch());

	// 备份旧服务列表
	ServerStateMap tmpOld(_clusterState->states);
	_clusterStateOld->states.swap(tmpOld);

	// 决策新服务列表
	ServerStateMap tmpNew;
	map<string, ServerStateInfoPtr> mapBk;	// 备份服务列表
	for (ServerStateSeq::const_iterator it = receives.begin(); 
			it != receives.end(); ++it ) {
		int mod = (*it)->mod;
		// int cluster = (*it)->cluster;
		
		// 将从server中获取cluster, 改为从controller中读取  -- 090727 by zhanghan
		/* if ( (*it)->cluster >= 0 ) {
			_cluster = (*it)->cluster;
		} */
		(*it)->cluster = _cluster;

		// 如果是非正常状态，则排除
		/*if ( (*it)->status != 1 ) {
			continue;
		}*/

		// 挑出备份服务
		if ( (mod < 0) || (mod >= _cluster) ) {
			mapBk.insert(make_pair<string, ServerStateInfoPtr>((*it)->prx->ice_toString(), (*it)));
			continue;
		}

		// 生成新的服务组
		if ( tmpNew.find(mod) != tmpNew.end() ) {
			tmpNew[mod].push_back(*it);
		}
		else {
			ServerStateSeq ssseq;
			ssseq.push_back(*it);
			tmpNew.insert(pair<int, ServerStateSeq>(mod, ssseq));
		}
	}
	// 决策备份服务
	set<string> setBkUsed;	//  已被选中的备份服务
	// 第一轮:如果上次已经选中，则继续选择该备份服务
	for (int mod = 0; mod < _cluster; mod++ ) {
		if ( tmpNew.find(mod) != tmpNew.end() ) {
			continue;
		}
		if ( tmpOld.find(mod) != tmpOld.end() ) {
			if ( tmpOld[mod].size() > 0 ) {
				string tmpKey = tmpOld[mod][0]->prx->ice_toString();
				if ( mapBk.find(tmpKey) != mapBk.end() ) {
					ServerStateInfoPtr tmpInfo = mapBk[tmpKey];
					tmpInfo->cluster = _cluster;
					tmpInfo->mod = mod;
					ServerStateSeq ssseq;
					ssseq.push_back(tmpInfo);
					tmpNew.insert(pair<int, ServerStateSeq>(mod, ssseq));
					setBkUsed.insert(tmpKey);
				}
			}
		}
	}
	// 第二轮：选择上次未选择的备份服务
	for (int mod = 0; mod < _cluster; mod++ ) {
		if ( tmpNew.find(mod) != tmpNew.end() ) {
			continue;
		}
		if ( setBkUsed.size() >= mapBk.size() ) {
			continue;
		}
		for ( map<string, ServerStateInfoPtr>::iterator it = mapBk.begin(); 
				it != mapBk.end(); ++it ) {
			if ( setBkUsed.find((*it).first) != setBkUsed.end() ) {
				continue;
			}
			ServerStateInfoPtr tmpInfo = (*it).second;
			tmpInfo->cluster = _cluster;
			tmpInfo->mod = mod;
			ServerStateSeq ssseq;
			ssseq.push_back(tmpInfo);
			tmpNew.insert(pair<int, ServerStateSeq>(mod, ssseq));
			setBkUsed.insert((*it).first);
      break;
		}
	}

	// 判断状态
	if ( _cluster > 0 ) {
		bool fatal = false;
		for ( int i = 0; i < _cluster; i++ )	{
			if ( tmpNew.find(i) == tmpNew.end() ) {
				fatal = true;
			}
			else if ( tmpNew[i].size() <= 0 ) {
				fatal = true;
			}
		}
		_fatalStatus = fatal;
	}
	
	
	// 更新服务列表
	_clusterState->states.swap(tmpNew);
}

void ControllerManagerI::NotifyClusterState(const Ice::Current& crt) {
  InvokeClient::create(crt);
  check(_clusterState->states, _clusterStateOld->states);
	ClusterStateListener::instance().NotifyClient(_clusterState->states);	
}

void ControllerManagerI::check(const ServerStateMap& nState, const ServerStateMap& oState) {
  for (ServerStateMap::const_iterator oi = oState.begin(); oi != oState.end(); ++oi) {
    ServerStateMap::const_iterator ni = nState.find(oi->first);
    if (ni == nState.end()) {
      ostringstream oss;
      oss << ServiceI::instance().getName() << " mod " << oi->first << " disappear";
      MCE_WARN("ControllerManagerI::check" << oss.str());
      MonitorAlerterAdapter::instance().alert(ServiceI::instance().getName(), oss.str(), ALERTERALL, 900);
      continue;
    }
    for (ServerStateSeq::const_iterator oii = oi->second.begin(); oii != oi->second.end(); ++oii) {
      ServerStateSeq::const_iterator nii = ni->second.begin();
      for (; nii != ni->second.end() && (*nii)->prx->ice_toString() != (*oii)->prx->ice_toString(); ++nii);
      if (nii == ni->second.end()) {
        ostringstream oss;
        oss << ServiceI::instance().getName() << " mod " << oi->first << " proxy " << (*oii)->prx->ice_toString() << " disappear";
        MCE_WARN("ControllerManagerI::check" << oss.str());
        MonitorAlerterAdapter::instance().alert(ServiceI::instance().getName(), oss.str(), ALERTERALL, 900);
      }
    }
  }
}

bool ControllerManagerI::ClusterChanged() {

	if ( _clusterState->states.size() != _clusterStateOld->states.size() ) {
		return true;
	}

	for ( ServerStateMap::const_iterator it = _clusterState->states.begin();
			it != _clusterState->states.end(); ++it ) {
		ServerStateMap::const_iterator itOld = _clusterStateOld->states.find((*it).first);
		if ( itOld == _clusterStateOld->states.end() ) {
			return true;
		}
		if ( (*it).second.size() != (*itOld).second.size()) {
			return true;
		}

		map<string, ServerStateInfoPtr> tmpOld;
		for ( ServerStateSeq::const_iterator itSeqOld = (*itOld).second.begin();
				itSeqOld != (*itOld).second.end(); ++itSeqOld) {
			tmpOld.insert(make_pair<string,  ServerStateInfoPtr>((*itSeqOld)->prx->ice_toString(), *itSeqOld) );
		}
		for ( ServerStateSeq::const_iterator itSeq = (*it).second.begin();
				itSeq != (*it).second.end(); ++itSeq) {
			string toString = (*itSeq)->prx->ice_toString();
			map<string, ServerStateInfoPtr>::iterator itHit = tmpOld.find(toString);
			if ( itHit == tmpOld.end() ) {
				return true;
			}
			else if ( (*itSeq)->status != (*itHit).second->status ) {
				return true;
			} else if ( ( (*itSeq)->newset && (! (*itHit).second->newset) )
					|| ( (! (*itSeq)->newset) && (*itHit).second->newset ) ) {
				return true;
			}
		}
	}

	return false;
}

string ControllerManagerI::ShowClusterState() {

	ostringstream str;
	for (int i = 0; i < _cluster; i++ ) {
		str << i << " : ";
		if ( _clusterState->states.find(i) == _clusterState->states.end() ) {
			str << "none";
		}
		else {
			ServerStateSeq& seq = _clusterState->states[i];
			for (ServerStateSeq::iterator it = seq.begin(); it != seq.end(); ++it) {
				str << (*it)->prx->ice_toString() << " : " << (*it)->status << "\t";
			}
		}
		str << "\n";
	}
	return str.str();
}

bool ControllerManagerI::ModifyClusterState(size_t pos, const string& endpoints) {
	
	if ( (pos < 0) || (pos >= _clusterState->states.size()) ) {
		// e = "out of cluster bounder";
		return false;
	}	
	/* if ( _clusterState->states[pos] == endpoints ) {
		// e = "the value to change is the same as the original";
		return true;
	} */
	else {
		// e = "modify complete";
		return true;
	}
			
}

int ControllerManagerI::StateValid() {
	if ( _fatalStatus ) {
		return -2;
	}
	else if ( _warnStatus ) {
		return -1;
	}
	else {
		return 0;
	}
}

string ControllerManagerI::description() {
        return "commands to manage clustercontrollers\n";
}

string ControllerManagerI::help() {
        ostringstream oss;
        oss << "state\tdisplay the status of service" << endl;
        oss << "modify\tmodify the list of service" << endl;
        oss << "show\tshow the list of service" << endl;
        return oss.str();
}

string ControllerManagerI::execute(const vector<string> argv) {
        if (argv[0] == "state") {
                if (argv.size() != 1){
                        return "Usage: state does not need args\n";
                }
                try {
			int state = StateValid();
                        return boost::lexical_cast<string>(state)+"\n";
                } catch (std::exception& e) {
                        return "Usage: state does not need args\n";
                }
        } else if (argv[0] == "modify") {
                if (argv.size() < 3) {
                        return "Usage: modify pos endpoint info\n";
                }
                try {
			string info;
			return boost::lexical_cast<string>(ModifyClusterState(boost::lexical_cast<int>(argv[1]),argv[2]))+"\n";
                } catch (std::exception& e) {
                        return "Usage: modify pos endpoint info\n";
                }
        } else if (argv[0] == "show") {
                if (argv.size() < 2) {
                        return "Usage: show state1 state2 ...\n";
                }
                try {
			return ShowClusterState()+"\n";
                        // return "OK\n";
                } catch (std::exception& e) {
                        return "Usage: show state1 state2 ...\n";
                }
        } else {
                return help();
        }
}

