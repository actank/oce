#include <string>
using namespace std;

static std::string GmtTime(time_t t) {
  static const string week[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  static const string month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
                                    "Aug", "Sep", "Oct", "Nov", "Dec" };
  struct tm tms;
  tms = *gmtime(&t);
  ostringstream res;
  // Expires: Mon, 26 Jul 1997 05:00:00 GMT
  res << month[tms.tm_wday] << ", ";
  if(tms.tm_mday < 10){
    res << 0;
  }
  res << tms.tm_mday << " "
      << month[tms.tm_mon] << " " << tms.tm_year + 1900 << " ";
  if (tms.tm_hour < 10) {
    res << 0;
  }
  res << tms.tm_hour << ":";

  if (tms.tm_min < 10) {
    res << 0;
  }
  res << tms.tm_min << ":";

  if (tms.tm_sec < 10) {
    res << 0;
  }
  res << tms.tm_sec << " GMT";
  return res.str();
}

static const char * GetReasonPhrase(int code) {
  switch(code) {
  case 200:
    return "OK";
  case 404:
    return "Not Found";
  case 403:
    return "Forbidden";
  case 408:
    return "Request Timeout";
  default:
    return "Unknown";
  }
}

