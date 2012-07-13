#ifndef ZOOKEEPERMONITOR_H
#define ZOOKEEPERMONITOR_H

#include <zookeeper/zookeeper.h>
#include <vector>
#include <string>
#include <iostream>
#include <IceUtil/IceUtil.h>

#include <ServiceI.h>

namespace xce
{
namespace distcache
{
namespace monitor
{
	void ZKWatcher(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx) {
		std::ostringstream os;
		os<<"ZKWatcher type = " << type << " state = " <<  state << " path = " << path;
		MCE_INFO(os.str()); 
	}

	class ZooKeeperMonitor : virtual public IceUtil::Thread , public MyUtil::Singleton<ZooKeeperMonitor>{
	public:
		ZooKeeperMonitor() {
			init();
		}

		void init()
		{
			try {
				zks_.push_back(zookeeper_init("UserZooKeeper1:2181", ZKWatcher, 10000, 0, 0, 0));
				zks_.push_back(zookeeper_init("UserZooKeeper2:2181", ZKWatcher, 10000, 0, 0, 0));
				zks_.push_back(zookeeper_init("UserZooKeeper3:2181", ZKWatcher, 10000, 0, 0, 0));
				zks_.push_back(zookeeper_init("UserZooKeeper4:2181", ZKWatcher, 10000, 0, 0, 0));
				zks_.push_back(zookeeper_init("UserZooKeeper5:2181", ZKWatcher, 10000, 0, 0, 0));
				//zks_.push_back(zookeeper_init("u29:2181", ZKWatcher, 10000, 0, 0, 0));
			} catch (...) {
				MCE_ERROR("ZooKeeperThread::ZooKeeperThread unknown exception");
			}
			for (std::vector<zhandle_t*>::iterator i = zks_.begin(); i != zks_.end(); ++i) {
				if (*i == 0) {
					MCE_ERROR("ZooKeeperThread::ZooKeeperThread zookeeper_init failure");
				}
			}
		}
		
		~ZooKeeperMonitor() {}
		virtual void run() {
			while(true) {
				try {
					sleep(10);
					if (!check("/DistCache")) {
						MCE_WARN("ZooKeeperThread::run check failure");
						if( zks_.size() <= 0)
						{
							init();
						}
					}
				} catch (...) {
					MCE_WARN("ZooKeeperThread::run unknown exception.");
				}
			}
		}
	
	private:
		std::vector<zhandle_t*> zks_;
		bool check(const std::string& path) {
			MCE_DEBUG("ZooKeeperMonitor::check path: " << path);
			std::vector<std::string> datas;
	
			for (std::vector<zhandle_t*>::iterator i = zks_.begin(); i != zks_.end(); ++i) {
				if( *i == 0 )
				{
					MCE_WARN("ZooKeeperMonitor::check zhandle_t = 0");
					std::vector<zhandle_t*>(0).swap(zks_);
					return false;
				}
				char buffer[1048576];
				int buflen = sizeof(buffer);
				int rc = zoo_get(*i, path.c_str(), 0, buffer, &buflen, 0);
				if (rc != ZOK) {
					MCE_WARN("ZooKeeperMonitor::check zoo_get result: "<<rc);
					return false;
				}
				datas.push_back(std::string(buffer, buffer + buflen));
			}
			for (unsigned int i = 1; i < zks_.size(); ++i) {
				if (datas[i] != datas[0]) {
					MCE_WARN("ZooKeeperMonitor::check data different");
					return false;
				}
			}
			struct String_vector node_names;
			int rc = zoo_get_children(zks_[0], path.c_str(), 0, &node_names);
			if (rc != ZOK) {
				MCE_WARN("ZooKeeperMonitor::check zoo_get_children result: "<<rc);
				return false;
			}
	
			int count = node_names.count;
			std::vector<std::string> nodes;
			for (int node_name = 0; node_name < node_names.count; ++node_name) {
				nodes.push_back(node_names.data[node_name]);
			}
			for (std::vector<zhandle_t*>::iterator i = zks_.begin() + 1; i != zks_.end(); ++i) {
				rc = zoo_get_children(*i, path.c_str(), 0, &node_names);
				if (rc != ZOK) {
					MCE_WARN("ZooKeeperMonitor::check zoo_get_children result: "<<rc);
					return false;
				}
				if (count != node_names.count) {
					MCE_WARN("ZooKeeperMonitor::check zoo_get_children count different");
					return false;
				}
			}
	
			for (std::vector<std::string>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
				std::ostringstream oss;
				oss << path << "/" << *i;
				if (!check(oss.str())) {
					return false;
				}
			}
			return true;
		}
	};
}
}
}

#endif
