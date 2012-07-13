#ifndef __TT_LOADER_H_
#define __TT_LOADER_H_

#include "common_header.h"

const unsigned int kMaxUserIdNumEachProcess = 200;
const string UPDATE_PREFIX = "sc_date_";
const string DELETE_PREFIX = "delete_";
const unsigned int IO_BUFFER_SIZE = 8192;

const int SEARCH_CACHE = 0;

const std::string SEARCH_PERSISTENT_CACHE_MANAGER = "M";


class FillTask{
 public:
   FillTask();
   void handle(int cycles, int begin_id, int valid_id_num);
};

#endif
