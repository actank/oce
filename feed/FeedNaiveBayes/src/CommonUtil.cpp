
#include "CommonUtil.h"
#include <string>
#include <cstdio>
#include <boost/lexical_cast.hpp>



using namespace xce::feed;

using namespace std;
using namespace boost;

//"/data/xce/StatUtil/data/score/score_history.2011-05-21"


// void Extract4(uint64_t r, int &a, int &b, int& c, int &d);
uint64_t xce::feed::Join2(int a, int b) {
	uint64_t r = 0;
	r = r | (uint64_t(a) << 32);
	r = r | (uint32_t) b;

	return r;
}
 uint64_t xce::feed::Join4(int a, int b, int c, int d) {
	uint64_t r = 0;
	r = (uint64_t(a) << 48) | (uint64_t(b) << 32) | (uint64_t(c) << 16)
			| (uint64_t(d));

	Extract4(r, a, b, c, d);
	return r;
}

void xce::feed::Extract2(uint64_t r, int & a, int &b) {
	a = (int) ((r & 0xFFFFFFFF00000000) >> 32);
	b = (int) (r & 0x00000000FFFFFFFF);
}

void xce::feed::Extract4(uint64_t r, int &a, int &b, int& c, int &d) {
	a = (int) ((r & 0xFFFF000000000000) >> 48);
	b = (int) ((r & 0x0000FFFF00000000) >> 32);
	c = (int) ((r & 0x00000000FFFF0000) >> 16);
	d = (int) ((r & 0x000000000000FFFF));
}


string xce::feed::AddZero(int i) {
  string s;
  if (i < 10) {
    s = "0";
  }
  s += lexical_cast<string> (i);
  return s;
}



