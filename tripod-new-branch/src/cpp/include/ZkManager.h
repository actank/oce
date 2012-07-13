#ifndef _TRIPOD_ZKMANAGER_H__
#define _TRIPOD_ZKMANAGER_H__

#include "Common.h"

namespace com {
namespace renren {
namespace tripod {

const char* state2String(int state);

const char* watcherEvent2String(const int ev);

class ZkManager {

    friend class ZkManagerFactory;

    private:
    ZkManager();

    public:
    ~ZkManager();

    public:

    bool initialize(std::string zkAddress);

    void notifyZkLock();

    bool reInitZk();

    zhandle_t* getZk();

    static void static_run(ZkManager* zm);

    void notifyHandle();
    
    void setReNotifyThreadState(bool flag);

    private:

    void close();

    void run();

    void startAndDetach();

    private:

    std::string zkAddress_;

    zhandle_t *zk_;

    boost::mutex zkLock_;
    boost::condition zkCdt_;

    bool reNotifyFlag_;
    boost::mutex reNotifyFlagLock_;
    boost::condition reNotifyFlagCdt_;


    boost::thread* reNotifyThread_;

}; 

static std::map<zhandle_t*, ZkManagerPtr> managers_;
static boost::shared_mutex managers_lock_;


}
}
}
#endif
