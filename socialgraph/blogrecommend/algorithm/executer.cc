#include "socialgraph/blogrecommend/algorithm/executer.h"

#include "base/logging.h"

namespace xce {
namespace socialgraph {

void Executer::Process(long blogid) {
	blogid_ = blogid;
	if (0 != blogid_) {
		try {
			if (NULL != instance_) {
				instance_->Process(blogid);
			}
		} catch (std::exception& e) {
			LOG(WARNING) << "[BlogExecuter] Process blogid(" << blogid << ") std::exception " << e.what();
		} catch (...) {
			LOG(WARNING) << "[BlogExecuter] Process blogid(" << blogid << ") unknown::exception";
		}
	}	
}

void Executer::Display() {
	try {
		if (NULL != instance_) {
			instance_->Display();
		}
	} catch (std::exception& e) {
		LOG(WARNING) << "[BlogExecuter] Display blogid(" << blogid_ << ") std::exception " << e.what();
	} catch (...) {
		LOG(WARNING) << "[BlogExecuter] Display blogid(" << blogid_ << ") unknown::exception";
	}
}

void Executer::Get(Items& result) {
	try {
		instance_->Get(result);
	} catch (std::exception& e) {
		LOG(WARNING) << "[BlogExecuter] Get blogid(" << blogid_ << ") std::exception " << e.what();
	} catch (...) {
		LOG(WARNING) << "[BlogExecuter] Get blogid(" << blogid_ << ") unknown::exception";
	}
}

}
}
