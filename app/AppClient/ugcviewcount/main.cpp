#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "util/cpp/String.h"
#include <fstream>

using std::map;
using std::string;
using std::vector;

string printMap(const map<string, int>& stats) {
	ostringstream os;
	for (map<string, int>::const_iterator it = stats.begin(); it != stats.end(); ++it) {
		os << (*it).first << "\t" << (*it).second << "\n";
	}
	return os.str();
}

void stat(const string& filepath, map<int, vector<int> >& friendMap) {

	map<string, int> stats;

	xce::buddy::adapter::BuddyByIdCacheAdapter adapter;
	ifstream ifsFile;
	ifsFile.open(filepath.c_str());
	string line;
	while (getline(ifsFile, line) ) {
		vector<string> segs = MyUtil::StrUtil::split(line, "\t");
		if ( segs.size() < 3 ) {
			continue;
		}
		string type = segs[0];
		int owner = atoi(segs[1].c_str());
		int visitor = atoi(segs[2].c_str());
		string relation = "";

		if ( visitor < 0 ) {
			relation = "all";
		} else if ( visitor == 0 ) {
			relation = "zero";
		} else if ( visitor == owner ) {
			relation = "self";
		} else {
			map<int, vector<int> >::iterator itFriend = friendMap.find(visitor);
			if ( itFriend == friendMap.end() ) {
				vector<int> newlist = adapter.getFriendListAsc(visitor, 2000);
				friendMap.insert( pair<int, vector<int> >(visitor, newlist) );
				itFriend = friendMap.find(visitor);
			}
			vector<int>& friends = (*itFriend).second;
			vector<int>::iterator itPos = lower_bound(friends.begin(), friends.end(), owner);
			if ( (*itPos) == owner ) {
				relation = "friend";
			} else {
				relation = "notfriend";
			}
		}
		string key = type + "-" + relation;
		if ( stats.find(key) != stats.end() ) {
			++stats[key];
		} else {
			stats.insert(pair<string, int>(key, 1));
		}
	}
	ofstream ofsStat(string(filepath+".stat").c_str());
	ofsStat << printMap(stats);
}

int main(int argc, char* argv[]) {

	map<int, vector<int> > friendMap;

	for ( int i = 1; i < argc; ++i ) {
		cout << "File : " << argv[i] << "\n";
		stat(argv[i], friendMap);
	}

}
