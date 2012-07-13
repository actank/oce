#include "AdIndex.h"
#include "AdCache.h"
using namespace MyUtil;
using namespace xce::ad;
/*int IndexCache::GetIndex(long groupid){//查找并分配新的index
 //MCE_DEBUG("IndexCache::GetIndex-->aid: "<< aid);
 IceUtil::Mutex::Lock lock(mutex_);
 AidIndex& aindex = container_.get<0>();
 AidIndex::iterator ait = aindex.find(groupid);
 if(ait != aindex.end()) {
 return (*ait).index_;
 }
 curindex_++;

 AdDimIndex index;
 index.groupid_ = groupid;
 index.index_ = curindex_;
 container_.insert(index);
 //MCE_DEBUG("IndexCache::GetIndex-->aid: "<< aid <<"curindex: "<< curindex_);

 return curindex_;
 //return 0;
 }*/
int IndexCache::FindIndex(long groupid) {//在cache中查找groupid，如果不存在，不新分配index;
  IceUtil::Mutex::Lock lock(mutex_);
  AidIndex& aindex = container_.get<0> ();
  AidIndex::iterator ait = aindex.find(groupid);
  if (ait != aindex.end()) {
    return (*ait)->index_;
  }
  return -2;
}

int IndexCache::GetIndex(long groupid) {
  //MCE_DEBUG("IndexCache::GetIndex-->aid: "<< aid);
  IceUtil::Mutex::Lock lock(mutex_);
  MCE_DEBUG("IndexCache::GetIndex-->aid: "<< groupid);
  AidIndex& aindex = container_.get<0> ();
  AidIndex::iterator ait = aindex.find(groupid);
  if (ait != aindex.end()) {
    MCE_DEBUG("IndexCache::GetIndex-->aid: in the cache "<< groupid);
    return (*ait)->index_;

  }
  curindex_++;
  if (curindex_ < ItemSize) {
    AdDimIndexPtr index = new AdDimIndex();
    index->groupid_ = groupid;
    index->index_ = curindex_;
    container_.insert(index);
    MCE_DEBUG("IndexCache::GetIndex-->index need increase: groupid:"
        << index->groupid_ << " index:" << index->index_);
  //  PrintCache(groupid, "insert");
  } else {
    curindex_ = ItemSize - 1;
    return -1;
  }
  return curindex_;
  //return 0;
}
void IndexCache::PrintCache(long groupid, string str) {
  AidIndex& aindex = container_.get<0> ();
  AidIndex::iterator ait = aindex.begin();
  string result;
  for(; ait != aindex.end(); ++ ait) {

    string str = boost::lexical_cast<string>((*ait)->groupid_);
    result += ",";
    result += str;

  }
  MCE_DEBUG("IndexCache::PrintCache " << str << ",groupid:" << groupid << ", cache:" << result);


}
/*int IndexCache::ReGetIndex(long groupid) {
 MCE_DEBUG("IndexCache::ReGetIndex--> groupid:" << groupid);
 IceUtil::Mutex::Lock lock(mutex_);
 int index = -1;
 AidIndex& gindex = container_.get<0>();
 AidIndex::iterator git = gindex.begin();
 for(; git != gindex.end(); ++git) {
 long id = (*git)->groupid_;

 int flag = AdGroupPool::instance().HasAdGroup(id);
 MCE_DEBUG("IndexCache::ReGetIndex--> groupid:" << id <<" valid:" << flag);

 if(!(AdGroupPool::instance().HasAdGroup(id))) {//找到过期的groupid

 index = (*git)->index_;
 (*git)->groupid_ = groupid;
 MCE_DEBUG("IndexCache::ReGetIndex--> groupid: need replace old groupid:" <<id <<" with new groupid:" << groupid << " at pos:" <<index);
 break;
 }


 }
 return index;
 }*/
int IndexCache::ReGetIndex(long groupid) {
  MCE_DEBUG("IndexCache::ReGetIndex--> groupid:" << groupid);
  IceUtil::Mutex::Lock lock(mutex_);

  ////debug
  {
    AidIndex& aindex = container_.get<0> ();
    AidIndex::iterator ait = aindex.find(groupid);
    if(ait != aindex.end()) {
      MCE_WARN("exist in container:groupid:" << groupid);
    }

  }
  int index = -1;
  AidIndex& aindex = container_.get<0> ();
  AidIndex::iterator ait = aindex.begin();
  for (; ait != aindex.end(); ++ait) {
    long id = (*ait)->groupid_;

//    int flag = AdGroupPool::instance().HasAdGroup(id);
  //  MCE_DEBUG("IndexCache::ReGetIndex--> groupid:" << id << " valid:" << flag);

    if (!(AdGroupPool::instance().HasAdGroup(id))) {//找到过期的groupid

/*     index = (*ait)->index_;
      (*ait)->groupid_ = groupid;
      AdDimIndexPtr ptr = *ait;
      ptr->groupid_ = groupid;
      aindex.replace(ait, ptr);*/
      index = (*ait)->index_;
      aindex.erase(ait);
      AdDimIndexPtr ptr = new AdDimIndex();
      ptr->groupid_ = groupid;
      ptr->index_ = index;
      container_.insert(ptr);


      MCE_DEBUG("IndexCache::ReGetIndex--> groupid: need replace old groupid:"
          << id << " with new groupid:" << groupid << " at pos:" << index);
     // PrintCache(groupid, "modify");
      break;
    }

  }
  return index;
}
long IndexCache::GetAdId(int index) {
  IceUtil::Mutex::Lock lock(mutex_);
  DimIndex& dindex = container_.get<1> ();
  DimIndex::iterator dit = dindex.find(index);
  if (dit != dindex.end()) {
    return (*dit)->groupid_;
  }
  return -1;
  //	return 0;
}
int IndexCache::GetMaxIndex() {
  IceUtil::Mutex::Lock lock(mutex_);
  return curindex_;
}
