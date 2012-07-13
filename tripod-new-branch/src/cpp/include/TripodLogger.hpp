#ifndef _TRIPOD_LOGGER_HPP__
#define _TRIPOD_LOGGER_HPP__

#include "time.h"
#include <string>
#include <vector>
#include <LogWrapper.h>
#include <util/cpp/IpAddr.hpp>
#include "Singleton.hpp"
#include "OceCxxAdapter/src/XceLoggerAdapter.h"

namespace com {
namespace renren {
namespace tripod {

class TripodLogger : public Singleton<TripodLogger> {

    public:
        TripodLogger() {
            category_.push_back("tripod");
            category_.push_back("unknown");
            std::string endPoints="M@XceLogger";
            logger_ = com::xiaonei::service::XceLoggerAdapter::instance().getLogger(endPoints);
            logger_->initialize();
            localAddress_ = MyUtil::IpAddr::inner();
        }

    public:
        void addLog(const std::string& msg) {
            std::ostringstream os;
            os << getNow();
            os << " Local : " << localAddress_;
            os << " Message : " << msg;
            logger_->addLog(category_, os.str());
        }

    private:
        std::string getNow() {
            time_t nowT;
            struct tm* nowTm;
            time(&nowT);
            nowTm = localtime(&nowT);
            std::ostringstream os;
            os << (nowTm->tm_year + 1900) << "-" << nowTm->tm_mon << "-" << nowTm->tm_mday << " " << nowTm->tm_hour << ":" << nowTm->tm_min << ":" << nowTm->tm_sec;
            return os.str();
        }
    
    private: 
        std::string localAddress_;
        com::xiaonei::service::XceLoggerPtr logger_;
        std::vector<std::string> category_;
};

#define TRIPOD_DEBUG(msg) MCE_DEBUG(msg);
#define TRIPOD_INFO(msg) MCE_INFO(msg);

#define TRIPOD_WARN(msg) \
    do { \
        MCE_WARN(msg);\
        TripodLogger::getInstance()->addLog(msg);\
    } while(0);

}
}
}
#endif
