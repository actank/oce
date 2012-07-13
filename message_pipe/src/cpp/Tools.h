#ifndef _MP_TOOLS_H__
#define _MP_TOOLS_H__



#include "Common.h"
#include "Producer.h"
#include "MessageData.h"
#include "Consumer.h"

namespace xce {
namespace messagepipe{

class ProducerTool : public IceUtil::Thread {

public:
	static int messageCount_ ;
	static int messageSize_;
	static volatile int parallelThreads_;
	static std::string cluster_;
	static std::string subject_;

	virtual void run();
	void sendLoop();

  MessageDataPtr createMessageText(int index) {
		std::vector<unsigned char> bytes(messageSize_);
		MessageDataPtr data(new MessageData(cluster_, subject_, bytes));
		return data;
	}

	static void setMessageCount(int messageCount) {
		messageCount_ = messageCount;
	}

	static void setMessageSize(int messageSize) {
		messageSize_ = messageSize;
	}

	static void setSubject(const std::string& subject) {
		subject_ = subject;
	}

	static void setParallelThreads(int parallelThreads) {
		if (parallelThreads < 1) {
			parallelThreads = 1;
		}
		parallelThreads_ = parallelThreads;
	}
	
};
typedef boost::shared_ptr<ProducerTool> ProducerToolPtr;

class ConsumerTool : public Consumer {

public:
	static volatile int parallelThreads_;
	static std::string subject_;
	static std::string cluster_;
	ConsumerTool(const std::string& zk_address, const std::string& cluster, const std::string& subject) : Consumer(zk_address, cluster, subject){
	}


	static void setSubject(const std::string& subject) {
		subject_ = subject;
	}

	static void setParallelThreads(int parallelThreads) {
		if (parallelThreads < 1) {
			parallelThreads = 1;
		}
		parallelThreads_ = parallelThreads;
	}

	virtual void onMessage(const std::vector<MessageDataPtr>& receiveMessages, std::vector<MessageDataPtr>& failMessages) {
		std::cout << "receive Message size :"  << receiveMessages.size() << std::endl;
	}
};

typedef boost::shared_ptr<ConsumerTool> ConsumerToolPtr;
}
}
#endif
