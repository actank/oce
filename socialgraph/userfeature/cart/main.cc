#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "socialgraph/userfeature/cart/cart.h"
#include "socialgraph/socialgraphutil/clogging.h"

using namespace xce::socialgraph;
using namespace std;

ItemList ReadRawData(const std::string& file);
void CreateModel(char* create_source, char* test_source, char* save_tree);
void ProcessUserData(char* load_tree, char* data, char* result);

int main(int argc, char* argv[]) {
	char* create_source = NULL;
	char* test_source = NULL;
	char* save_tree = NULL;
	char* load_tree = NULL;
	char* data = NULL;
	char* result = NULL;
	int o;

	while (-1 != (o = getopt(argc, argv, "c:t:s:l:d:r:"))) {
		switch (o) {
			case 'c': create_source = optarg; break;
			case 't':	test_source = optarg; break;
			case 's':	save_tree = optarg; break;
			case 'l':	load_tree = optarg; break;
			case 'd':	data = optarg; break;
			case 'r':	result = optarg; break;
		}		
	}
	if (NULL != create_source) {
		CreateModel(create_source, test_source, save_tree);	
		return 0;
	}
	if (NULL != load_tree) {
		ProcessUserData(load_tree, data, result);
		return 0;
	}
	return 0;
}

ItemList ReadRawData(const std::string& file) {
	std::string line = "";
	std::ifstream infile(file.c_str(), std::ifstream::in);
	ItemList item_list;

	while (getline(infile, line)) {
		if (line.empty()) {
			continue;
		}
		Item item;
		int userid = 0;
		int friend_num = 0;
		std::string gender = "";
		int stage = 0;
		int login = 0;
		int type = 0;

		std::istringstream stream(line);
		stream >> userid >> friend_num >> gender >> stage >> login >> type;
		CLOG(INFO) << userid << ":" << friend_num << ":" << gender << ":" << stage << ":" << login << ":" << type;
		
		if (userid > 0 &&
				friend_num > 0 &&
				friend_num < 10000 &&
				stage < 1000 &&
				login < 100) {
			item.userid_ = userid;
			item.friend_num_ = friend_num;
			strcpy(item.gender_, gender.c_str());
			item.stage_ = stage;
			item.login_ = login;
			item.type_ = type;
			if (strcmp(item.gender_, "男生") == 0 ||
					strcmp(item.gender_, "女生") == 0) {
				item_list.push_back(item);
			}
		}
	}	
	return item_list;
}

void CreateModel(char* create_source, char* test_source, char* save_tree) {
	Cart tree;
	if (NULL != create_source) {
		/* get raw from external file */
		ItemList item_list = ReadRawData(std::string(create_source));
		CLOG(INFO) << "item_list size:" << item_list.size();
		tree.CreateTree(item_list);
		CLOG(INFO) << "finish create tree leaf num:" << tree.GetLeafNum();
		/* prune the biggest tree into sub_tree list */
		std::vector<Cart> subtree_list;
		subtree_list.push_back(tree);
		while (subtree_list.back().IsPrune()) {
			Cart subtree = subtree_list.back();
			subtree.Prune();
			subtree_list.push_back(subtree);
			if (subtree_list.size() % 1000 == 0) {
				CLOG(INFO) << "subtree_list size:" << subtree_list.size();
			}
		}

		if (NULL != test_source) {
			/* select the best sub tree as result according to test set */
			ItemList test_list = ReadRawData(std::string(test_source));
			Cart final_tree;
			float min_err = INT_MAX;
			for (std::vector<Cart>::iterator iter = subtree_list.begin();
					iter != subtree_list.end(); ++iter) {
				float tmp_err = iter->GetErrRate(test_list);
				if (tmp_err < min_err) {
					min_err = tmp_err;
					final_tree = *iter;
				}
			}
			CLOG(INFO) << "*******************************************";
			CLOG(INFO) << "min_err:" << min_err; 
			final_tree.PrintTree();
			if (NULL != save_tree) {
				final_tree.SaveTreeToDisk(std::string(save_tree));
			}
		}
	}
}

void ProcessUserData(char* load_tree, char* data, char* result) {
	Cart tree;
	if (NULL != load_tree) {
		tree.LoadTreeFromDisk(std::string(load_tree));
		CLOG(INFO) << "********************************************";
		tree.PrintTree();

		if (NULL != data && NULL != result) {
			std::ofstream outfile(result, ofstream::trunc);		//open file
			ItemList data_list = ReadRawData(std::string(data));	
			for (ItemList::iterator iter = data_list.begin();
					iter != data_list.end(); ++iter) {
				int type = tree.EstimateItemTpye(*iter);
				outfile << iter->userid_ << "\t" << iter->friend_num_ << "\t" << iter->gender_ 
								<< "\t" << iter->stage_ << "\t" << iter->login_ << "\t" << type << "\n";
			}
		}
	}
}
