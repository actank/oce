#include "Date.h"
#include <string.h>

using namespace MyUtil;

//--------------------------------------------------------------------------
Date::Date() {
	;
}

Date::Date(time_t sec) {
	tm_ = IceUtil::Time::microSeconds((IceUtil::Int64)sec*1000000);
}

Date::Date(const string& datetime, const string& format) {
	struct tm tm_datetime;
	memset(&tm_datetime, 0, sizeof(tm_datetime));
	strptime(datetime.c_str(), format.c_str(), &tm_datetime);
	tm_ = IceUtil::Time::seconds(mktime(&tm_datetime));
}

Date::Date(IceUtil::Time tm) :
	tm_(tm) {
	;
}

Date Date::now() {
	return Date(IceUtil::Time::now().toSeconds());
}

Date Date::seconds(IceUtil::Int64 t) {
	return Date(IceUtil::Time::seconds(t));
}

Date Date::milliSeconds(IceUtil::Int64 t) {
	return Date(IceUtil::Time::milliSeconds(t));
}

Date Date::microSeconds(IceUtil::Int64 t) {
	return Date(IceUtil::Time::microSeconds(t));
}

Date Date::datetime(const string& datetime, const string& format) {
	struct tm tm_datetime;
	memset(&tm_datetime, 0, sizeof(tm_datetime));
	strptime(datetime.c_str(), format.c_str(), &tm_datetime);
	return Date::seconds(mktime(&tm_datetime));
}

IceUtil::Int64 Date::toSeconds() const {
	return tm_.toSeconds();
}

IceUtil::Int64 Date::toMilliSeconds() const {
	return tm_.toMilliSeconds();
}

IceUtil::Int64 Date::toMicroSeconds() const {
	return tm_.toMicroSeconds();
}

double Date::toSecondsDouble() const {
	return tm_.toSecondsDouble();
}

double Date::toMilliSecondsDouble() const {
	return tm_.toMicroSecondsDouble();
}

double Date::toMicroSecondsDouble() const {
	return tm_.toMicroSecondsDouble();
}

string Date::toDateTime(const string& format) const {
	struct tm tr;
	localtime(tr);
	char buf[40];
	strftime(buf, sizeof(buf), format.c_str(), &tr);
	return buf;
}

string Date::toDuration() const {
	return tm_.toDuration();
}

string Date::str(const string& format) const {
	return toDateTime(format);
}
time_t Date::time() const {
	return toSeconds();
}

int Date::year() {
	struct tm tr;
	localtime(tr);
	return tr.tm_year + 1900;
}

int Date::month() {
	struct tm tr;
	localtime(tr);
	return tr.tm_mon + 1;
}

int Date::wday() {
	struct tm tr;
	localtime(tr);
	return tr.tm_wday;
}

int Date::day() {
	struct tm tr;
	localtime(tr);
	return tr.tm_mday;
}

int Date::hour() {
	struct tm tr;
	localtime(tr);
	return tr.tm_hour;
}

int Date::minute() {
	struct tm tr;
	localtime(tr);
	return tr.tm_min;
}

int Date::second() {
	struct tm tr;
	localtime(tr);
	return tr.tm_sec;
}

void Date::localtime(struct tm& tr) const {
	time_t time = static_cast<long> (toSeconds());
	localtime_r(&time, &tr);
}

std::ostream&
MyUtil::operator<<(std::ostream& out, const Date& date) {
	return out << date.toDateTime("%Y-%m-%d %H:%M:%S");
}
