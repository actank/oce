#ifndef _TRIPOD_CONFIGCLIENT_H__
#define _TRIPOD_CONFIGCLIENT_H__

#include "Common.h"

namespace com {
namespace renren {
namespace tripod {

class ConfigClient {

    protected:
        ConfigClient(const std::string& zkAddress, const std::string& identifier);

    public:
        virtual ~ConfigClient();

    public:
        bool initialize();

        void notifyHandle();

        void startAndDetach();

        static void static_run(ConfigClient* cc);

        virtual bool update(const ConfigDataPtr& newConfig) = 0;

    protected:

        void run();

        void notifyReUpdateThread();

    private: 
        bool reUpdateFlag_;
        boost::mutex notifyReUpdateThreadLock_;
        boost::condition notifyReUpdateThreadCdt_;
        boost::thread* reUpdateThread_;

        std::string zkAddress_;
        std::string identifier_;
};

}
}
}
#endif
