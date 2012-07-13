#include <UserCacheAdapter.h>
#include <iostream>

using namespace std;
using namespace xce::usercache;

int main(int argc, char* argv[]) {
	MyUtil::IntSeq ids;
	ids.push_back(68126);
	std::map<int, UserCachePtr> res = UserCacheAdapter::instance().GetUserCache(ids);
	cout << "id: " << res.find(68126)->second->id() << endl;
	cout << "name: " << res.find(68126)->second->name() << endl;
	cout << "status: " << res.find(68126)->second->status() << endl;
	cout << "tinyurl: " << res.find(68126)->second->tinyurl() << endl;
	cout << "state: " << res.find(68126)->second->state() << endl;
	cout << "gender: " << res.find(68126)->second->gender() << endl;
	cout << "online: " << res.find(68126)->second->online() << endl;
	cout << "level: " << res.find(68126)->second->level() << endl;
	cout << "univ: " << res.find(68126)->second->univ() << endl;
	cout << res.find(68126)->second->IsSelected() << endl;
	return 0;
}

