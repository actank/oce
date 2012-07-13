#include <fstream>
#include "util/cpp/String.h"

using namespace std;
using namespace MyUtil;

int main(int argc, char** argv) {

	map<string, vector<string> > merges;
	ifstream ifsList("list.txt");
	ofstream ofsMerge("merge.txt");
	string line;
	while (getline(ifsList, line) ) {
		vector<string> params = StrUtil::split(line, " ");
		if ( params.size() >= 4 ) {
			string host = params[3];
			if ( merges.find(host) != merges.end() ) {
				merges[host].push_back(line);
			} else {
				vector<string> lines;
				lines.push_back(line);
				merges.insert(pair<string, vector<string> >(host, lines));
			}
		}
	}

	for (map<string, vector<string> >::iterator it = merges.begin(); it != merges.end(); ++it ) {
		ofsMerge << (*it).first;
		for ( size_t i = 0; i < (*it).second.size(); ++i ) {
			ofsMerge << "\t" << (*it).second[i];
		}
		ofsMerge << "\n";
	}
}
