#include "UGCVisitRecord.h"

using namespace std;

void UGCVisitRecord::LoadData(const std::string& fileName) {
	cout << "[UGCVisitRecord] LoadData fileName(" << fileName << ")" << endl;
	
	ifstream infile(fileName.c_str(), ifstream::in);
	string line = "";
	int lineNumber = 0;

	while (getline(infile, line)) {
		if (++lineNumber % 5000 == 0) {
			fprintf(stderr, "UGCVisitRecord LoadData reading line %d\r", lineNumber);		
		}

		if (line.empty()) {
			continue;
		}
		
		int guestId = 0;
		string action = "";
		string type = "";
		long sourceId = 0;
		int ownerId = 0;
		int flag = 0;

		istringstream stream(line);
		stream >> guestId >> action >> type >> sourceId >> ownerId >> flag;
	//	cout << guestId << " " << action << " " << type << " " << sourceId << " " << ownerId << " " << flag << endl;

		record_[guestId].insert(sourceId);

		//if (lineNumber == 5) {
		//	break;
		//}
	}
	cout << "record_ size = " << record_.size() << endl;
	//for (map<int, set<long> >::iterator iter = record_.begin();
	//	iter != record_.end(); ++iter) {
	//	cout << iter->first << endl;
	//	ostringstream oss;
	//	for (set<long>::iterator it = iter->second.begin();
	//		it != iter->second.end(); ++it) {
	//		oss << *it << " ";
	//	}	
	//	cout << oss.str() << endl;
	//	cout << endl;
	//}
}

bool UGCVisitRecord::IsVisit(int userId, long ugcId) {
	IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	
	map< int, set<long> >::iterator fIt = record_.find(userId);
	if (fIt == record_.end()) {
		return false;
	}

	set<long>::iterator IIt = fIt->second.find(ugcId);
	if (IIt != fIt->second.end()) {
		return true;
	}
	return false;
}

std::set<long> UGCVisitRecord::GetVisitRecord(int userid) {
	map< int, set<long> >::iterator find_iter = record_.find(userid);
	if (find_iter != record_.end()) {
		return find_iter->second;
	}

	return set<long>();
}

void UGCVisitRecord::PrintMap() {
	int count = 0;
	for (map<int, set<long> >::iterator iter = record_.begin();
			iter != record_.end(); ++iter) {
		cout << iter->first << " : " << endl;
		for (set<long>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			cout << *it << "  ";
		}

		cout << endl;
		cout << endl;

		if (++count >= 10) {
			break;
		}
	}
}
