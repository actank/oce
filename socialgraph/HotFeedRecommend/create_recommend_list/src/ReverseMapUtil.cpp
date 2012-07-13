#include "ReverseMapUtil.h"

using namespace reversemap;
using namespace std;

void ReverseMapUtil::ReverseMap(const std::map<int, std::vector<int> >& resource, std::map<int, std::vector<int> >& destination) {
	time_t phrase_start = 0, phrase_end = 0;
	phrase_start = time(NULL);	

	for (map<int, vector<int> >::const_iterator iter = resource.begin();
			iter != resource.end(); ++iter) {
		for (vector<int>::const_iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			destination[*it].push_back(iter->first);
		}
	}

	phrase_end = time(NULL);

	fprintf(stderr, "[ReverseMapUtil] ReverseMap size of resource=%ld, size of destination=%ld, using %.1fm\n", 
		resource.size(), destination.size(), (phrase_end - phrase_start)/60.0);
}
