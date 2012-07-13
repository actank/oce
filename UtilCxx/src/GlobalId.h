#include <Util.h>

const static int64_t TYPE_MASK = 1000000LL;
const static int64_t OBJECT_MASK = 8000000000000LL;

namespace MyUtil{
const int PROFILE = 0;
const int BLOG = 1;
const int PHOTO = 2;
const int ALBUM = 3;
const int SHARE = 4;
const int STATUS = 5;
const int EDM = 6;
const int LOVE = 7;
const int CONNECT = 8;
const int ADS = 9;
const int GAME = 10;
const int MINISITE = 11;
const int PAGE = 12;
const int VIDEO = 13;
const int APP = 14;

class GlobalIdTransfer{
public:
	static GlobalIdPtr get(long gid){
		GlobalIdPtr p = new GlobalId;
		p->type = (int)(gid/OBJECT_MASK);
		p->id = gid%OBJECT_MASK;
		return p;
		
	};
	static long get(GlobalIdPtr p){
		long gid = (p->type%TYPE_MASK) * OBJECT_MASK + p->id%OBJECT_MASK;
		return gid;
	};
};

}
