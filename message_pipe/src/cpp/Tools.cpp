#include "Tools.h"

namespace xce {
namespace messagepipe { 

void ProducerTool::run() {
		std::cout << "thread start at " << time(NULL) << std::endl;
		try {
			sendLoop();
		} catch (std::exception& e) {
			std::cout << "exception : " << e.what() << std::endl;
		}
		--parallelThreads_;
		std::cout << "thread end at " << time(NULL) << std::endl;
}
void ProducerTool::sendLoop() {

	for (int i = 0; i < messageCount_ || messageCount_ == 0; i++) {
		xce::messagepipe::Producer::getInstance()->send("localhost:2181",createMessageText(i));
	}
}

}
}
