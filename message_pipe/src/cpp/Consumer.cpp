#include "Consumer.h"
#include "BrokerConnection.h"
#include "Producer.h"

namespace xce {
namespace messagepipe {

void Consumer::run() {
		while (true) {
			try {
				std::vector<BytesMessagePtr> buffers;
				std::cout << "Consumer Receive ";
				brokerConnection_->receive(subject_,buffers);
				std::cout << "Consumer Receive " << buffers.size() << std::endl;
				std::vector<MessageDataPtr> datas;
				datas.reserve(buffers.size());
				for (std::vector<BytesMessagePtr>::iterator bm=buffers.begin();bm!=buffers.end();++bm) {
					try {
						std::vector<unsigned char> body((*bm)->getBodyLength());
						(*bm)->readBytes(body);
						MessageDataPtr data( new MessageData(cluster_, subject_, body) );
						datas.push_back(data);
					} catch (cms::CMSException& e) {
					}
				}
				if (!datas.empty()) {
					std::vector<MessageDataPtr> failMessages;
					onMessage(datas,failMessages);
					if (!failMessages.empty()) {
						Producer::getInstance()->sendBatch(zk_address_, failMessages);
					}
				}
				if (!buffers.empty()) {
					try {
						buffers.back()->acknowledge();
					} catch (cms::CMSException& e) {
					}
				}
				sleep(1);
			} catch (std::exception& ex) {
				std::cout << "consumer exception : " << ex.what() << std::endl;
			} catch (...){
				std::cout << "consumer unknown_exception " << std::endl;
			}
	}
}

std::string Consumer::getCluster() {
    return cluster_;
}

}
}
