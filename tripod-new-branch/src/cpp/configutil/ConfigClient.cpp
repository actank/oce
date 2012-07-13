#include "ConfigManager.h"
#include "ConfigManagerFactory.h"

#include "ConfigClient.h"

namespace com {
namespace renren {
namespace tripod {
    
ConfigClient::ConfigClient(const std::string& zkAddress, const std::string& identifier) :
    zkAddress_(zkAddress), identifier_(identifier) {
    }

ConfigClient::~ConfigClient() {
    if(reUpdateThread_) {
        delete reUpdateThread_;
    }
}

bool ConfigClient::initialize() {
    MCE_INFO("ConfigClient::initialize zkAddress " << zkAddress_);
    ConfigManagerPtr configManager = ConfigManagerFactory::getInstance()->getConfigManager(zkAddress_);
    if(configManager == NULL) {
        MCE_WARN("ConfigClient::initialize no configmanager of zkAddress " << zkAddress_);
        return false;
    }

    configManager->registerClient(ConfigClientPtr(this));

    ConfigDataPtr newConfig = configManager->getConfigData();
    if(update(newConfig) == false) {
        MCE_WARN("ConfigClient::initialize update failed!");
        return false;
    }

    startAndDetach();

    return true;
}

void ConfigClient::startAndDetach() {
    reUpdateThread_ = new boost::thread(boost::bind(static_run, this));
    reUpdateThread_->detach();
}

void ConfigClient::static_run(ConfigClient* cc) {
    cc->run(); 
}

void ConfigClient::run() { 
    while( true ){
        MCE_INFO("ConfigClient::run begin");
        {
            boost::unique_lock<boost::mutex> lock(notifyReUpdateThreadLock_);
            if( reUpdateFlag_ == false ) {
                notifyReUpdateThreadCdt_.wait( lock );
            }
            reUpdateFlag_ = false;
        } 

        notifyHandle();

        sleep( 1000 );
    }
}

void ConfigClient::notifyHandle() {
    MCE_INFO("ConfigClient::notifyHandle");
    ConfigDataPtr newConfig = ConfigManagerFactory::getInstance()->getConfigManager(zkAddress_)->getConfigData();
    if(update(newConfig) == false) {
        MCE_WARN("ConfigClient::notifyHandle update failed!");
        notifyReUpdateThread();
    }
}

void ConfigClient::notifyReUpdateThread() {
    MCE_INFO("ConfigClient::notifyReUpdateThread");
    boost::unique_lock<boost::mutex> lock(notifyReUpdateThreadLock_);
    reUpdateFlag_ = true;
    notifyReUpdateThreadCdt_.notify_all();
}

}
}
}
