/*
 * AppendReceive.h
 *
 *  Created on: Jun 5, 2011
 *      Author: yejingwei
 */

#ifndef COMMONUTIL_H
#define COMMONUTIL_H
#include <map>
#include <ext/hash_map>
#include <iostream>
#include <fstream>
#include "Singleton.h"
#include <cstdio>
using namespace std;

#define DoLog(msg) std::cout << msg << std::endl;;
namespace xce {
namespace feed {

uint64_t Join2(int a, int b) {
	uint64_t r = 0;
	r = r | (uint64_t(a) << 32);
	r = r | (uint32_t) b;
	return r;
}



void Extract2(uint64_t r, int & a, int &b) {
	a = (int) ((r & 0xFFFFFFFF00000000) >> 32);
	b = (int) (r & 0x00000000FFFFFFFF);
}


class TimeStat{
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log=false){
    _log = log;
    reset();
  }
  ~TimeStat(){
    if(_log){

    }
  }
  void reset(){
    gettimeofday(&_begin, NULL);
  }
  float getTime(){
    gettimeofday(&_end, NULL);
    float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
    -_begin.tv_usec;
    timeuse/=1000;
    return timeuse;
  }


};

}
}
#endif /* APPENDRECEIVE_H_ */
