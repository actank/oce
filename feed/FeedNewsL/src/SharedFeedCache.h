
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
class SharedFeed/*: public Ice::Object*/ {
public:
	Ice::Long id_;
	Ice::Long merge_;
	int type_;
	int actor_;
//	time_t time_;
	int ref_count_;
public:
	SharedFeed(const SharedFeed& f):
	 id_(f.id_),
	 merge_(f.merge_),
	 type_(f.type_),
	 actor_(f.actor_),
//	 time_(f.time_),
	 ref_count_(0)
	{
		SharedFeedCache::instance().Ctor();
	}
	SharedFeed(const FeedItem& item) :
		id_ (item.feed),
		merge_ (item.merge),
		type_ ( (unsigned int) item.type),
		actor_ ( item.actor),
//		time_ ( item.time),
		ref_count_ (0)
	{
		SharedFeedCache::instance().Ctor();
	}
	virtual ~SharedFeed(){
		SharedFeedCache::instance().Dtor();
	}
	Ice::Long GetFid() const { return id_; }
	Ice::Long GetMerge() const { return merge_; }
	int GetStype() const { return type_ & 0xFFFF; }
	int GetVersionStype() const { return type_ & 0xFFFFFF; }
	int GetOrigType()const {return type_;}
	int GetVersion(){ return (type_&0xFF0000)>>16; }
	int GetFeedNewsMergeType() const { return (((unsigned int)type_) >> 30); }

	pair<int,long> GetStypeMerge()const{	return make_pair(GetStype(),merge_); }
	pair<int,long> GetVersionStypeMerge()const{	return make_pair(GetVersionStype(),merge_); }
	bool SameStypeMerge(int stype,long merge)const{ return GetStype() == stype && merge == merge_;}
	pair<int,int> GetStypeActor()const{ return make_pair(GetStype(),actor_); }

//	bool IsEdm()const{ return time_ < 0 ||time_ > time(NULL); }
	bool IsRandomType()const{ return (GetStype() == 601); }
	int IsMergeReplace()const {return GetFeedNewsMergeType() == FeedMergeReplace;}
	int IsMergeAppend()const {return GetFeedNewsMergeType() == FeedMergeAppend;}

	int GetActor(){ return actor_; }




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
		raw_ptr_ = sfp;
		if(sfp) SharedFeedCache::instance().Inc(raw_ptr_);
	}

	SharedFeedPtr(const SharedFeedPtr & sfh){
		raw_ptr_ = sfh.raw_ptr_;
		if(raw_ptr_) SharedFeedCache::instance().Inc(raw_ptr_);
	}

	virtual ~SharedFeedPtr(){
		if(raw_ptr_) SharedFeedCache::instance().Dec(raw_ptr_);
	}

	SharedFeed* operator->() const { return raw_ptr_; }

	SharedFeedPtr & operator=(const SharedFeedPtr & other){
		if(raw_ptr_ != other.raw_ptr_){
			if(other.raw_ptr_) {
				SharedFeedCache::instance().Inc(other.raw_ptr_);
			}
			SharedFeed* tmp = this->raw_ptr_;
			this->raw_ptr_ = other.raw_ptr_;
			if(tmp){
				SharedFeedCache::instance().Dec(tmp);
			}
		}
		return *this;
	}
//	SharedFeedPtr operator=(const SharedFeedPtr & sfp){
//		if(raw_ptr_ != sfp){
//			if(sfp) {
//				SharedFeedCache::instance().Inc(sfp);
//			}
//			SharedFeedPtr tmp = this->raw_ptr_;
//			this->raw_ptr_ = sfp;
//			if(tmp){
//				SharedFeedCache::instance().Dec(tmp);
//			}
//		}
//		return *this;
//	}

	SharedFeed* raw_ptr_;
};

typedef vector<SharedFeedPtr> SFeed1D;
typedef vector<SFeed1D> SFeed2D;
typedef vector<SFeed2D> SFeed3D;




}
}


#endif /* SharedFeedCACHE_H_ */
