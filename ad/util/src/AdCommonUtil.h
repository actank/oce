/*
 * AdCommonUtil.h
 *
 *  Created on: 2010-12-24
 *      Author: zhaohui.tang
 */

#ifndef AD_COMMON_UTIL_H_
#define AD_COMMON_UTIL_H_

#include "Util.h"
#include "Evictor.h"
#include <map>

namespace xce {
namespace ad {


/***********************************************************************************************************************/
class TimeStat {
  timeval _begin, _end;
public:
  TimeStat() {
    reset();
  }
  void reset() {
    gettimeofday(&_begin, NULL);
  }
  float getTime() {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec
        - _begin.tv_usec;
    timeuse /= 1000;
    return timeuse;
  }
};

template<class _Key, class _Object>
class Evictor : public MyUtil::SizedEvictor<_Key, _Object> {
public:
  typedef typename MyUtil::SizedEvictor<_Key, _Object>::EvictorMap::iterator
      iterator;
  Evictor() {
  }
  Evictor(const Evictor<_Key, _Object> & evictor) {
    MyUtil::SizedEvictor<_Key, _Object>::_map = evictor._map;
    MyUtil::SizedEvictor<_Key, _Object>::_queue = evictor._queue;
    MyUtil::SizedEvictor<_Key, _Object>::_size = evictor._size;
  }
  Evictor & operator=(const Evictor<_Key, _Object> & evictor) {
    MyUtil::SizedEvictor<_Key, _Object>::_map = evictor._map;
    MyUtil::SizedEvictor<_Key, _Object>::_queue = evictor._queue;
    MyUtil::SizedEvictor<_Key, _Object>::_size = evictor._size;
    return *this; 
  }
  bool operator<(const Evictor<_Key, _Object> & evictor) {
    return MyUtil::SizedEvictor<_Key, _Object>::_map < evictor._map;
  }
  iterator begin() {
    return MyUtil::SizedEvictor<_Key, _Object>::_map.begin();
  }

  iterator end() {
    return MyUtil::SizedEvictor<_Key, _Object>::_map.end();
  }
  pair<_Key, _Object> getObject(const iterator& pos) {
    IceUtil::Mutex::Lock lock(MyUtil::SizedEvictor<_Key, _Object>::_mutex);
    if (pos != MyUtil::SizedEvictor<_Key, _Object>::_map.end()) {

      return pair<_Key, _Object> (pos->first, pos->second.obj);
    }
    return pair<_Key, _Object> (0, NULL);

  }
};

} // end namespace ad
} // end namespace xce
#endif /* AD_COMMON_UTIL_H_ */
