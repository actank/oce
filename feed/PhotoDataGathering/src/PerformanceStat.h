#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

//class TimeStat{
//        timeval _begin, _end;
//public:
//        TimeStat(){
//                reset();
//        }
//        void reset(){
//                gettimeofday(&_begin, NULL);
//        }
//        float getTime(){
//
//                gettimeofday(&_end, NULL);
//                float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
//                                -_begin.tv_usec;
//                timeuse/=1000;
//                return timeuse;
//        }
//};


class TimeStat {
  time_t _begin, _end;
public:
  TimeStat() {
    reset();
  }
  void reset() {
    time(&_begin);
  }
  float getTime() {
    time(&_end);

    time_t timeuse = _end - _begin;
    return timeuse;
  }
};




#endif
