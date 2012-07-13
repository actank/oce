#include "ZkManager.h"

#include "ConfigManager.h"
#include "ConfigManagerFactory.h"

namespace com {
namespace renren {
namespace tripod {

const char* state2String(int state){
    switch(state){
        case 0:
            return "ZOO_CLOSED_STATE";
        case 1:
            return "ZOO_CONNECTING_STATE";
        case 2:
            return "ZOO_ASSOCIATING_STATE";
        case 3:
            return "ZOO_CONNECTED_STATE";
        case -112:
            return "ZOO_EXPIRED_SESSION_STATE";
        case -113:
            return "ZOO_AUTH_FAILED_STATE";
    }
    return "INVALID_STATE";
}

const char* watcherEvent2String(const int ev){
    switch(ev){
        case 0:
            return "ZOO_ERROR_EVENT";
        case 1:
            return "ZOO_CREATED_EVENT";
        case 2:
            return "ZOO_DELETED_EVENT";
        case 3:
            return "ZOO_CHANGED_EVENT";
        case 4:
            return "ZOO_CHILD_EVENT";
        case -1:
            return "ZOO_SESSION_EVENT";
        case -2:
            return "ZOO_NOTWATCHING_EVENT";
    }
    return "INVALID_EVENT";
}


void ZkWatcher(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx) {
    std::ostringstream os;
    os<<"ZkWatcher type = "<<watcherEvent2String(type)<<" state = "<<state2String(state)<<" path = "<<path; 
    MCE_WARN(os.str());

    if( type == ZOO_SESSION_EVENT )
    {

        if( state == ZOO_CONNECTING_STATE )
        {
            boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
            managers_[zzh]->setReNotifyThreadState(false);
            return;
        }
        
        if( state == ZOO_CONNECTED_STATE )
        {
            boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
            managers_[zzh]->notifyZkLock();
            managers_[zzh]->notifyHandle();
            return;
        }

        if( state == ZOO_EXPIRED_SESSION_STATE )
        {
            ZkManagerPtr tempPtr = ZkManagerPtr();
            {
                boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
                tempPtr = managers_[zzh];
            }

            if( tempPtr )
            {
                tempPtr->setReNotifyThreadState(false);
                while( tempPtr->reInitZk(zzh) == false )
                {
                    sleep(1000);
                }
            }else
            {
                MCE_WARN("ZkWatcher can not reinitzk!");
            }

            {
                boost::upgrade_lock<boost::shared_mutex> lock(managers_lock_);
                boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
                std::map<zhandle_t*,ZkManagerPtr>::iterator findIt = managers_.find( zzh );
                if( findIt != managers_.end() )
                {
                    managers_.erase(findIt);
                }
            }
            return;
        }
    }

    {
        boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
        managers_[zzh]->notifyHandle();
    }

}

ZkManager::ZkManager() {
}

ZkManager::~ZkManager() {
    if(reNotifyThread_) {
        delete reNotifyThread_;
    }
}

bool ZkManager::initialize(std::string zkAddress) {

    MCE_INFO("ZkManager::initialize zkAddress " << zkAddress);
    
    zkAddress_ = zkAddress;

    {
        boost::unique_lock<boost::mutex> lock( zkLock_ );

        {
            boost::upgrade_lock<boost::shared_mutex> lock(managers_lock_);
            boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock); 

            zk_ = zookeeper_init(zkAddress_.c_str(), com::renren::tripod::ZkWatcher, 10000, 0, 0, 0);

            if( zk_ == NULL )
            {
                MCE_WARN("ZkManager::initZk can not initZk for zkAddress " << zkAddress_ << " !");
                return false;
            }
            managers_[zk_] = ZkManagerPtr(this);
        }

        zkCdt_.wait( lock );
    }

    reNotifyFlag_ = false;

    startAndDetach();

    return true;
}

void ZkManager::notifyZkLock() {
    MCE_INFO("ZkManager::notifyZkLock");
    zkCdt_.notify_all();
}

bool ZkManager::reInitZk(zhandle_t* zzh) {
    MCE_INFO("ZkManager::reInitZk");
    close();

    boost::unique_lock<boost::mutex> lock( zkLock_ );

    {
        boost::upgrade_lock<boost::shared_mutex> lock(managers_lock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock); 

        zk_ = zookeeper_init(zkAddress_.c_str(), com::renren::tripod::ZkWatcher, 10000, 0, 0, 0);

        if( zk_ == NULL )
        {
            MCE_WARN("ZkManager::reinitZk can not initZk for zkAddress " << zkAddress_ << " !");
            return false;
        }
        managers_[zk_] = managers_[zzh];
    }

    return true;
}

zhandle_t* ZkManager::getZk() {
    MCE_INFO("ZkManager::getZk");
    boost::unique_lock<boost::mutex> lock( zkLock_ );
    return zk_;
}

void ZkManager::static_run(ZkManager* zm) {
    zm->run();
}

void ZkManager::notifyHandle() {

    MCE_INFO("ZkManager::notifyHandle");
    ConfigManagerPtr configManagerPtr = ConfigManagerFactory::getInstance()->getConfigManager(zkAddress_);

    if(configManagerPtr){
        boost::unique_lock<boost::mutex> lock( zkLock_ );
        if(configManagerPtr->updateConfig(zk_)){
            return;
        }
    }else{
        MCE_WARN("ZkManager::notifyHandle failed no configmanager of zkaddress " << zkAddress_);
    }

    MCE_WARN("ZkManager::notifyHandle failed for configmanager of zkaddress " << zkAddress_);

    setReNotifyThreadState(true);
}

void ZkManager::close() {
    MCE_INFO("ZkManager::close");
    boost::unique_lock<boost::mutex> lock( zkLock_ );
    if (zk_ != NULL) {
        zookeeper_close(zk_);
    }
}

void ZkManager::setReNotifyThreadState(bool flag) {
    MCE_INFO("ZkManager::setReNotifyThreadState flag " << flag);
    boost::unique_lock<boost::mutex> lock(reNotifyFlagLock_);
    reNotifyFlag_ = flag;
    if(reNotifyFlag_) {
        reNotifyFlagCdt_.notify_all();
    }
}

void ZkManager::run() {
    while( true ){
        MCE_INFO("ZkManager::run begin");
        {
            boost::unique_lock<boost::mutex> lock(reNotifyFlagLock_);
            if( reNotifyFlag_ == false ) {
                reNotifyFlagCdt_.wait( lock );
            }
            reNotifyFlag_ = false;
        }

        notifyHandle();

        sleep( 1000 );
    }
}

void ZkManager::startAndDetach() {
    reNotifyThread_ = new boost::thread(boost::bind(static_run, this));
    reNotifyThread_->detach();
}

};
};
};
