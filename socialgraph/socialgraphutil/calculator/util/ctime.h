#ifndef CTIME_H_
#define CTIME_H_

#include <string>

namespace xce {

template <typename T>
class CTimeCostT {
public:
	CTimeCostT(T& time)
		: time_(time) {
	}

	~CTimeCostT() {
		time_.Check("TOTAL");
	}

	void Check(const std::string& name) {
		time_.Check(name);
	}

private:
	T& time_;
};

}

#endif

