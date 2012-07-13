#ifndef _MP_MESSAGEDATA_H__
#define _MP_MESSAGEDATA_H__

#include "Common.h"

namespace xce {
namespace messagepipe {

class MessageData {

private:
	std::string cluster_;
	std::string subject_;
	std::vector<unsigned char> data_;
public: 
	MessageData(const std::string& cluster, const std::string& subject, const std::vector<unsigned char>& data)
				:	cluster_(cluster),subject_(subject) {
		data_ = data;
	}
	

	std::string getCluster() {
		return cluster_;
	}

	std::string getSubject() {
		return subject_;
	}

	std::vector<unsigned char>& getData() {
		return data_;
	}

};

}
}

#endif
