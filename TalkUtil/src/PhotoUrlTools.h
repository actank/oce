/*
code by guoqing.liu

根据照片的相对地址，取照片的绝对地址

*/


#ifndef _PHOTO_URL_TOOLS_
#define _PHOTO_URL_TOOLS_

#include "TaskManager.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <Ice/Ice.h>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>

namespace com{
namespace xiaonei{
namespace talk{
namespace util{
using namespace MyUtil;
using namespace std;

const string DB_INSTANCE = "upload";
const string TABLE_NAME = "storage_node";
const string KAIXIN_PREFIX = "kx";


const int NODE_STATUS_READ = 1;
const int NODE_STATUS_READANDWRIGHT = 3;


const string UrlPhotoAlbum = "http://photos.album.5q.com";
const string UrlPhoto2Album = "http://photos2.album.5q.com";
const string UrlImg = "http://img.xiaonei.com";
const string UrlHead = "http://head.xiaonei.com";
const string UrlPic001 = "http://pic.xiaonei.com";
const string UrlPic002 = "http://pic2.xiaonei.com";


//DB 配置Node
class Node : public IceUtil::Shared{
public:
	string node_name;
	string domain;
	string group_name;
	int type;
	int status;
};
typedef IceUtil::Handle<Node> NodePtr;

//DB配置Group
class Group : public IceUtil::Shared{
public:
	void AddNode(const NodePtr& node);
	NodePtr GetReadAbleNode(int mod);
	string name;
private:
	vector<NodePtr> _nodes;//每个group包含多个node
};

typedef IceUtil::Handle<Group> GroupPtr;


class PhotoUrlHelper : virtual public Singleton<PhotoUrlHelper>{
public:
	void StartReloadTimer(int time = 60 * 1000);//启动重新加载DB数据的Timer
	string GetFullUrl(string& pattern);//获取全路径
	void Reload();//重新加载
	GroupPtr GetGroup(const string& group_name);//根据group_name 获取group
private:
	class ReloadTimer : virtual public Timer{//重新加载的Timer
	public:
		ReloadTimer(int time) : Timer(time){};
		void handle(){
			PhotoUrlHelper::instance().Reload();
		}
	};
	
	typedef map<string, GroupPtr> GroupMap;//group_name 2 groupPtr
	typedef map<string, GroupPtr>::iterator GroupMapIt;

	GroupMap _groupmap;
	IceUtil::RWRecMutex _rwmutex;
};




};
};
};
};

#endif
