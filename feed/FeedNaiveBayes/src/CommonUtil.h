#ifndef FEEDNAIVEBAYESI_COMMONUTIL_H_
#define FEEDNAIVEBAYESI_COMMONUTIL_H_

#include <string>
#include <cstdio>
namespace xce {
namespace feed {

using namespace xce::feed;
using namespace std;

//"/data/xce/StatUtil/data/score/score_history.2011-05-21"

//static void Extract4(uint64_t r, int &a, int &b, int& c, int &d);
uint64_t Join2(int a, int b) ;
uint64_t Join4(int a, int b, int c, int d) ;

void Extract2(uint64_t r, int & a, int &b) ;

void Extract4(uint64_t r, int &a, int &b, int& c, int &d);


string AddZero(int i) ;



}
}
#endif
