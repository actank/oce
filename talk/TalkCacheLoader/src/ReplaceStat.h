#ifndef _REPLACESTAT_H_
#define _REPLACESTAT_H_

#include <vector>
#include <string>
#include <IceUtil/RWRecMutex.h>
#include "TaskManager.h"
#include "DictTree.h"
#include "Singleton.h"
namespace com {
namespace xiaonei {
namespace talk {
namespace cache {
namespace loader{

	
typedef enum DoingType
{
	TYPE_NORMAL=0,
	TYPE_LINK=1,
	TYPE_AUDIO=2,
	TYPE_VIDEO=3,
	TYPE_PHONE=4,
	TYPE_MSN=5,
	TYPE_PHOTO = 6
} DoingType;

//typedef vector<string> UbbMap;	
//	typedef shared_ptr< UbbMap > UbbMapPtr;	
//vector<string> _udd2src
	

const string HEAD_URL = "http://xnimg.cn"	;
class ReplaceStat : public MyUtil::Timer,
	public MyUtil::Singleton<ReplaceStat>
{
public:
	ReplaceStat() : Timer(1000*60*60){}
	void replace(long id, int type, string& content, const string& link, unsigned int maxLength);
	void loadFaceTree();
	void handle();
private:
	void _replaceLink(long id, int type, string& content, const string& link, unsigned int maxLength);
	void _replaceFace(string& content);
	vector<string> _ubb2src;
	DictTree _dictTree;
	IceUtil::RWRecMutex _mutex;
};

};	
};
};
};
};	


#endif /* _REPLACESTR_H_ */
