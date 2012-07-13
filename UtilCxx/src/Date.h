#ifndef __XCE_UTILCXX_DATE_H__
#define __XCE_UTILCXX_DATE_H__

#include <string>
#include <IceUtil/Time.h>

namespace MyUtil {
using namespace std;

class Date {
public:
	Date();
	explicit Date(time_t sec);
	explicit Date(IceUtil::Time tm);
	explicit Date(const string& datetime, const string& format = "%Y-%m-%d %H:%M:%S");

	static Date now();
	static Date seconds(IceUtil::Int64);
	static Date milliSeconds(IceUtil::Int64);
	static Date microSeconds(IceUtil::Int64);
	static Date datetime(const string& datetime, const string& format =
			"%Y-%m-%d %H:%M:%S");

	IceUtil::Int64 toSeconds() const;
	IceUtil::Int64 toMilliSeconds() const;
	IceUtil::Int64 toMicroSeconds() const;

	double toSecondsDouble() const;
	double toMilliSecondsDouble() const;
	double toMicroSecondsDouble() const;

	string toDateTime(const string& format = "%Y-%m-%d %H:%M:%S") const;
	string toDuration() const;

	string str(const string& format = "%Y-%m-%d %H:%M:%S") const;
	time_t time() const;

	int year();
	int month();
	int wday();
	int day();
	int hour();
	int minute();
	int second();

	Date operator-() const {
		return Date(-tm_);
	}

	Date operator-(const Date& rhs) const {
		return Date(tm_ - rhs.tm_);
	}

	Date operator+(const Date& rhs) const {
		return Date(tm_ + rhs.tm_);
	}

	Date& operator+=(const Date& rhs) {
		tm_ += rhs.tm_;
		return *this;
	}

	Date& operator-=(const Date& rhs) {
		tm_ -= rhs.tm_;
		return *this;
	}

	bool operator<(const Date& rhs) const {
		return tm_ < rhs.tm_;
	}

	bool operator<=(const Date& rhs) const {
		return tm_ <= rhs.tm_;
	}

	bool operator>(const Date& rhs) const {
		return tm_ > rhs.tm_;
	}

	bool operator>=(const Date& rhs) const {
		return tm_ >= rhs.tm_;
	}

	bool operator==(const Date& rhs) const {
		return tm_ == rhs.tm_;
	}

	bool operator!=(const Date& rhs) const {
		return tm_ != rhs.tm_;
	}

	double operator/(const Date& rhs) const {
		return  tm_ / rhs.tm_;
	}

	Date& operator*=(int rhs) {
		tm_ *= rhs;
		return *this;
	}

	Date operator*(int rhs) const {
		Date t;
		t.tm_ = tm_ * rhs;
		return t;
	}

	Date& operator/=(int rhs) {
		tm_ /= rhs;
		return *this;
	}

	Date operator/(int rhs) const {
		Date t;
		t.tm_ = tm_ / rhs;
		return t;
	}

	Date& operator*=(IceUtil::Int64 rhs) {
		tm_ *= rhs;
		return *this;
	}

	Date operator*(IceUtil::Int64 rhs) const {
		Date t;
		t.tm_ = tm_ * rhs;
		return t;
	}

	Date& operator/=(IceUtil::Int64 rhs) {
		tm_ /= rhs;
		return *this;
	}

	Date operator/(IceUtil::Int64 rhs) const {
		Date t;
		t.tm_ = tm_ / rhs;
		return t;
	}

	Date& operator*=(double rhs) {
		tm_ = tm_ * rhs;
		return *this;
	}

	Date operator*(double rhs) const {
		Date t;
		t.tm_ = tm_ * rhs;
		return t;
	}

	Date& operator/=(double rhs) {
		tm_ = tm_ / rhs;
		return *this;
	}

	Date operator/(double rhs) const {
		Date t;
		t.tm_ = tm_ / rhs;
		return t;
	}
private:
	void localtime(struct tm& tr) const;

	IceUtil::Time tm_;
};

std::ostream& operator<<(std::ostream&, const Date&);

}
;

#endif

