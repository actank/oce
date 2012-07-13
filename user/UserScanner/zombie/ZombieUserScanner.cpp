#include "ZombieUserScanner.h"

using namespace std;
using namespace MyUtil;
using namespace xce::userfilter;
using namespace com::xiaonei::xce;
using namespace xce::adapter::userstate;


//ZombieUserScanner
//--------------------------------------------------------------
//先获取新的活跃用户和
ZombieUserScanner::ZombieUserScanner(unsigned min, unsigned max):_minId(min),_maxId(max){
	_historyBitMap = new UserBitMap();
	_newBitMap = new UserBitMap();
	ConnectionPoolManager::instance().initialize();
	loadNewData();
	loadHistoryData();
	mapToFile();
//	setActive();
}
ZombieUserScanner::~ZombieUserScanner(){
	if(_historyBitMap)delete _historyBitMap;
	if(_newBitMap)delete _newBitMap;
}

//注意：_newBitMap中保存的是30天内活跃的用户，1表示活跃，0表示不活跃
void ZombieUserScanner::setActive(){
	int diff = 0;
	for(unsigned i=0; i < ALL_ID_COUNT ; i++){
		if((*_newBitMap)[i] ^ (*_historyBitMap)[i]){
			//cout << "setActive : id = " << i << ", active = " << ((*_newBitMap)[i]) << endl;
			try {
				if((*_newBitMap)[i]){
					UserStateAdapter::instance().setZombieOff(i);
				}else{
					UserStateAdapter::instance().setZombieOn(i);
				}
				diff++;
			} catch (...) {
				std::cout << "Exception id= " << i << " status= " << (*_newBitMap)[i] << std::endl;
				continue;
			}
		}
	}
	cout << "[ZombieUserScanner] setActive : total id count = " << diff << endl;
}

//需要在调用loadNewData()之后调用
void ZombieUserScanner::loadHistoryData(){
	//首先从文件加载
	bool hasLog = fileToMap();
	//加载失败，则重新加载所有用户信息
	if(!hasLog){
		AllUserFilterPtr allFilter = new AllUserFilter();
		allFilter->getAllUserIDs(_historyBitMap, _minId, _maxId);
		for(unsigned i=0; i < ALL_ID_COUNT ; i++){
			//如果有这个用户且30天内登录过，则把历史登录设置为0，以方便在setActive()中被检测到
			//对于没登录和不存在的用户，可以不需要设置
			if((*_historyBitMap)[i] && (*_newBitMap)[i]){
				_historyBitMap->reset(i);
			}
		}
	}
	cout << "[ZombieUserScanner] loadHistoryData : active id count = " << _historyBitMap->count() << endl;
}

void ZombieUserScanner::loadNewData(){

	//从数据库读取用户登录信息到infoMap
	time_t now = time(0);
	LoginInfoMap infoMap;
	UserLoginInfoLoaderPtr loader = new UserLoginInfoLoader();
	loader->loadFromDB(infoMap, now, _minId, _maxId);

	//判断是否Zombie，存储到newBitMap（实际上存储的是指定时间内的活跃用户）
	UserLoginFilterPtr filter = new UserLoginFilter();
	CheckerPtr zombieChecker = new ActiveChecker(30, 1);
	filter->getLoginUserIDs(_newBitMap, infoMap, zombieChecker);

	cout << "[ZombieUserScanner] loadNewData : active id count = " << _newBitMap->count() << endl;
}

bool ZombieUserScanner::fileToMap(){
	ifstream ifile(getFilePath().c_str());
	if(!ifile) {
		return false;
	}
	string line;
	while (getline(ifile, line)) {
		int id = atoi(line.c_str());
		_historyBitMap->set(id);
	}
	cout << "[ZombieUserScanner] fileToMap, file " << getFilePath() << " active id count = " << _historyBitMap->count() << endl;
	ifile.close();
	return true;
}

bool ZombieUserScanner::mapToFile(){

	cout << "[ZombieUserScanner] mapToFile, file " << getFilePath() << " active id count = " << _newBitMap->count() << endl;

	ofstream ofile(getFilePath().c_str());
	if(!ofile) {
		return false;
	}
	for(unsigned i=0; i < ALL_ID_COUNT ; i++){
		if((*_newBitMap)[i]){
			ofile << i << endl;
		}
	}
	ofile.close();
	return true;
}

string ZombieUserScanner::getFilePath() {
	return "../log/user_zombie.log";
}
