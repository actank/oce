#include "CheckerI.h"

using namespace std;
using namespace MyUtil;
using namespace xce::check;

void MyUtil::initialize() {
  MCE_INFO("MyUtil::initialize begin");
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&CheckerManagerI::instance(), service.createIdentity(
        "M", ""));

  CheckerManagerI::instance().initialize();
  MCE_INFO("MyUtil::initialize end");
}


bool CheckerManagerI::initConfig(bool& sequencevalid){
  _filename = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("Service.Config",
      "../etc/checker.xml");
  bool success = ParserHelper::parse(_filename,sequencevalid);
  return success;		
}

void CheckerManagerI::displayConfig(){
  MCE_INFO( "***************Display Config************" );
  for(PathMapIter pmit = _config->getConfig().begin(); pmit != _config->getConfig().end(); pmit++){
    MCE_INFO( "***************Path Map Begin*************" );
    MCE_INFO( "Path: " << pmit->first );
    _config->getConfig()[pmit->first]->display("","");
    MCE_INFO( "***************Path Map End***************" );
  }
}

void CheckerManagerI::add(const string& path, const string& step, const string& replica, 
    const string& key, const string& checksum, const Ice::Current& current){

  MCE_INFO( "CheckerManagerI::add key: " << key << " path:" << path << " step:" << step 
      << " replica:" << replica << " checksum:" << checksum );
  PathCheckerMapIter pcpi = _checkerPtr->pathCheckerMap.find(path);
  if(pcpi != _checkerPtr->pathCheckerMap.end() ){
    PathPtr pmi = pcpi->second->find(key);
    if(pmi){
      //found the same key, return immediately
      MCE_WARN("CheckerManagerI::add path: " << path << "key: " << key << " find the same key, path: " << path << " step: " << step
          << " replica: " << replica << " checksum: " << checksum);
      return;
    }
    PathPtr pa = NULL;
    {
      IceUtil::RWRecMutex::RLock lock(_configMutex);
      //(_config->getConfig()[path])->display("config",key);
      pa = new Path(*_config->getConfig()[path]);
    }
    pa->set(key,step, replica, checksum);
    pcpi->second->add(key,replica,pa);
  }
}

void CheckerManagerI::notify(const string& path, const string& step, const string& replica, 
    const string& key, const string& checksum, const Ice::Current& current){
  MCE_INFO("CheckerManagerI::notify key: " << key << " path:" << path <<" step:" << step 
      << " replica:" << replica << " checksum:" << checksum );	
  Entry entry;
  entry.path = path;
  entry.step = step;
  entry.replica = replica;
  entry.key = key;
  entry.checksum = checksum;

  {
    IceUtil::Mutex::Lock lock(_notifyMutex);
    _notify.push_back(entry);
  }
}

void CheckerManagerI::add(const Entry& entry){
  PathCheckerMapIter pcpi = _checkerPtr->pathCheckerMap.find(entry.path);
  if(pcpi != _checkerPtr->pathCheckerMap.end() ){
    PathPtr pmi = pcpi->second->find(entry.key);
    if(pmi){
      //found the same key, return immediately
      MCE_WARN("CheckerManagerI::add path: " << entry.path << "key: " << entry.key << " find the same key, path: " << entry.path << " step: " << entry.step
          << " replica: " << entry.replica << " checksum: " << entry.checksum);
      return;
    }
    PathPtr pa = NULL;
    {
      IceUtil::RWRecMutex::RLock lock(_configMutex);
      //(_config->getConfig()[path])->display("config",key);
      pa = new Path(*_config->getConfig()[entry.path]);
    }
    pa->set(entry.key,entry.step, entry.replica, entry.checksum);
    pcpi->second->add(entry.key,entry.replica,pa);
  }
}

void CheckerManagerI::addOne(const Entry& entry, const Ice::Current& current){
  MCE_INFO( "CheckerManagerI::addOne key: " << entry.key << " path:" << entry.path << " step:" << entry.step
      << " replica:" << entry.replica << " checksum:" << entry.checksum );
  add(entry);
}

void CheckerManagerI::notify(const Entry& entry ){
  IceUtil::Mutex::Lock lock(_notifyMutex);
  _notify.push_back(entry);
}

void CheckerManagerI::notifyOne(const Entry& entry, const Ice::Current& current){
  MCE_INFO("CheckerManagerI::notifyOne key: " << entry.key << " path:" << entry.path <<" step:" << entry.step
      << " replica:" << entry.replica << " checksum:" << entry.checksum );
  notify(entry);
}

void CheckerManagerI::addBatch(const std::vector<Entry>& batch, const Ice::Current& current) {
  MCE_INFO( "CheckerManagerI::addBatch size: " << batch.size() );
  for(std::vector<Entry>::const_iterator it = batch.begin(); it!= batch.end(); ++it){
    MCE_INFO( "CheckerManagerI::addBatch key: " << it->key << " path:" << it->path << " step:" << it->step
        << " replica:" << it->replica << " checksum:" << it->checksum );
    add(*it);
  }
}

void CheckerManagerI::notifyBatch(const std::vector<Entry>& batch, const Ice::Current& current) {
  MCE_INFO( "CheckerManagerI::notifyBatch size: " << batch.size() );
  for(std::vector<Entry>::const_iterator it = batch.begin(); it!= batch.end(); ++it){
    MCE_INFO( "CheckerManagerI::notifyBatch key: " << it->key << " path:" << it->path << " step:" << it->step
        << " replica:" << it->replica << " checksum:" << it->checksum );
    notify(*it);
  }
}

bool CheckerManagerI::reloadConfig(const Ice::Current& current){
  bool sequencevalid = false;
  bool success = ParserHelper::parse(_filename, sequencevalid);
  displayConfig();
  setChecker(sequencevalid);
  return success;
}
