//Function: calculate run time cost of the module, modified from MyUtil::RunTime
//Author:   ?, modified by XiongJunWu
//Email:    junwu.xiong@renren-inc.com
//Data:     2011-11-16
#ifndef __TIME_COST_H_
#define __TIME_COST_H_

#include <string>
#include <sys/time.h>
#include "socialgraph/socialgraphutil/clogging.h"

using namespace std;

namespace xce {
namespace socialgraph {

class RunTime {
 public: 
  static RunTime create(const std::string& name){
    RunTime new_instance(name);
    new_instance.start();
    return new_instance;
  }                       
                          
  void step(string step_name){
    long now = get_now();
    //std::cout << "RunTime : " << name_ << " " << step_name <<" "<< now - last_point_ <<" ms" << std::endl;
    CLOG(INFO) << "RunTime : " << name_ << " " << step_name <<" "<< now - last_point_ <<" ms";
    last_point_ = now;
  }       
          
  ~RunTime(){
          //std::cout << "RunTime : " << name_ << " TOTAL "<< get_now() - start_ <<" ms" << std::endl;
          CLOG(INFO) << "RunTime : " << name_ << " TOTAL "<< get_now() - start_ <<" ms";
  }       
 private:        
  std::string name_;
  long start_;
  long last_point_;

  RunTime(const string& name) : name_(name) {
  }

  void start(){
          start_ = get_now();
          last_point_ = start_;
  }

  long get_now(){
    struct timeval tv;
    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000+tv.tv_usec/1000;
  };
};

};
};

#endif 
