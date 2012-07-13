#ifndef __CHECHER_H__
#define __CHECHER_H__

#include "Checker.h"
#include "UtilCxx/src/Singleton.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "PathChecker.h"
#include "Parser.h"
#include "Reloader.h"

namespace xce{
  namespace check{

    const static int CHECKINTERNAL = 180000;
    const static int NOTIFYINTERNAL = 1000;

    class CheckerManagerI : public CheckerManager,
    public MyUtil::Singleton<CheckerManagerI> {

      public:
        void add(const std::string& path, const std::string& step, const std::string& replica, const std::string& key, 
            const std::string& checksum, const Ice::Current& = Ice::Current());

        void notify(const std::string& path, const std::string& step, const std::string& replica, const std::string& key, 
            const std::string& checksum, const Ice::Current& = Ice::Current());

        void addOne(const Entry& entry, const Ice::Current& = Ice::Current());

        void notifyOne(const Entry& entry, const Ice::Current& = Ice::Current());

        void addBatch(const std::vector<Entry>& batch, const Ice::Current& = Ice::Current());

        void notifyBatch(const std::vector<Entry>& batch, const Ice::Current& = Ice::Current());

        bool reloadConfig(const Ice::Current& = Ice::Current());

        void initialize(){
          MCE_INFO("CheckerManagerI::initialize begin");
          bool sequencevalid = false;
          initConfig(sequencevalid);
          displayConfig();
          setChecker(sequencevalid);
          _reloader = ReloaderFactory::instance().getReloader(MyUtil::ServiceI::instance().getName());
          MyUtil::TaskManager::instance().scheduleRepeated(new NotifyTimer);
          MyUtil::TaskManager::instance().scheduleRepeated(new CheckTimer(_reloader));
          MCE_INFO("CheckerManagerI::initialize end");
        }

        FullPathCheckerPtr getChecker(){
          IceUtil::RWRecMutex::RLock lock(_checkerMutex);
          return _checkerPtr;
        }	

        void setChecker(bool& sequencevalid){
          IceUtil::RWRecMutex::WLock lock(_checkerMutex);
          MCE_INFO( "***************InitFullPathChecker************" );
          _checkerPtr = &FullPathChecker::instance();
          _checkerPtr->pathCheckerMap.clear();
          for(PathMapIter pmit = _config->getConfig().begin(); pmit != _config->getConfig().end(); pmit++){
            if(sequencevalid){
              _checkerPtr->pathCheckerMap[pmit->first] = new SequencePathChecker;
            }else{
              _checkerPtr->pathCheckerMap[pmit->first] = new PathChecker;
            }
          }
        }

        void setConfig(ConfigPtr& config){
          IceUtil::RWRecMutex::WLock lock(_configMutex);
          _config = config;
        }
        ConfigPtr getConfig(){
          IceUtil::RWRecMutex::RLock lock(_configMutex);
          return _config;
        }

        void swapNotify(std::vector<Entry>& notify){
          IceUtil::Mutex::Lock lock(_notifyMutex);
          _notify.swap(notify);
        }

        class CheckTimer : public MyUtil::Timer {
          public:
            CheckTimer(ReloaderPtr reloader) :
              Timer(CHECKINTERNAL),reloader_(reloader) {
              }
            virtual void handle() {
              MCE_INFO( "Check Timer begin" );
              for(PathCheckerMap::iterator it = CheckerManagerI::instance().getChecker()->pathCheckerMap.begin();
                  it!=CheckerManagerI::instance().getChecker()->pathCheckerMap.end();it++){
                if(!it->second->checkVecEmpty(it->first)){
                  //check
                  it->second->check(it->first,reloader_);
                  it->second->clearCheckVec(it->first);
                }else{
                  MCE_INFO( "Check Timer Handle: PathChecker:" << it->first  << " checkVec is Empty");
                }
                it->second->evict(it->first);
              }
              MCE_INFO( "Check Timer end" );
            }
          private:
            ReloaderPtr reloader_;
        };

        class NotifyTimer : public MyUtil::Timer {
          public:
            NotifyTimer():
              Timer(NOTIFYINTERNAL){
              }

            virtual void handle() {
              MCE_INFO( "Notify Timer begin" );
              std::vector<Entry> todo;
              CheckerManagerI::instance().swapNotify(todo);	
              for(std::vector<Entry>::iterator it = todo.begin();it!= todo.end();++it){
                PathCheckerMapIter pcpi = CheckerManagerI::instance().getChecker()->pathCheckerMap.find(it->path);
                if(pcpi == CheckerManagerI::instance().getChecker()->pathCheckerMap.end() ){
                  MCE_WARN("NotifyTimer find no path. key: " << it->key << " not found, path: " << it->path << " step:" << it->step
                      << " replica:" << it->replica << " checksum:" << it->checksum );
                  continue;
                }
                pcpi->second->set(it->key,it->step,it->replica,it->checksum);
              }
              MCE_INFO( "Notify Timer end" );
            }

        };

      private:
        void add(const Entry& entry);
        void notify(const Entry& entry);
      private:
        bool initConfig(bool& sequencevalid);
        void displayConfig();
        void initFullPathChecker();

        FullPathCheckerPtr _checkerPtr;
        IceUtil::RWRecMutex _checkerMutex;
        ConfigPtr _config;
        IceUtil::RWRecMutex _configMutex;
        std::vector<Entry> _notify;
        IceUtil::Mutex _notifyMutex;

        std::string _filename;
        ReloaderPtr _reloader;

        friend class MyUtil::Singleton<CheckerManagerI>;
        CheckerManagerI(){
          ;
        }	
    };

    class ParserHelper {
      public:
        static bool parse(std::string& file, bool& sequencevalid){
          MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
          MyUtil::DocumentPtr doc = MyUtil::Parser::parse(file);
          if(!doc)
            return false;
          MyUtil::NodeList roots = doc->getChildren();
          if (!roots.size()) {
            MCE_INFO("CheckerManagerI::initConfig -> roots.size: " << roots.size() << " is invalid" );
            return false;
          }
          MCE_INFO("service name: " << service.getName());
          MyUtil::NodeList instanceNodes = roots[0]->getChildren();

          ConfigPtr config = new Config;
          bool success = false;
          for (size_t i = 0; i < instanceNodes.size(); ++i) {
            MyUtil::NodePtr serviceNode = instanceNodes.at(i);
            if (serviceNode->getName() == "service" &&
                service.getName() == serviceNode->getAttribute("name")) {
              if( "YES" == serviceNode->getAttribute("sequencevalid")){
                sequencevalid = true;
              }
              success = true;
              MyUtil::NodeList pathNodes = serviceNode->getChildren();
              for(size_t j =0; j<pathNodes.size(); ++j){
                MyUtil::NodePtr pathNode = pathNodes.at(j);
                if(pathNode->getName() == "path"){
                  map<string, vector<string> > stepMap;
                  map<string, bool > checksumValidMap;
                  MyUtil::NodeList stepNodes = pathNode->getChildren();
                  for(size_t k =0; k<stepNodes.size(); ++k){
                    MyUtil::NodePtr stepNode = stepNodes.at(k);
                    if(stepNode->getName() == "step"){
                      vector<string> replicaVec;
                      MyUtil::NodeList replicaNodes = stepNode->getChildren();
                      for(size_t l =0; l<replicaNodes.size(); ++l){
                        MyUtil::NodePtr replicaNode = replicaNodes.at(l);
                        if(replicaNode->getName() == "replica"){
                          replicaVec.push_back(replicaNode->getAttribute("name"));
                        }
                      }
                      stepMap.insert(make_pair<string, vector<string> >( stepNode->getAttribute("name"), replicaVec));
                      if( "YES" == stepNode->getAttribute("checksumvalid")){
                        checksumValidMap[stepNode->getAttribute("name")] = true;
                      }else{
                        checksumValidMap[stepNode->getAttribute("name")] = false;
                      }
                    }
                  }
                  config->getConfig().insert(make_pair<std::string, PathPtr>(pathNode->getAttribute("name"),new Path(stepMap, checksumValidMap)));
                }
              }
            }
          }
          if(success){
            CheckerManagerI::instance().setConfig(config);	
          }
          return true;
        }	
        /*
           MyUtil::NodePtr getNode(MyUtil::NodeList& list, std::string name, std::string compare){
           for (size_t i = 0; i < list.size(); ++i) {
           MyUtil::NodePtr serviceNode = list.at(i);
           if( (serviceNode->getName() == name) 
           && (serviceNode->getAttribute("name") == compare)) {
           return serviceNode;	
           }
           }
           return NULL;
           }

           void parse(MyUtil::NodeList& list, std::string name[], int index, StepMap& stepmap, std::string& step){
           for (size_t i = 0; i < list.size(); ++i) {
           MyUtil::NodePtr serviceNode = list.at(i);
           if (serviceNode->getName() == name[index]) {
           MCE_INFO(serviceNode->getName() << " name:" << serviceNode->getAttribute("name"));
           switch(name[index]){
           case "step":{
           step = name[index];
           StepMapIter it = stepmap.find(step);
           if(it == stepmap.end()){
           stepmap.insert(make_pair<std::string,StepPtr>(name[index], new Step));
           }
           }break;
           case "replica": {
           StepMapIter it = stepmap.find(step);
           if(it != stepmap.end()){
           it->replicaMap.insert(make_pair<std::string,ReplicaPtr>(name[index],new Replica));
           }
           }break;

           }
           MyUtil::NodeList nodes = serviceNode->getChildren();
           parse(nodes,name,index+1);
           }
           }	
           }
         */
    };

  };
};

#endif
