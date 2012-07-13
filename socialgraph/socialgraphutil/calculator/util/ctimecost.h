#ifndef CTIMECOST_H_
#define CTIMECOST_H_
#include <sys/time.h>
#include <string>
#include <ctime>

#include "base/logging.h"
#include "socialgraph/socialgraphutil/calculator/util/ctime.h"

namespace xce {

namespace detail {
inline double TimevalSub(const struct timeval *t1, const struct timeval *t2) {
	return (double)((t2->tv_sec * 1000000 + t2->tv_usec)
		- (t1->tv_sec * 1000000 + t1->tv_usec))/1000;
}
}

class CTimeCost {
public:
	typedef CTimeCostT<CTimeCost> CTime;

	CTimeCost(const std::string& name)
		: name_(name) {
		gettimeofday(&wall_clock_, 0);
	}

	void Check(const std::string& append) {
		double diff_wall = WallClock();
		gettimeofday(&wall_clock_, 0);

		std::ostringstream os;
		std::istream::fmtflags old_flags = os.setf(std::istream::fixed,
			std::istream::floatfield);
		std::streamsize old_prec = os.precision(3);

		os << name_ << " ";
		if (!append.empty()) {
			os << append << " ";
		}
		os << "clock: " << diff_wall << " ms";
		os.flags(old_flags);
		os.precision(old_prec);

		LOG(INFO) << os.str();  //tmp
	}

private:
	double WallClock() const {
		struct timeval end;
		gettimeofday(&end, 0);
		return detail::TimevalSub(&wall_clock_, &end);
	}

	std::string name_;
	struct timeval wall_clock_;
};

}

#define SGTIME(ct, name) \
	xce::CTimeCost ct(name)

#define SGTIME_CHECK(ct, name) \
	ct.Check(name)

#define SGCTIME(cct, name) \
	xce::CTimeCost tt(name); \
	xce::CTimeCost::CTime cct(tt)

#define SGCTIME_CHECK(cct, name) \
	cct.Check(name)
#endif
