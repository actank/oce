#ifndef _COMMON_H_
#define _COMMON_H_

#include    <mysql/mysql.h>
#include    <cassert>
#include    <climits>                           // for 'DBL_MAX'
#include    <cmath>
#include    <ctype.h>                           // for 'isspace'
#include    <ctime>                             // for 'time'
#include    <cstdlib>                           // for 'system'
#include    <cstdio>
#include    <errno.h>

#include    <errno.h>                           // for errno,perror,strerror

#include    <algorithm>                         // include STL algorithms
#include    <fstream>
#include    <iomanip>
#include    <iostream>
#include    <sstream>                           // for (o)stringstream
#include    <list>
#include    <queue>                             // for 'priority_queue'
#include    <set>
#include    <string>
#include    <vector>
#include    <map>
#include    <ext/hash_map>
#include    <ext/hash_set>

#include    <sys/types.h>
#include    <sys/stat.h>
#include    <unistd.h>
#include    <dirent.h>
#include    <ServiceI.h>   // for MCE_INFO

using namespace std;
using namespace __gnu_cxx;

typedef int USERID;

struct Comp_ByCommonFriend
{
	bool operator()(const pair<USERID,float>& p1, const pair<USERID,float>& p2)
	{
		return p1.second > p2.second;
	}
};

struct Comp_ByUserId
{
	bool operator()(const USERID p1, const USERID p2)
	{
		return p1 > p2;
	}
};

time_t GetYestodayZero(time_t now);
int split_c(char* str, char** items, int item_size, const char* delim);
char* TrimLastEnter(char* str);
int TravesalDir(const char* dir, set<string>& fnames);
void PrintTime();
long get_filesize(char *filename);
char* time2str(const time_t ltime, char* str);

#endif

