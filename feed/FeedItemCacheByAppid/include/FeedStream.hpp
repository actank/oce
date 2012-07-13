
/*
 * FeedStream.h
 *
 *  Created on: Aug 31, 2011
 *      Author: happy, guanghe.ge@renren-inc.com
 */

#ifndef __FEEDSTREAM_HPP__
#define __FEEDSTREAM_HPP__

#include <map>
#include <deque>
#include <vector>
#include <asm/atomic.h>
#include <util/cpp/TimeCost.h>

#include <Ice/Ice.h>
#include <IceUtil/RWRecMutex.h>
#include <IceLogger.h>

#include <pthread.h>

namespace xce {
namespace feed {

using namespace MyUtil;

const int FEEDSTREAM_MUTEX_NUM = 1000;

template<typename Data>
struct Less {
	bool operator()(const Data& a, const Data& b) {
		return a < b;
	}
};

/*
 *结构内部不再加锁，减少内存占用，用外部加锁解决并发
 */
template<typename FeedData, typename Less = Less<FeedData> >
struct MutexData {
	typedef std::vector<FeedData> DataDeqType;

	bool is_order;
	DataDeqType data;

	MutexData() : is_order(false) {
	}
	~MutexData() {
	} 

	inline bool order() {
		return is_order;
	}

	inline void set_order(bool order) {
		is_order = order;
	}

	inline bool add(const FeedData& feed){
		if(!data.empty() && data[data.size() - 1] ==  feed) {
			return false;
		}

		data.push_back(feed);
		return true;
	}

	inline int free(const FeedData& upper_bound) {
		int count = 0;
		for(typename DataDeqType::iterator it = data.begin();
				it != data.end() && *it < upper_bound; ++it) {
			count++;
		}
		data.erase(data.begin(), data.begin() + count);
		return count;
	}

	inline bool get_min(FeedData& output) {
		if(data.empty()) {
			return false;
		}
		output = data[0];
		return true;
	}

	inline int size() {
		return data.size();
	}

	inline void check_order(){
		std::sort(data.begin(), data.end(), Less());
	}
};//end MutexData

//---------------------------------------------------------------------
//
template<typename FeedData, typename Less = Less<FeedData> >
class FeedStream {

typedef MutexData<FeedData, Less> MutexDataType;
typedef std::map<int, MutexDataType*> UserMapType;

public:
	FeedStream() {
		atomic_set( &total_count_, 0 );
		
		user_mutexes_.reserve(FEEDSTREAM_MUTEX_NUM);
		for (int i = 0; i < FEEDSTREAM_MUTEX_NUM; i++) {
			pthread_rwlock_t mutex;
			pthread_rwlockattr_t attr;
			int ret;
			ret = pthread_rwlock_init(&mutex, &attr);

			user_mutexes_.push_back(mutex);
		}
	}
	~FeedStream() {
		{
			IceUtil::RWRecMutex::WLock lock(mutex_);
			for(typename UserMapType::iterator i = usermap_.begin(); i != usermap_.end(); ++i) {
				delete i->second;
			}
			usermap_.clear();
		}

		user_mutexes_.clear();
	}

	void check_order() {
		typedef std::vector<MutexDataType*> MdsType;
		IceUtil::RWRecMutex::RLock lock(mutex_);
		for(typename UserMapType::const_iterator it = usermap_.begin(); it != usermap_.end(); ++it) {
			MutexDataType* md = it->second;
			pthread_rwlock_wrlock(&user_mutexes_[it->first%FEEDSTREAM_MUTEX_NUM]);
			if (!md->order()) {
				md->check_order();
				md->set_order(true);
			}
			pthread_rwlock_unlock(&user_mutexes_[it->first%FEEDSTREAM_MUTEX_NUM]);
		}
	}

	void add_count(int count) {
		atomic64_add(count, &total_count_);
	}

	int user_count() const {
		IceUtil::RWRecMutex::RLock lock(mutex_);
		return usermap_.size();
	}

	long total_count() const {
		return atomic64_read(&total_count_);
	}

	int item_count(int userId) {
		IceUtil::RWRecMutex::RLock lock(mutex_);
		typename UserMapType::const_iterator it = usermap_.find(userId);
		if(it != usermap_.end()) {
			pthread_rwlock_rdlock(&user_mutexes_[userId%FEEDSTREAM_MUTEX_NUM]);
			int result = it->second->size();
			pthread_rwlock_unlock(&user_mutexes_[userId%FEEDSTREAM_MUTEX_NUM]);
			return result;
		}
		return 0;
	}

	bool get_min(FeedData& output) {
		bool init = false;
		FeedData tmp;

		IceUtil::RWRecMutex::RLock lock(mutex_);
		for(typename UserMapType::const_iterator it = usermap_.begin(); it != usermap_.end(); ++it) {
			MutexDataType * md = it->second;
			pthread_rwlock_rdlock(&user_mutexes_[it->first%FEEDSTREAM_MUTEX_NUM]);
			if(!init) {
				init = md->get_min(output);
			} else {
				if(md->get_min(tmp) && Less()(tmp, output)) {
					output = tmp;
				}
			}
			pthread_rwlock_unlock(&user_mutexes_[it->first%FEEDSTREAM_MUTEX_NUM]);
		}
		return init;
	}

	void free(const FeedData& upper_bound) {
		typedef std::vector<MutexDataType*> MdsType;
		int count = 0;
		IceUtil::RWRecMutex::RLock lock(mutex_);
		for(typename UserMapType::const_iterator it = usermap_.begin(); it != usermap_.end(); ++it) {
			MutexDataType* md = it->second;
			pthread_rwlock_wrlock(&user_mutexes_[it->first%FEEDSTREAM_MUTEX_NUM]);
			count += md->free(upper_bound);
			pthread_rwlock_unlock(&user_mutexes_[it->first%FEEDSTREAM_MUTEX_NUM]);
		}

		if(count > 0) {
			MCE_INFO("FeedStream::free, count: " << count);
			atomic64_sub(count, &total_count_);
		}
	}

	bool put(int uid, const FeedData& feed, bool addCount = true) {
		MutexDataType* md = NULL;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			typename UserMapType::const_iterator it = usermap_.find(uid);
			if(it != usermap_.end()) {
				md = it->second;
				assert(md);
			}
		}

		if(md == NULL) {
			std::pair<typename UserMapType::iterator, bool> p;
			{
				IceUtil::RWRecMutex::WLock lock(mutex_);
				if (NULL == md) {
					 p = usermap_.insert(std::make_pair(uid, (MutexDataType*)(new MutexDataType)));
				}
			}

			md = p.first->second;
		}

		pthread_rwlock_wrlock(&user_mutexes_[uid%FEEDSTREAM_MUTEX_NUM]);
		bool result = md->add(feed);
		pthread_rwlock_unlock(&user_mutexes_[uid%FEEDSTREAM_MUTEX_NUM]);

		if(result && addCount) {
			atomic64_inc(&total_count_);
		}

		return result;
	}

	bool puts(int mod, const std::vector<FeedData>& feeds, bool addCount = true) {
		//校验元素
		for (typename std::vector<FeedData>::const_iterator it = feeds.begin();
				it != feeds.end(); ++it) {
			int uid = (*it).userid;
			typename UserMapType::const_iterator it;
			{
				IceUtil::RWRecMutex::RLock lock(mutex_);
				it = usermap_.find(uid);
			}

			if(it == usermap_.end()) {
				IceUtil::RWRecMutex::WLock lock(mutex_);
				usermap_.insert(std::make_pair(uid, (MutexDataType*)(new MutexDataType)));
			}
		}

		//成批插入OA
		pthread_rwlock_wrlock(&user_mutexes_[mod%FEEDSTREAM_MUTEX_NUM]);
		for (typename std::vector<FeedData>::const_iterator it = feeds.begin();
				it != feeds.end(); ++it) {
			int uid = (*it).userid;
			MutexDataType* md = NULL;
			typename UserMapType::const_iterator im;
			{
				IceUtil::RWRecMutex::RLock lock(mutex_);
				im = usermap_.find(uid);
			}

			if (im != usermap_.end()) {
				md = im->second;
				md->add(*it);
			}
		}
		pthread_rwlock_unlock(&user_mutexes_[mod%FEEDSTREAM_MUTEX_NUM]);
		return true;
	}

	typename std::vector<FeedData> get(int uid) const {
		MutexDataType* md;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			typename UserMapType::const_iterator it = usermap_.find(uid);
			if(it == usermap_.end()) {
				return std::vector<FeedData>();
			}
			md = it->second;
		}
		assert(md);

		if(md) {
			pthread_rwlock_rdlock(&user_mutexes_[uid%FEEDSTREAM_MUTEX_NUM]);
			std::vector<FeedData> result = std::vector<FeedData>(md->data.rbegin(), md->data.rend());
			pthread_rwlock_unlock(&user_mutexes_[uid%FEEDSTREAM_MUTEX_NUM]);
			return result;
		}
		return std::vector<FeedData>();
	}

	typename std::vector<FeedData> getGreaterThanFeedid(int appid, long feedid, int limit) {
		MutexDataType* md;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			typename UserMapType::const_iterator it = usermap_.find(appid);
			if(it == usermap_.end()) {
				return std::vector<FeedData>();
			}
			md = it->second;
		}
		assert(md);

		if(md) {
			pthread_rwlock_rdlock(&user_mutexes_[appid%FEEDSTREAM_MUTEX_NUM]);
			std::vector<FeedData> result;
			for (typename MutexDataType::DataDeqType::const_reverse_iterator it = md->data.rbegin();
					it != md->data.rend(); ++it) {
				if (it->feedid > feedid) {
					result.push_back(*it);
				}

				if (result.size() >= (uint)limit) break; 
			}
			pthread_rwlock_unlock(&user_mutexes_[appid%FEEDSTREAM_MUTEX_NUM]);
			return result;
		}
		return std::vector<FeedData>();
	}

	template<typename TypeFilter>
	typename std::vector<FeedData> getGreaterThanFeedidWithFriendsByTypes(int appid, long feedid, int limit, 
			FriendFilter friends, TypeFilter types) {
		MutexDataType* md;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			typename UserMapType::const_iterator it = usermap_.find(appid);
			if(it == usermap_.end()) {
				return std::vector<FeedData>();
			}
			md = it->second;
		}
		assert(md);

		if(md) {
			pthread_rwlock_rdlock(&user_mutexes_[appid%FEEDSTREAM_MUTEX_NUM]);
			std::vector<FeedData> result;
			for (typename MutexDataType::DataDeqType::const_reverse_iterator it = md->data.rbegin();
					it != md->data.rend(); ++it) {

				if (it->feedid > feedid && friends(*it) && types(*it)) {
					result.push_back(*it);
				}

				if (result.size() >= (uint)limit) break; 
			}
			pthread_rwlock_unlock(&user_mutexes_[appid%FEEDSTREAM_MUTEX_NUM]);
			return result;
		}
		return std::vector<FeedData>();
	}

	typename std::vector<FeedData> getLessThanFeedid(int appid, long feedid, int limit) {
		MutexDataType* md;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			typename UserMapType::const_iterator it = usermap_.find(appid);
			if(it == usermap_.end()) {
				return std::vector<FeedData>();
			}
			md = it->second;
		}
		assert(md);

		if(md) {
			pthread_rwlock_rdlock(&user_mutexes_[appid%FEEDSTREAM_MUTEX_NUM]);
			std::vector<FeedData> result;
			for (typename MutexDataType::DataDeqType::const_reverse_iterator it = md->data.rbegin();
					it != md->data.rend(); ++it) {
				if (it->feedid < feedid) {
					result.push_back(*it);
				}

				if (result.size() >= (uint)limit) break; 
			}
			pthread_rwlock_unlock(&user_mutexes_[appid%FEEDSTREAM_MUTEX_NUM]);
			return result;
		}
		return std::vector<FeedData>();
	}

	typename std::vector<std::vector<FeedData> > gets(const std::vector<int>& uids, size_t limit = 0xffffffff) {
		typedef std::vector<int> UidsType;
		typedef std::map<int, MutexDataType*> MdsMapType;
		MdsMapType mds;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			for(typename UidsType::const_iterator i = uids.begin(); i != uids.end(); ++i) {
				typename UserMapType::const_iterator it = usermap_.find(*i);
				mds[*i] = (it != usermap_.end() ? it->second : NULL);
			}
		}

		std::vector<std::vector<FeedData> > v;
		v.reserve(uids.size());
		for(typename MdsMapType::const_iterator i = mds.begin(); i != mds.end(); ++i) {
			MutexDataType* md = i->second;
			if(md) {
				pthread_rwlock_rdlock(&user_mutexes_[i->first%FEEDSTREAM_MUTEX_NUM]);
				if (md->data.size() > limit && !md->order()) {
					md->check_order();
				}

				v.push_back(typename std::vector<FeedData>(md->data.rbegin(), md->data.rbegin() + std::min(limit, md->data.size())));
				pthread_rwlock_unlock(&user_mutexes_[i->first%FEEDSTREAM_MUTEX_NUM]);
			} else {
				v.push_back(std::vector<FeedData>());
			}
		}
		return v;
	}

	template<typename TypeFilter, typename IdsFilter>
	typename std::vector<std::vector<FeedData> > gets_with_filter(const std::vector<int>& uids, TypeFilter types, IdsFilter ids, size_t limit = 0xffffffff) {

		typedef std::vector<int> UidsType;
		typedef std::map<int, MutexDataType*> MdsMapType;
		MdsMapType mds;
		{
			IceUtil::RWRecMutex::RLock lock(mutex_);
			for(typename UidsType::const_iterator i = uids.begin(); i != uids.end(); ++i) {
				typename UserMapType::const_iterator it = usermap_.find(*i);
				mds[*i] = (it != usermap_.end() ? it->second : NULL);
			}
		}
		
		std::vector<std::vector<FeedData> > v;
		v.reserve(uids.size());

		for(typename MdsMapType::const_iterator i = mds.begin(); i != mds.end(); ++i) {
			MutexDataType* md = i->second;
			if(md) {
				pthread_rwlock_rdlock(&user_mutexes_[i->first%FEEDSTREAM_MUTEX_NUM]);
				if (md->data.size() > limit && !md->order()) {
					md->check_order();
				}

				size_t lim = std::min(limit, md->data.size());
				std::vector<FeedData> vf;
				vf.reserve(lim);
				for(typename MutexDataType::DataDeqType::const_reverse_iterator it = md->data.rbegin(); lim > 0 && it != md->data.rend(); ++it) {
					if(types(*it) && !ids(*it)) {
						vf.push_back(*it);
						--lim;
					}
				}
				v.push_back(vf);
				pthread_rwlock_unlock(&user_mutexes_[i->first%FEEDSTREAM_MUTEX_NUM]);
			} else {
				v.push_back(std::vector<FeedData>());
			}
		}

		return v;
	}

private:

	atomic64_t total_count_;

	IceUtil::RWRecMutex mutex_;
	
	std::vector<pthread_rwlock_t> user_mutexes_;

	UserMapType usermap_;

};//end FeedStreams

}
}

#endif // __FEEDSTREAM_H__
