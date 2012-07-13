#include "ugc_model.h"

using namespace std;

void RawUgcManager::LoadDataFromFile(const std::string& file_name) {
	ifstream infile(file_name.c_str(), ifstream::in);

	string line = "";
	int line_number = 0;

	while (getline(infile, line)) {
		if (++line_number % 5000 == 0) {
			fprintf(stderr, "Ugc LoadData reading line %d\r", line_number);
		}

		if (line.empty()) {
			continue;
		}

		int uid = 0;
		long ugcid = 0;
		int comment = 0;
		int view = 0;
		int weight = 0;
		int type = 0;
		string interest = "";

		istringstream stream(line);
		stream >> uid >> ugcid >> comment >> view >> weight >> type >> interest;
		//cout << uid << " : " << ugcid << " : " << comment << " : " << view << " : " << weight 
		//		<< " : " << type << " : " << interest << endl;

		RawUgcModel model(uid, ugcid, comment, view, weight, type, interest);

		user_ugc_map_[uid].push_back(model);
	}
}

std::vector<RawUgcModel> RawUgcManager::GetUgcList(int ownerid) {
	std::map<int, std::vector<RawUgcModel> >::iterator fit = user_ugc_map_.find(ownerid);
	if (fit != user_ugc_map_.end()) {
		return fit->second;
	}
	return std::vector<RawUgcModel>();
}

void RawUgcManager::PrintInfo() {
	for (map<int, vector<RawUgcModel> >::iterator iter = user_ugc_map_.begin();
			iter != user_ugc_map_.end(); ++iter) {
		cout << iter->first << " : " << endl;
		for (vector<RawUgcModel>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
				cout << it->ownerid_ << ":" << it->ugcid_ << ":" << it->ugcid_ << ":" 
						<< it->comment_count_ << ":" << it->view_count_ << ":" << it->weight_
						<< it->type_ << ":" << it->interest_ << endl;
		}
		cout << endl;
		cout << endl;
	}
}
