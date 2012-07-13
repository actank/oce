/*
 * Communicator.h
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <iostream>
#include <zookeeper/zookeeper.h>
#include <zookeeper/zookeeper.jute.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "logger.h"
#include "observer.h"
#include "subject.h"

namespace com {
namespace renren {
namespace tripod {

static bool ready_;
static boost::shared_mutex ready_mutex_;

class BufferContainer {
public:
	BufferContainer() {
		buflen = sizeof(buffer);
		memset(buffer, 0, buflen);
	}
public:
	char buffer[1048576];
	int buflen;
};
typedef boost::shared_ptr<BufferContainer> BufferContainerPtr;

void ZooKeeperWatcher(zhandle_t *zzh, int type, int state, const char *path,
		void *watcherCtx);

class Communicator: public Subject {
public:
	void Initialize(const std::string& communicator_addr);
	void Register(ObserverPtr observer);
	void UnRegister(ObserverPtr observer);

	std::string GetData(const std::string& path);
	std::vector<std::string> GetChild(const std::string& path);
private:
	zhandle_t* handle_;
};// end Communicator
typedef boost::shared_ptr<Communicator> CommunicatorPtr;

class CommunicatorFactory {
public:
	static CommunicatorPtr Create(const std::string& communicator_addr);
	static std::map<std::string, CommunicatorPtr> getAll();
private:
	static std::map<std::string, CommunicatorPtr> communicators_;
	static boost::shared_mutex mutex_;
};// end CommunicatorFactory

}
}
}

#endif /* COMMUNICATOR_H_ */
