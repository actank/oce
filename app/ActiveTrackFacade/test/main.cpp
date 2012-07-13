#include <ActiveTrackAdapter.h>
#include <ActiveTrackFacadeAdapter.h>

using namespace std;
using namespace xce::log;
using namespace xce::activetrack::adapter;

int main(int argc, char* argv[]) {
	int userId = 25596255;
	vector<string> paths;
	paths.push_back("RE_68126");
	paths.push_back("AT_PHOTO_12345");
	string action = "REPLY";
	string object = "PHOTO_12345_68128";
	cout << "action: " << userId << " ";
	for( vector<string>::iterator it = paths.begin(); it != paths.end(); ++it ){
		cout << *it << " ";
	}
	cout << action << " ";
	cout << object << " ";
	cout << endl;

	ActiveTrack::instance().initialize();
	ActiveTrack::instance().action(userId, paths, action, object);
	ActiveTrack::instance().action(userId, "RE_68126", action, object);

	ActiveTrackFacadeAdapter::instance().action(userId,paths,action,object);

	cout << "action done" << endl;
}

