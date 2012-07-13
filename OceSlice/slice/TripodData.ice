#ifndef __TRIPOD_DATA_ICE__
#define __TRIPOD_DATA_ICE__

#include <TripodException.ice>

module com {
module renren {
module tripod {

//["cpp:type:std::list<std::string>"]
sequence<string> StrList;

//["cpp:type:std::list<int>"]
sequence<int> IntList;

sequence<string> KeySeq;

sequence<byte> Data;

dictionary<string, Data> DataMap;

sequence<string> BigKeySeq;

dictionary<string, string> BigKeyDataMap;

};
};
};
#endif
