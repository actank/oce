#ifndef ADINDEX_H_
#define ADINDEX_H_
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Mutex.h>
#include "Singleton.h"

using namespace boost::multi_index;
using namespace MyUtil;

const size_t ItemSize = 5000;
class AdDimIndex : public IceUtil::Shared {
public:
  long groupid_;
  int index_;
};
typedef IceUtil::Handle<AdDimIndex> AdDimIndexPtr;

typedef boost::multi_index_container<AdDimIndexPtr, indexed_by<ordered_unique<
    BOOST_MULTI_INDEX_MEMBER(AdDimIndex, long, groupid_)> , ordered_unique<
    BOOST_MULTI_INDEX_MEMBER(AdDimIndex, int, index_)> > > IndexContainer;
typedef IndexContainer::nth_index<0>::type AidIndex;
typedef IndexContainer::nth_index<1>::type DimIndex;

class IndexCache : public IceUtil::Shared  { //  : public Singleton<IndexCache>
public:
  IndexCache() :
    curindex_(-1) {
  }
  IndexCache(const IndexCache & index) :
    container_(index.container_), curindex_(index.curindex_) {
  }
//  IndexCache operator =(const IndexCache& right) const {
//    return right;
//  }
  long GetAdId(int index);
  int GetIndex(long id);
  int FindIndex(long groupid);
  int ReGetIndex(long groupid);
  int GetMaxIndex();
  void PrintCache(long groupid,string str);
  IndexContainer GetIndexImage() {
    return container_;
  }
  int size() {
    return container_.size();
  }
private:
//  typedef boost::multi_index_container<AdDimIndexPtr, indexed_by<hashed_unique<
//      BOOST_MULTI_INDEX_MEMBER(AdDimIndex, long, groupid_)> , hashed_unique<
//      BOOST_MULTI_INDEX_MEMBER(AdDimIndex, int, index_)> > > IndexContainer;

  IndexContainer container_;
//  typedef IndexContainer::nth_index<0>::type AidIndex;
//  typedef IndexContainer::nth_index<1>::type DimIndex;
  int curindex_;
  IceUtil::Mutex mutex_;
};

typedef IceUtil::Handle<IndexCache> IndexCachePtr;

#endif
