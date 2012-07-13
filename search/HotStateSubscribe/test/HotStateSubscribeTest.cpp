#ifndef __HOTSTATESUBSCRIBETEST_H__
#define __HOTSTATESUBSCRIBETEST_H__

#include "OceCxxAdapter/src/HotStateSubscribeAdapter.h"

void usage()  {
  std::cout<<"Usage: [HotStateSubscribeTest] subscribe     type    pattern    max_size    passwd"<<std::endl;
  std::cout<<"                               unSubscribe   type"<<std::endl;
  std::cout<<"                               getNewState   type    limit"<<std::endl;
  return;
}



int main(int argc, char ** argv)  {
  if(argc < 2)  {
    usage();
  } else if(strcmp(argv[1], "subscribe") == 0  && argc == 6)  {
    string type = (string)argv[2];
    string pattern = (string)argv[3];
    int max_size = atoi(argv[4]);
    string passwd = (string)argv[5];
    HotStateSubscribeAdapter::instance().subscribe(type, pattern, max_size, passwd); 
  } else if(strcmp(argv[1], "unSubscribe") == 0 && argc == 3)  {
    
    string type = (string)argv[2];
    HotStateSubscribeAdapter::instance().unSubscribe(type); 
  } else if(strcmp(argv[1], "getNewState") == 0 && argc == 4)  {
    int limit = atoi(argv[3]); 
    string type = (string)argv[2];
    HotStateSubscribeAdapter::instance().getNewState(type, limit); 
  }  else  {
    usage();
  }
  return 0;
}


#endif
