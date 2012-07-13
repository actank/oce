#include "executer.h"
namespace xce {
namespace socialgraph {

void Executer::Grouping(int userid) {
	userid_ = userid;
	if (0 != userid_) {
		try {
			if (NULL != instance_) {
				instance_->Grouping(userid_);
			}
		} catch (std::exception e) {
			std::cout << "[AutoGroupWorker] Grouping userid(" << userid_ << ") std::exception "	<< e.what() << std::endl;
		} catch (...) {
			std::cout << "[AutoGroupWorker] Grouping userid(" << userid_ << ") unknown::exception" << std::endl;
		}
	}
}

void Executer::Display() {
	try {
		if (NULL != instance_) {
			instance_->Display();
		}
	} catch (std::exception e) {
		std::cout << "[AutoGroupWorker] Display userid(" << userid_ << ") std::exception "	<< e.what() << std::endl;
	} catch (...) {
		std::cout << "[AutoGroupWorker] Display userid(" << userid_ << ") unknown::exception" << std::endl;
	}
}

GroupStats Executer::GetGroupStats() {
	try {
		if (NULL != instance_) {
			return instance_->GetGroupStats();
		}
	} catch (std::exception e) {
		std::cout << "[AutoGroupWorker] Display userid(" << userid_ << ") std::exception "	<< e.what() << std::endl;
	} catch (...) {
		std::cout << "[AutoGroupWorker] Display userid(" << userid_ << ") unknown::exception" << std::endl;
	}

	return GroupStats();
}

void Executer::GetGroupResult(Items& result) {
	try {
		if (NULL != instance_) {
			return instance_->GetGroupResult(result);
		}
	} catch (std::exception e) {
		std::cout << "[AutoGroupWorker] Display userid(" << userid_ << ") std::exception "	<< e.what() << std::endl;
	} catch (...) {
		std::cout << "[AutoGroupWorker] Display userid(" << userid_ << ") unknown::exception" << std::endl;
	}
}

}
}
