
#ifndef SharedFeedCACHE_H_
#define SharedFeedCACHE_H_

#include <ext/hash_map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <Singleton.h>
#include "RFeed.h"
#include "LogWrapper.h"
#include "TaskManager.h"
#include <boost/foreach.hpp>
#include <boost/pool/object_pool.hpp>
namespace xce {
namespace feed {

using namespace std;
using namespace __gnu_cxx;
using namespace MyUtil;

//typedef IceUtil::Handle<SharedFeed> SharedFeed*;
class SharedFeed;
class SharedFeedPtr;
typedef hash_map<long, SharedFeed*> FeedMap;//SharedFeedCache保存的是裸指针
const static int kSplitterStype = 9901;
class SharedFeedCache : public MyUtil::Singleton<SharedFeedCache>{
public:
	static const int kPoolSize = 1;
	static const int kDumpInteval = 1000 * 10;

	SharedFeedCache():ctor_(0),dtor_(0){}

	SharedFeedPtr Add(const FeedItem& item);
	SharedFeedPtr Get(Ice::Long feed_id);

	void    Inc(SharedFeed* feed);
	void    Dec(SharedFeed* feed);

	int     TotalCount() const ;
  string  ToString() const {
  	ostringstream os;
  	os << "SharedFeedCache: " << DumpMaps()
  			<< " ctor:" << ctor_ << " dtor:" << dtor_ ;
  	return os.str();
  }

private:
  string DumpMaps() const;
	int GetIndex(Ice::Long fid) const { return abs(fid) % kPoolSize; }

	IceUtil::Mutex mutex_[kPoolSize];
	FeedMap feed_maps_[kPoolSize];
	boost::object_pool<SharedFeed> pools_[kPoolSize];

public:
	void Ctor(){
	  IceUtil::Mutex::Lock lock(stat_mutex_);
	  ++ctor_;
	}
	void Dtor(){
		++dtor_;
	}
private:
	long ctor_,dtor_;
	IceUtil::Mutex stat_mutex_;
};

class SharedFeedPtr;
class SharedFeed: public Ice::Object {
public:
	Ice::Long id_;
	Ice::Long merge_;
	int type_;
	int actor_;
	time_t time_;
	int ref_count_;
public:
	SharedFeed(const SharedFeed& f):
	 id_(f.id_),
	 merge_(f.merge_),
	 type_(f.type_),
	 actor_(f.actor_),
	 time_(f.time_),
	 ref_count_(0)
	{
		SharedFeedCache::instance().Ctor();
	}
	SharedFeed(const FeedItem& item) :
		id_ (item.feed),
		merge_ (item.merge),
		type_ ( (unsigned int) item.type),
		actor_ ( item.actor),
		time_ ( item.time),
		ref_count_ (0)
	{
		SharedFeedCache::instance().Ctor();
	}
	virtual ~SharedFeed(){
		SharedFeedCache::instance().Dtor();
	}
	Ice::Long GetFid()const{ return id_; }
	Ice::Long GetMerge()const { return merge_; }
	int GetStype() const { return type_ & 0xFFFF; }
	int GetOrigType()const {return type_;}
	int Ver(){ return (type_&0xFF0000)>>16; }
	int GetFeedNewsMergeType() const { return (((unsigned int)type_) >> 30); }

	pair<int,long> GetStypeMerge()const{	return make_pair(GetStype(),merge_); }
	bool SameStypeMerge(int stype,long merge)const{ return GetStype() == stype && merge == merge_;}
	pair<int,int> GetStypeActor()const{ return make_pair(GetStype(),actor_); }

	bool IsEdm()const{ return time_ < 0 ||time_ > time(NULL); }
	bool IsRandomType()const{ return (GetStype() == 601); }
	int IsMergeReplace()const {return GetFeedNewsMergeType() == FeedMergeReplace;}
	int IsMergeAppend()const {return GetFeedNewsMergeType() == FeedMergeAppend;}

	int GetActor(){ return actor_; }

	bool IsSplitter() const { return kSplitterStype == type_;}
	long GetTime(){return time_;}


	//-------------------------------------------------------------------------
//	void IncRef() {
//		SharedFeedCache::instance().Inc(this);
//	}
//	void DecRef() {
//		SharedFeedCache::instance().Dec(this);
//	}

	//-------------------------------------------------------------------------

};





class SharedFeedPtr{
public:
	SharedFeedPtr(SharedFeed* sfp = 0){
		sfp_ = sfp;
		if(sfp) SharedFeedCache::instance().Inc(sfp_);
	}

	SharedFeedPtr(const SharedFeedPtr & sfh){
		sfp_ = sfh.sfp_;
		if(sfp_) SharedFeedCache::instance().Inc(sfp_);
	}

	virtual ~SharedFeedPtr(){
		if(sfp_) SharedFeedCache::instance().Dec(sfp_);
	}

	SharedFeed* operator->() const { return sfp_; }

	SharedFeedPtr & operator=(const SharedFeedPtr & other){
		if(sfp_ != other.sfp_){
			if(other.sfp_) {
				SharedFeedCache::instance().Inc(other.sfp_);
			}
			SharedFeed* tmp = this->sfp_;
			this->sfp_ = other.sfp_;
			if(tmp){
				SharedFeedCache::instance().Dec(tmp);
			}
		}
		return *this;
	}
	SharedFeed* GetRawPtr() const { return sfp_;}
//	SharedFeedPtr operator=(const SharedFeedPtr & sfp){
//		if(sfp_ != sfp){
//			if(sfp) {
//				SharedFeedCache::instance().Inc(sfp);
//			}
//			SharedFeedPtr tmp = this->sfp_;
//			this->sfp_ = sfp;
//			if(tmp){
//				SharedFeedCache::instance().Dec(tmp);
//			}
//		}
//		return *this;
//	}

	SharedFeed* sfp_;
};

typedef vector<SharedFeedPtr> SFeedPtr1D;
typedef vector<SFeedPtr1D> SFeedPtr2D;
typedef vector<SFeedPtr2D> SFeedPtr3D;




}
}


#endif /* SharedFeedCACHE_H_ */
