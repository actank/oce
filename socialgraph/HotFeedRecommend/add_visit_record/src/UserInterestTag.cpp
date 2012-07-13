#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "UserInterestTag.h"

using namespace std;

void UserInterestTag::LoadData(const std::string& fileName) {
	cout << "[UserInterestTag] LoadData fileName(" << fileName << ")" << endl;

	ifstream infile(fileName.c_str(), ifstream::in);
	string line = "";
	int lineNumber = 0;

	while (getline(infile, line)) {
//		if (lineNumber >= 2) {
//			break;
//		}

		if (++lineNumber % 5000 == 0) {
			fprintf(stderr, "UserInterestTag LoadData reading line %d\r", lineNumber);		
		}

		if (line.empty()) {
			continue;
		}

		int ownerId;
		int weight;
		string tags;
		
		istringstream stream(line);
		stream >> ownerId >> weight >> tags;
		
		//cout << ownerId << " " << weight << " " << tags << endl;
    
		vector<string> split_vector;
		boost::split(split_vector, tags, boost::is_any_of(";"));

		//cout << "split_vector size : " << split_vector.size() << endl;
		//for (vector<string>::iterator iter = split_vector.begin();
		//		iter != split_vector.end(); ++iter) {
		//	cout << *iter << "  ";
		//}
		//cout << endl;
		//cout << endl;

		for (vector<string>::iterator iter = split_vector.begin();
				iter != split_vector.end() && iter != split_vector.begin() + 10; ++iter) {     //取用户的10个标签
			vector<string> tmp;
			boost::split(tmp, *iter, boost::is_any_of(","));
			
			if (2 == (int)tmp.size()) {
				userTagMap_[ownerId].insert(tmp.at(0));
			}
		}
	}

	//GetInterestMap();
//	PrintMap();
}

bool UserInterestTag::IsInterested(int userId, const std::string& interest) {
	TagMapType::iterator fIt = userTagMap_.find(userId);
	if (fIt == userTagMap_.end()) {
		return false;
	}

	set<string>::iterator fIIt = fIt->second.find(interest);
	if (fIIt == fIt->second.end()) {
		return false;
	}

	return true;
}

void UserInterestTag::PrintMap() {
	int count = 0;
	for (TagMapType::iterator iter = userTagMap_.begin();
			iter != userTagMap_.end(); ++iter) {
		cout << iter->first << " : " << endl;
		for (set<string>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			cout << *it << "  ";
		}
		cout << endl;
		cout << endl;

		if (count > 10) {
			break;	
		}

		++count;
	}
}

void UserInterestTag::GetInterestMap() {
	cout << "[UserInterestTag] map size : " << userTagMap_.size() << endl;
}

std::set<std::string> UserInterestTag::GetUserInterest(int userid) {
	map< int, set<string> >::iterator find_iter = userTagMap_.find(userid);
	if (find_iter != userTagMap_.end()) {
		return find_iter->second;
	}

	return set<string>();
}
