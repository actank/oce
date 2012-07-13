#ifndef RMESSAGEUTIL_ 
#define RMESSAGEUTIL_ 


#include "IceLogger.h"
#include <string>
#include "Singleton.h"
#include<sys/stat.h>
#include   <unistd.h>

namespace xce{
namespace notify{


using namespace std;
using namespace MyUtil;
/*
const int BIGTYPE_LEFTMOVE = 16;
const int BIGTYPE_ANDVALUE = 16711680;
const int TYPE_ANDVALUE = 65535;

const int GetRMessageType(const int type){
	return type & TYPE_ANDVALUE;
}

const int GetRMessageBigtype(const int type){
	int result = ((type & BIGTYPE_ANDVALUE) >> BIGTYPE_LEFTMOVE);
	//MCE_INFO("GetRMessageBigtype-->type:" << type << " result:" << result);
	return result;
}
*/

class RMessageUtil : public Ice::Object, public Singleton<RMessageUtil>{
public:	
	int Util_GetRMessageType(const int type){
		return type & 65535;
	}

	int Util_GetRMessageBigtype(const int type){
		int result = ((type & 16711680) >> 16);
		//MCE_INFO("GetRMessageBigtype-->type:" << type << " result:" << result);
		return result;
	}


};

};
};
#endif 
