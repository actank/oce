#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

#include "TaskManager.h"
#include "IceLogger.h"
#include "Singleton.h"
#include "FeedCacheI.h"

using namespace xce::feed;
class PerformanceStat : public MyUtil::Timer, public MyUtil::Singleton<PerformanceStat>{
public:
	static const int INTERVAL = 20*1000; //debug-------------------

	PerformanceStat() : Timer(INTERVAL, 1){
    _put_count = 0;
    _hit_get_count = 0;
    _load_get_count = 0;
    _reject_feed_count = 0;
    _creat_user_count = 0;

    _put_max_delay = 0;
    _put_min_delay = 0;
    _put_sum_delay = 0;

    _hit_get_max_delay = 0;
    _hit_get_min_delay = 0;
    _hit_get_sum_delay = 0;

    _load_get_max_delay = 0;
    _load_get_min_delay = 0;
    _load_get_sum_delay = 0;

    _reject_feed_max_delay = 0;
    _reject_feed_min_delay = 0;
    _reject_feed_sum_delay = 0;

    _create_user_max_delay = 0;
    _create_user_min_delay = 0;
    _create_user_sum_delay = 0;

    _remove_count = 0;
    _remove_user_count = 0;
    _remove_all_count = 0;
    _remove_by_merge_count = 0;
    _remove_merge_by_fid_count = 0;

    _get_ids_count = 0;
    _put_by_user_id_count = 0;
    _modify_count = 0;


    _remove_max_delay = 0;
    _remove_min_delay = 0;
    _remove_sum_delay = 0;

    _remove_user_max_delay = 0;
    _remove_user_min_delay = 0;
    _remove_user_sum_delay = 0;

    _remove_all_max_delay = 0;
    _remove_all_min_delay = 0;
    _remove_all_sum_delay = 0;

    _remove_by_merge_max_delay = 0;
    _remove_by_merge_min_delay = 0;
    _remove_by_merge_sum_delay = 0;

    _remove_merge_by_fid_max_delay = 0;
    _remove_merge_by_fid_min_delay = 0;
    _remove_merge_by_fid_sum_delay = 0;

    _get_ids_max_delay = 0;
    _get_ids_min_delay = 0;
    _get_ids_sum_delay = 0;

    _put_by_user_id_max_delay = 0;
    _put_by_user_id_min_delay = 0;
    _put_by_user_id_sum_delay = 0;

    _modify_max_delay = 0;
    _modify_min_delay = 0;
    _modify_sum_delay = 0;

	}

	void handle(){
    //MCE_DEBUG("PerformanceStat::handle");
		//IceUtil::Mutex::Lock lock(_mutex);
		pair<Ice::Long, Ice::Long> count = FeedCacheI::instance().queryCacheCount();
		MCE_INFO("### PerformanceStat ### users:" << count.first << " feeds:" << count.second 
      << " hit get:" << _hit_get_count 
			<< " max:" << _hit_get_max_delay << " min:" << _hit_get_min_delay << " avg:" << _hit_get_sum_delay/(_hit_get_count+1)
			<< " load get:" << _load_get_count
			<< " max:" << _load_get_max_delay << " min:" << _load_get_min_delay << " avg:" << _load_get_sum_delay/(_load_get_count+1)
      << " reject feed count:" << _reject_feed_count 			
      << " max:" << _reject_feed_max_delay << " min:" << _reject_feed_min_delay << " avg:" << _reject_feed_sum_delay/(_reject_feed_count+1) 
      << " create user count:" << _creat_user_count 
      << " max:" << _create_user_max_delay << " min:" << _create_user_min_delay << " avg:" << _create_user_sum_delay/(_creat_user_count + 1)
      << " put count:" << _put_count
      << " max:" << _put_max_delay << " min:" << _put_min_delay << " avg:" << _put_sum_delay/(_put_count + 1)

      << " remove count:" << _remove_count
      << " max:" << _remove_max_delay << " min:" << _remove_min_delay << " avg:" << _remove_sum_delay/(_remove_count + 1)
      << " remove_all count:" << _remove_all_count
      << " max:" << _remove_all_max_delay << " min:" << _remove_all_min_delay << " avg:" << _remove_all_sum_delay/(_remove_all_count + 1)
      << " remove_user count:" << _remove_user_count
      << " max:" << _remove_user_max_delay << " min:" << _remove_user_min_delay << " avg:" << _remove_user_sum_delay/(_remove_user_count + 1)
      << " remove_by_merge count:" << _remove_by_merge_count
      << " max:" << _remove_by_merge_max_delay << " min:" << _remove_by_merge_min_delay << " avg:" << _remove_by_merge_sum_delay/(_remove_by_merge_count + 1)
      << " remove_merge_by_fid count:" << _remove_merge_by_fid_count
      << " max:" << _remove_merge_by_fid_max_delay << " min:" << _remove_merge_by_fid_min_delay << " avg:" << _remove_merge_by_fid_sum_delay/(_remove_merge_by_fid_count + 1)
      << " get_ids count:" << _get_ids_count
      << " max:" << _get_ids_max_delay << " min:" << _get_ids_min_delay << " avg:" << _get_ids_sum_delay/(_get_ids_count + 1)
      << " put_by_user_id count:" << _put_by_user_id_count
      << " max:" << _put_by_user_id_max_delay << " min:" << _put_by_user_id_min_delay << " avg:" << _put_by_user_id_sum_delay/(_put_by_user_id_count + 1)
      << " modify count:" << _modify_count
      << " max:" << _modify_max_delay << " min:" << _modify_min_delay << " avg:" << _modify_sum_delay/(_modify_count + 1);

			FeedCacheI::instance().printThreadPoolInfoByLevel();

	  );	

    _put_count = 0;
		_hit_get_count = 0;
		_load_get_count = 0;
		_reject_feed_count = 0;
    _creat_user_count = 0;

    _put_max_delay = 0;
    _put_min_delay = 0;
    _put_sum_delay = 0;
	
		_hit_get_max_delay = 0;
		_hit_get_min_delay = 0;
		_hit_get_sum_delay = 0;
   

		_load_get_max_delay = 0;
		_load_get_min_delay = 0;
		_load_get_sum_delay = 0;

    _reject_feed_max_delay = 0;
    _reject_feed_min_delay = 0;
    _reject_feed_sum_delay = 0;

    _create_user_max_delay = 0;
    _create_user_min_delay = 0;
    _create_user_sum_delay = 0;

   _remove_count = 0;
   _remove_user_count = 0;
   _remove_all_count = 0;
   _remove_by_merge_count = 0;
   _remove_merge_by_fid_count = 0;

   _get_ids_count = 0;
   _put_by_user_id_count = 0;
   _modify_count = 0;


	 _remove_max_delay = 0;
	 _remove_min_delay = 0;
	 _remove_sum_delay = 0;
 
	 _remove_user_max_delay = 0;
	 _remove_user_min_delay = 0;
	 _remove_user_sum_delay = 0;

	 _remove_all_max_delay = 0;
	 _remove_all_min_delay = 0;
	 _remove_all_sum_delay = 0;

	 _remove_by_merge_max_delay = 0;
	 _remove_by_merge_min_delay = 0;
	 _remove_by_merge_sum_delay = 0;

	 _remove_merge_by_fid_max_delay = 0;
	 _remove_merge_by_fid_min_delay = 0;
	 _remove_merge_by_fid_sum_delay = 0;

	 _get_ids_max_delay = 0;
	 _get_ids_min_delay = 0;
	 _get_ids_sum_delay = 0;

   _put_by_user_id_max_delay = 0;
   _put_by_user_id_min_delay = 0;
   _put_by_user_id_sum_delay = 0;

	 _modify_max_delay = 0;
	 _modify_min_delay = 0;
	 _modify_sum_delay = 0;

	}

  void incPut(float delay){
    IceUtil::Mutex::Lock lock(_mutex);
    _put_count++;
		_put_sum_delay += delay;
		if(delay > _put_max_delay || _put_max_delay == 0){
			_put_max_delay = delay;
		}
		if(delay < _put_min_delay || _put_min_delay == 0){
			_put_min_delay = delay;
		}
  }

  void incHitGet(float delay){
    IceUtil::Mutex::Lock lock(_mutex);
    _hit_get_count++;
    _hit_get_sum_delay += delay;
    if(delay > _hit_get_max_delay || _hit_get_max_delay == 0){
      _hit_get_max_delay = delay;
    }
    if(delay < _hit_get_min_delay || _hit_get_min_delay == 0){
      _hit_get_min_delay = delay;
    }
  } 

  void incLoadGet(float delay){ 
    IceUtil::Mutex::Lock lock(_mutex);
    _load_get_count++;
    _load_get_sum_delay += delay;
    if(delay > _load_get_max_delay || _load_get_max_delay == 0){
      _load_get_max_delay = delay;
    }
    if(delay < _load_get_min_delay || _load_get_min_delay == 0){
      _load_get_min_delay = delay;
    }


  }

	void incRejectFeed(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_reject_feed_count++;
		_reject_feed_sum_delay += delay;
		if(delay > _reject_feed_max_delay || _reject_feed_max_delay == 0){
			_reject_feed_max_delay = delay;
		}
		if(delay < _reject_feed_min_delay || _reject_feed_min_delay == 0){
			_reject_feed_min_delay = delay;
		}

	}

	void incCreateUser(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_creat_user_count++;
		_create_user_sum_delay += delay;
		if(delay > _create_user_max_delay || _create_user_max_delay == 0){
			_create_user_max_delay = delay;
		}
		if(delay < _create_user_min_delay || _create_user_min_delay == 0){
			_create_user_min_delay = delay;
		}
  }

	void incRemove(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_remove_count++;
		_remove_sum_delay += delay;
		if(delay > _remove_max_delay || _remove_max_delay == 0){
			_remove_max_delay = delay;
		}
		if(delay < _remove_min_delay || _remove_min_delay == 0){
			_remove_min_delay = delay;
		}
  }

	void incRemoveUser(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_remove_user_count++;
		_remove_user_sum_delay += delay;
		if(delay > _remove_user_max_delay || _remove_user_max_delay == 0){
			_remove_user_max_delay = delay;
		}
		if(delay < _remove_user_min_delay || _remove_user_min_delay == 0){
			_remove_user_min_delay = delay;
		}
  }

	void incRemoveAll(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_remove_all_count++;
		_remove_all_sum_delay += delay;
		if(delay > _remove_all_max_delay || _remove_all_max_delay == 0){
			_remove_all_max_delay = delay;
		}
		if(delay < _remove_all_min_delay || _remove_all_min_delay == 0){
			_remove_all_min_delay = delay;
		}
  }

	void incRemoveByMerge(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_remove_by_merge_count++;
		_remove_by_merge_sum_delay += delay;
		if(delay > _remove_by_merge_max_delay || _remove_by_merge_max_delay == 0){
			_remove_by_merge_max_delay = delay;
		}
		if(delay < _remove_by_merge_min_delay || _remove_by_merge_min_delay == 0){
			_remove_by_merge_min_delay = delay;
		}
  }

	void incRemoveMergeByFid(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_remove_merge_by_fid_count++;
		_remove_merge_by_fid_sum_delay += delay;
		if(delay > _remove_merge_by_fid_max_delay || _remove_merge_by_fid_max_delay == 0){
			_remove_merge_by_fid_max_delay = delay;
		}
		if(delay < _remove_merge_by_fid_min_delay || _remove_merge_by_fid_min_delay == 0){
			_remove_merge_by_fid_min_delay = delay;
		}
  }

	void incGetPoolUserIds(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_get_ids_count++;
		_get_ids_sum_delay += delay;
		if(delay > _get_ids_max_delay || _get_ids_max_delay == 0){
			_get_ids_max_delay = delay;
		}
		if(delay < _get_ids_min_delay || _get_ids_min_delay == 0){
			_get_ids_min_delay = delay;
		}
  }

	void incPutByUserId(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_put_by_user_id_count++;
		_put_by_user_id_sum_delay += delay;
		if(delay > _put_by_user_id_max_delay || _put_by_user_id_max_delay == 0){
			_put_by_user_id_max_delay = delay;
		}
		if(delay < _put_by_user_id_min_delay || _put_by_user_id_min_delay == 0){
			_put_by_user_id_min_delay = delay;
		}
  }

	void incModify(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_modify_count++;
		_modify_sum_delay += delay;
		if(delay > _modify_max_delay || _modify_max_delay == 0){
			_modify_max_delay = delay;
		}
		if(delay < _modify_min_delay || _modify_min_delay == 0){
			_modify_min_delay = delay;
		}
  }

private:
	IceUtil::Mutex _mutex;
	int _put_count;
	int _hit_get_count;
	int _load_get_count;
  int _reject_feed_count;
  int _creat_user_count;

  int _remove_count;
  int _remove_user_count;
  int _remove_all_count;
  int _remove_by_merge_count;
  int _remove_merge_by_fid_count;

  int _get_ids_count;
  int _put_by_user_id_count;
  int _modify_count;


	float _remove_max_delay;
	float _remove_min_delay;
	float _remove_sum_delay;
 
	float _remove_user_max_delay;
	float _remove_user_min_delay;
	float _remove_user_sum_delay;

	float _remove_all_max_delay;
	float _remove_all_min_delay;
	float _remove_all_sum_delay;

	float _remove_by_merge_max_delay;
	float _remove_by_merge_min_delay;
	float _remove_by_merge_sum_delay;

	float _remove_merge_by_fid_max_delay;
	float _remove_merge_by_fid_min_delay;
	float _remove_merge_by_fid_sum_delay;

	float _get_ids_max_delay;
	float _get_ids_min_delay;
	float _get_ids_sum_delay;

  float _put_by_user_id_max_delay;
  float _put_by_user_id_min_delay;
  float _put_by_user_id_sum_delay;

	float _modify_max_delay;
	float _modify_min_delay;
	float _modify_sum_delay;

	float _put_max_delay;
	float _put_min_delay;
	float _put_sum_delay;
	
	float _hit_get_max_delay;
	float _hit_get_min_delay;
	float _hit_get_sum_delay;

	float _load_get_max_delay;
	float _load_get_min_delay;
	float _load_get_sum_delay;

	float _reject_feed_max_delay;
	float _reject_feed_min_delay;
	float _reject_feed_sum_delay;

	float _create_user_max_delay;
	float _create_user_min_delay;
	float _create_user_sum_delay;

};

#endif
