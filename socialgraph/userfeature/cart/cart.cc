#include "socialgraph/userfeature/cart/cart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <set>

#include "socialgraph/socialgraphutil/clogging.h"

namespace xce {
namespace socialgraph {

Cart::Cart(const Cart& obj) {
	if (!(root_ = new TNode())) {
		CLOG(ERROR) << "new TNode error!";
		exit(1);
	}

	Copy(root_, obj.root_);
}

Cart& Cart::operator= (const Cart& obj) {
	Clear(root_);		//clear root
	if (!(root_ = new TNode())) {
		CLOG(ERROR) << "new TNode error!";
		exit(1);
	}

	Copy(root_, obj.root_);
	return *this;
}

void Cart::Copy(TNode* des, TNode* src) {
	if (!des || !src) {
		return;
	}

	des->item_list_ = src->item_list_;
	des->item_num_ = src->item_num_;
	des->gini_ = src->gini_;
	des->type_ = src->type_;

	des->condition_.attr_ = src->condition_.attr_;
	des->condition_.friend_num_ = src->condition_.friend_num_;
	des->condition_.stage_ = src->condition_.stage_;
	des->condition_.login_ = src->condition_.login_;
	strcpy(des->condition_.gender_, src->condition_.gender_);

	if (NULL == src->l_child_ &&
			NULL == src->r_child_) {
		des->l_child_ = NULL;
		des->r_child_ = NULL;
	} else {
		des->l_child_ = new TNode();
		Copy(des->l_child_, src->l_child_);
		
		des->r_child_ = new TNode();
		Copy(des->r_child_, src->r_child_);
	}
}

bool Cart::IsPrune() const {
	if (NULL == root_) {
		return false;
	}

	if (NULL == root_->l_child_ &&
			NULL == root_->r_child_) {
		return false;
	}

	return true;
}

void Cart::CreateTree(const ItemList& raw_data) {
	if (!(root_ = new TNode())) {
		CLOG(ERROR) << "malloc TNode error!";
		exit(1);
	}

	CreateTree(raw_data, root_);
}

void Cart::CreateTree(const ItemList& raw_data, TNode* node) {
	node->item_list_ = raw_data;
	node->item_num_ = (int)raw_data.size();

	if (IsPure(raw_data) ||										//纯节点或者节点中数据集数量小于定值
			raw_data.size() < ITEM_SIZE) {
		node->type_ = GetNodeType(raw_data); 			//叶子节点的类型
		node->l_child_ = NULL;
		node->r_child_ = NULL;
		return;
	}
	node->type_ = 2;		//非叶子节点

	ItemList left_branch;
	ItemList right_branch;
	float gini = 10;

	float tmp_gini = 10;
	ItemList tmp_left;
	ItemList tmp_right;
	Condition tmp_condition;
	tmp_gini = GetBestBranchByFriendNum(raw_data, tmp_left, tmp_right, tmp_condition);
	if (tmp_gini < gini) {
		gini = tmp_gini;
		left_branch.swap(tmp_left);
		right_branch.swap(tmp_right);
		node->condition_.attr_ = tmp_condition.attr_;		//设置节点属性
		node->condition_.friend_num_ = tmp_condition.friend_num_;
	}
	//std::cout << "tmp_gini:" << tmp_gini << " FriendsNumComp gini:" << gini << " left_branch size:" << left_branch.size() << " right_branch size:" << right_branch.size() << std::endl;

	tmp_gini = GetBestBranchByGender(raw_data, tmp_left, tmp_right, tmp_condition);
	if (tmp_gini < gini) {
		gini = tmp_gini;
		left_branch.swap(tmp_left);
		right_branch.swap(tmp_right);
		node->condition_.attr_ = tmp_condition.attr_;		//设置节点属性
		strcpy(node->condition_.gender_, tmp_condition.gender_);
	}
	//std::cout << "tmp_gini:" << tmp_gini << " GenderComp gini:" << gini << " left_branch size:" << left_branch.size() << " right_branch size:" << right_branch.size() << std::endl;

	tmp_gini = GetBestBranchByStage(raw_data, tmp_left, tmp_right, tmp_condition);
	if (tmp_gini < gini) {
		gini = tmp_gini;
		left_branch.swap(tmp_left);
		right_branch.swap(tmp_right);
		node->condition_.attr_ = tmp_condition.attr_;		//设置节点属性	
		node->condition_.stage_ = tmp_condition.stage_;
	}
	//std::cout << "tmp_gini:" << tmp_gini << " StageComp gini:" << gini << " left_branch size:" << left_branch.size() << " right_branch size:" << right_branch.size() << std::endl;

	tmp_gini = GetBestBranchByLogin(raw_data, tmp_left, tmp_right, tmp_condition);
	if (tmp_gini < gini) {
		gini = tmp_gini;
		left_branch.swap(tmp_left);
		right_branch.swap(tmp_right);
		node->condition_.attr_ = tmp_condition.attr_;		//设置节点属性
		node->condition_.login_ = tmp_condition.login_;
	}
	//std::cout << "tmp_gini:" << tmp_gini << " LoginComp gini:" << gini << " left_branch size:" << left_branch.size() << " right_branch size:" << right_branch.size() << std::endl;

	if (!(node->l_child_ = new TNode())) {
		CLOG(ERROR) << "malloc TNode error!";
		exit(1);
	}
	if (!(node->r_child_ = new TNode())) {
		CLOG(ERROR) << "malloc TNode error!";
		exit(1);
	}
	CreateTree(left_branch, node->l_child_);
	CreateTree(right_branch, node->r_child_);
}

void Cart::PrintTree() const {
	Travel(root_);
}

void Cart::Travel(const TNode* node) const {
	if (2 != node->type_) {
		CLOG(DEBUG) << "(type:" << node->type_ << ", num:" <<  node->item_num_ << ")";
		return;
	}
	
	if (node->condition_.attr_ == FRIEND_NUM) {
		CLOG(DEBUG) << "(type:" << node->type_ << ", num:" << node->item_num_ 
						<< ", condition:friend_num, value:" << node->condition_.friend_num_ << ")";
	} else if (node->condition_.attr_ == GENDER) {
		CLOG(DEBUG) << "(type:" << node->type_  << ", num:" << node->item_num_ 
						<< ", condition:gender, value:" << node->condition_.gender_ << ")";
	} else if (node->condition_.attr_ == STAGE) {
		CLOG(DEBUG) << "(type:" << node->type_  << ", num:" << node->item_num_ 
						<< ", condition:stage, value:" << node->condition_.stage_<< ")";
	} else if (node->condition_.attr_ == LOGIN) {
		CLOG(DEBUG) << "(type:" << node->type_  << ", num:" << node->item_num_ 
						<< ", condition:login, value:" << node->condition_.login_ << ")";
	}
	Travel(node->l_child_);
	Travel(node->r_child_);
}

bool Cart::IsPure(const ItemList& raw_data) const {
	if (raw_data.empty()) {
		return true;
	}
	int sample_type = raw_data.front().type_;
	for (ItemList::const_iterator iter = raw_data.begin();
			iter != raw_data.end(); ++iter) {
		if (sample_type != iter->type_) {
			return false;
		}
	}
	return true;
}

int Cart::GetNodeType(const ItemList& raw_data) const {
	int type0_count = 0;
	int type1_count = 0;

	for (ItemList::const_iterator iter = raw_data.begin();
			iter != raw_data.end(); ++iter) {
		if (0 == iter->type_) {
			++type0_count;
		} else if (1 == iter->type_) {
			++type1_count;
		}
	}

	if (type0_count > type1_count) {
		return NO_USE_TYPE;
	} 

	return USE_TYPE;
}

float Cart::GetBestBranchByFriendNum(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition) {
	condition.attr_ = FRIEND_NUM;

	std::set<int> threshold;
	for (ItemList::const_iterator iter = source.begin();
			iter != source.end(); ++iter) {
		threshold.insert(iter->friend_num_);
	}

	float global_gini = GINI_MAX;
	for (std::set<int>::iterator iter = threshold.begin();
			iter != threshold.end(); ++iter) {
		ItemList left_test;
		ItemList right_test;
		for (ItemList::const_iterator it = source.begin();
				it != source.end(); ++it) {
			if (it->friend_num_ >= *iter) {			// >=friend_num进入左子树，<friend_num进入右子树
				left_test.push_back(*it);
			} else {
				right_test.push_back(*it);
			}
		}

		if (left_test.empty() ||
				right_test.empty()) {
			continue;
		}

		float local_gini = GetGini(left_test, right_test);
		
		if (local_gini < global_gini) {
			global_gini = local_gini;
			l_set.swap(left_test);
			r_set.swap(right_test);
			condition.friend_num_ = *iter;
		}
	}
	return global_gini;
}

float Cart::GetBestBranchByGender(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition) {
	condition.attr_ = GENDER;

	std::set<std::string> threshold;
	for (ItemList::const_iterator iter = source.begin();
			iter != source.end(); ++iter) {
		threshold.insert(std::string(iter->gender_));
	}

	float global_gini = GINI_MAX;
	for (std::set<std::string>::iterator iter = threshold.begin();
			iter != threshold.end(); ++iter) {
		ItemList left_test;
		ItemList right_test;
		for (ItemList::const_iterator it = source.begin(); it != source.end(); ++it) {
			if (std::string(it->gender_) >= *iter) {			// >=friend_num进入左子树，<friend_num进入右子树
				left_test.push_back(*it);
			} else {
				right_test.push_back(*it);
			}
		}

		if (left_test.empty() ||
				right_test.empty()) {
			continue;
		}

		float local_gini = GetGini(left_test, right_test);
		
		if (local_gini < global_gini) {
			global_gini = local_gini;
			l_set.swap(left_test);
			r_set.swap(right_test);
			strcpy(condition.gender_, iter->c_str());
		}
	}
	return global_gini;
}

float Cart::GetBestBranchByStage(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition) {
	condition.attr_ = STAGE;

	std::set<int> threshold;
	for (ItemList::const_iterator iter = source.begin();
			iter != source.end(); ++iter) {
		threshold.insert(iter->stage_);
	}

	float global_gini = GINI_MAX;
	for (std::set<int>::iterator iter = threshold.begin();
			iter != threshold.end(); ++iter) {
		ItemList left_test;
		ItemList right_test;
		for (ItemList::const_iterator it = source.begin();
				it != source.end(); ++it) {
			if (it->stage_ >= *iter) {			// >=friend_num进入左子树，<friend_num进入右子树
				left_test.push_back(*it);
			} else {
				right_test.push_back(*it);
			}
		}

		if (left_test.empty() ||
				right_test.empty()) {
			continue;
		}

		float local_gini = GetGini(left_test, right_test);
		
		if (local_gini < global_gini) {
			global_gini = local_gini;
			l_set.swap(left_test);
			r_set.swap(right_test);
			condition.stage_ = *iter;
		}
	}
	return global_gini;
}

float Cart::GetBestBranchByLogin(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition) {
	condition.attr_ = LOGIN;

	std::set<int> threshold;
	for (ItemList::const_iterator iter = source.begin();
			iter != source.end(); ++iter) {
		threshold.insert(iter->login_);
	}

	float global_gini = GINI_MAX;
	for (std::set<int>::iterator iter = threshold.begin();
			iter != threshold.end(); ++iter) {
		ItemList left_test;
		ItemList right_test;
		for (ItemList::const_iterator it = source.begin();
				it != source.end(); ++it) {
			if (it->login_ >= *iter) {			// >=friend_num进入左子树，<friend_num进入右子树
				left_test.push_back(*it);
			} else {
				right_test.push_back(*it);
			}
		}

		if (left_test.empty() ||
				right_test.empty()) {
			continue;
		}

		float local_gini = GetGini(left_test, right_test);
		
		if (local_gini < global_gini) {
			global_gini = local_gini;
			l_set.swap(left_test);
			r_set.swap(right_test);
			condition.login_ = *iter;
		}
	}
	return global_gini;
}

float Cart::GetGini(const ItemList& left_test, const ItemList& right_test) {
	int l_test_type0 = 0;
	int l_test_type1 = 0;
	for (ItemList::const_iterator it = left_test.begin();
			it != left_test.end(); ++it) {
		if (0 == it->type_) {
			++l_test_type0;
		} else if (1 == it->type_) {
			++l_test_type1;
		}
	}

	int r_test_type0 = 0;
	int r_test_type1 = 0;
	for (ItemList::const_iterator it = right_test.begin();
			it != right_test.end(); ++it) {
		if (0 == it->type_) {
			++r_test_type0;
		} else if (1 == it->type_) {
			++r_test_type1;
		}
	}

	float l_0_gini = (float)(l_test_type0 / left_test.size());
	float l_1_gini = (float)(l_test_type1 / left_test.size());
	float l_gini = 1 - l_0_gini * l_0_gini - l_1_gini * l_1_gini;

	float r_0_gini = (float)(r_test_type0 / right_test.size());
	float r_1_gini = (float)(r_test_type1 / right_test.size());
	float r_gini = 1 - r_0_gini * r_0_gini - r_1_gini * r_1_gini;

	if (l_gini < 0 ||
			r_gini < 0) {
		return GINI_MAX;
	}

	float l_var = (float)left_test.size() / (left_test.size() + right_test.size());
	float r_var = (float)right_test.size() / (left_test.size() + right_test.size());
	
	float local_gini = l_var * l_gini + r_var * r_gini;
	return local_gini;
}

void Cart::Prune() {
	std::vector<TNode*> all_node_list = GetAllNode(root_);
	//std::cout << "all_node_list size:" << all_node_list.size() << std::endl;

	std::vector<std::pair<float, TNode*> > ccp_list;
	for (std::vector<TNode*>::iterator iter = all_node_list.begin();
			iter != all_node_list.end(); ++iter) {
		if (NULL == (*iter)->l_child_ &&		//找到叶子节点
				NULL == (*iter)->r_child_) {
			continue;
		}
		int node_err = GetNodeErrNum(*iter);
		int tree_err = GetTreeErrNum(*iter);
		int leaf_num = GetLeafNum(*iter);
		float a = (float)(node_err - tree_err) / (leaf_num - 1);
		//std::cout << "node_err:" << node_err << "	" << "tree_err:" << tree_err << "	"  
		//					<< "leaf_num:" << leaf_num << "	" << "a:" << a << std::endl;
		ccp_list.push_back(std::make_pair<float, TNode*>(a, *iter));
	}
	//std::cout << "ccp_list size:" << ccp_list.size() << std::endl;

	std::pair<float, TNode*> result(INT_MAX, NULL);
	for (std::vector<std::pair<float, TNode*> >::iterator iter = ccp_list.begin();
			iter != ccp_list.end(); ++iter) {
		if (iter->first < result.first) {
			result = *iter;
		} else if (iter->first == result.first) {
			if (NULL == result.second) {
				CLOG(DEBUG) << "result.second is NULL exception!";		//异常
				exit(1);
			}
			int result_leaf = GetLeafNum(result.second);
			int tmp_leaf = GetLeafNum(iter->second);
			if (result_leaf < tmp_leaf) {
				result = *iter;
			}
		}
	}
	//std::cout << "result_value:" << result.first << "	result_leaf:" << GetLeafNum(result.second) << std::endl;

	if (NULL != result.second) {
		Clear(result.second->l_child_);
		Clear(result.second->r_child_);		//为什么无法赋值成NULL呢?

		result.second->l_child_ = NULL;
		result.second->r_child_ = NULL;

		result.second->type_ = EstimateNodeType(result.second);
		result.second->condition_ = Condition();
		result.second->gini_ = 0;
	}
}

int Cart::GetLeafNum() const {
	return GetLeafNum(root_);
}

int Cart::GetLeafNum(TNode* node) const {
	if (NULL == node) {
		return 0;
	}
	if (NULL == node->l_child_ &&
			NULL == node->r_child_) {
		return 1;
	}

	int num = 0;
	num += GetLeafNum(node->l_child_);
	num += GetLeafNum(node->r_child_);
	return num;
}

int Cart::EstimateNodeType(const TNode* node) const {
	if (NULL == node) {
		return ERR_TYPE;
	}
	return GetNodeType(node->item_list_);
}

int Cart::GetNodeErrNum(const TNode* node) const {
	int node_type = EstimateNodeType(node);
	if (ERR_TYPE == node_type) {
		return 0;		//异常情况返回0
	}
	int err_num = 0;
	for (ItemList::const_iterator iter = node->item_list_.begin();
			iter != node->item_list_.end(); ++iter) {
		if (node_type != iter->type_) {
			++err_num;
		}	
	}

	return err_num;
}

int Cart::GetTreeErrNum() const {
	if (NULL == root_) {
		return 0;
	}

	return GetTreeErrNum(root_);
}

int Cart::GetTreeErrNum(const TNode* node) const {
	if (NULL == node) {
		return 0;		//异常情况返回0
	}

	if (NULL == node->l_child_ &&		//判断为叶子结点
			NULL == node->r_child_) {
		return GetNodeErrNum(node);	
	}
	int err_num = 0;
	err_num += GetTreeErrNum(node->l_child_);
	err_num += GetTreeErrNum(node->r_child_);
	return err_num;
}

std::vector<TNode*> Cart::GetAllNode(TNode* node) {
	std::vector<TNode*> node_list;
	if (NULL == node) {
		return node_list;
	}
	node_list.push_back(node);
	
	std::vector<TNode*> l_node_list = GetAllNode(node->l_child_);
	std::vector<TNode*> r_node_list = GetAllNode(node->r_child_);
	
	if (!l_node_list.empty()) {
		node_list.insert(node_list.end(), l_node_list.begin(), l_node_list.end());
	}
	if (!r_node_list.empty()) {
		node_list.insert(node_list.end(), r_node_list.begin(), r_node_list.end());
	}

	return node_list;
}

float Cart::GetErrRate(const ItemList& raw_data) const {
	int total_num = (int)raw_data.size();
	int err_num = 0;
	for (ItemList::const_iterator iter = raw_data.begin();
			iter != raw_data.end(); ++iter) {
		//std::cout << "id:" << iter->userid_ << "	friend_num:" << iter->friend_num_ << "	gender:" << iter->gender_ 
		//					<< "	stage:" << iter->stage_ << "	login:" << iter->login_ << "	type:" << iter->type_ << std::endl;
		if (!IsCorrect(root_, *iter)) {
			++err_num;
		}
	}
	
	return (float) err_num / total_num;
}

bool Cart::IsCorrect(const TNode* node, const Item& item) const {
	bool result = false;
	if (NULL == node) {
		return result;
	}
	
	if (NULL == node->l_child_ &&		//叶子节点
			NULL == node->r_child_) {
		return node->type_ == item.type_;
	}

	switch (node->condition_.attr_) {
		case FRIEND_NUM:
		{
			if (item.friend_num_ >= node->condition_.friend_num_) {
				result = IsCorrect(node->l_child_, item);
			} else {
				result = IsCorrect(node->r_child_, item);
			}
			break;
		}
		case STAGE:
		{
			if (item.stage_ >= node->condition_.stage_) {
				result = IsCorrect(node->l_child_, item);
			} else {
				result = IsCorrect(node->r_child_, item);
			}
			break;
		}
		case GENDER:
		{
			if (item.gender_ >= node->condition_.gender_) {
				result = IsCorrect(node->l_child_, item);
			} else {
				result = IsCorrect(node->r_child_, item);
			}
			break;
		}
		case LOGIN:
		{
			if (item.login_ >= node->condition_.login_) {
				result = IsCorrect(node->l_child_, item);
			} else {
				result = IsCorrect(node->r_child_, item);
			}
			break;
		}
	}
	return result;
}

int Cart::EstimateItemTpye(const Item& item) const {
	return EstimateItemTpye(root_, item);
}

int Cart::EstimateItemTpye(const TNode* node, const Item& item) const {
	int result = NO_USE_TYPE;
	if (NULL == node) {
		return result;
	}
	
	if (NULL == node->l_child_ &&		//叶子节点
			NULL == node->r_child_) {
		return node->type_;
	}

	switch (node->condition_.attr_) {
		case FRIEND_NUM:
		{
			if (item.friend_num_ >= node->condition_.friend_num_) {
				result = EstimateItemTpye(node->l_child_, item);
			} else {
				result = EstimateItemTpye(node->r_child_, item);
			}
			break;
		}
		case STAGE:
		{
			if (item.stage_ >= node->condition_.stage_) {
				result = EstimateItemTpye(node->l_child_, item);
			} else {
				result = EstimateItemTpye(node->r_child_, item);
			}
			break;
		}
		case GENDER:
		{
			if (item.gender_ >= node->condition_.gender_) {
				result = EstimateItemTpye(node->l_child_, item);
			} else {
				result = EstimateItemTpye(node->r_child_, item);
			}
			break;
		}
		case LOGIN:
		{
			if (item.login_ >= node->condition_.login_) {
				result = EstimateItemTpye(node->l_child_, item);
			} else {
				result = EstimateItemTpye(node->r_child_, item);
			}
			break;
		}
	}
	return result;
}

void Cart::SaveTreeToDisk(const std::string& target_file) const {
	//printf("SaveTreeToDist to target_file:%s\n", target_file.c_str());
	FILE* fp = fopen(target_file.c_str(), "w+");
	if (NULL == fp)	{
		CLOG(ERROR) << "fopen file error!";
		return;
	}
	SaveTreeToDisk(root_, fp);
	fclose(fp);
}

void Cart::SaveTreeToDisk(const TNode* node, FILE* fp) const {
	if (NULL == node) {
		return;
	}
	//printf("SaveTreeToDisk two parameter\n");
	//PrintNode(node);
	char* buff = new char[5<<20];
	int len = Tobuff(node, buff);	
	//printf("SaveTreeToDisk len:%d\n", len);

	NodeHead head;
	head.type_ = node->type_;
	head.len_ = len;

	fwrite(&head, sizeof(NodeHead), 1, fp);
	fwrite(buff, len, 1, fp);

	free(buff);
	
	SaveTreeToDisk(node->l_child_, fp);
	SaveTreeToDisk(node->r_child_, fp);
}

void Cart::LoadTreeFromDisk(const std::string& target_file) {
	CLOG(DEBUG) << "Load from dist target_file:" << target_file;
	Clear(root_);		//清空树
	
	FILE* fp = fopen(target_file.c_str(), "r+");	
	if (NULL == fp)	{
		CLOG(ERROR) << "fopen file error!";
		return;
	}
	root_ = new TNode();
	LoadTreeFromDisk(root_, fp);

	fclose(fp);
}

void Cart::LoadTreeFromDisk(TNode* node, FILE* fp) {
	CLOG(DEBUG) << "LoadTreeFromDisk two parameter!";
	char* buff = new char[5<<20];

	NodeHead head;
	fread(&head, sizeof(NodeHead), 1, fp);
	CLOG(DEBUG) << "LoadTreeFromDisk head.type:" << head.type_ << ", head.len:" << head.len_;
	fread(buff, head.len_, 1, fp);
	Frombuff(node, buff);
	free(buff);
	
	//PrintNode(node);
	if (head.type_ != 2) {
		return;
	}
	node->l_child_ = new TNode();
	LoadTreeFromDisk(node->l_child_, fp);
	node->r_child_ = new TNode();
	LoadTreeFromDisk(node->r_child_, fp);
}

int Cart::Tobuff(const TNode* node, char* buff) const {
	if (NULL == node) {
		return 0;
	}
	char* p = buff;
	*(int*)p = (int)node->item_list_.size();	p += sizeof(int);
	for (ItemList::const_iterator iter = node->item_list_.begin();
			iter != node->item_list_.end(); ++iter) {
		*(int*)p = iter->userid_; p += sizeof(int);		//userid_
		*(int*)p = iter->friend_num_; p += sizeof(int);		//friend_num_
		//int str_len = strlen(iter->gender_);
		//printf("str_len:%d\n", str_len);
		//printf("len:%d\n", std::string(iter->gender_).size());
		strncpy(p, iter->gender_, 10);	p += 10;		//gender_
		//strcpy(p, iter->gender_);	p += 5;		//gender_
		*(int*)p = iter->stage_; p += sizeof(int);		//stage_
		*(int*)p = iter->login_; p += sizeof(int);		//login_
		*(int*)p = iter->type_; p += sizeof(int);		//type_
	}
	*(int*)p = node->item_num_;	p += sizeof(int);
	*(float*)p = node->gini_; p += sizeof(float);
	*(int*)p = node->type_; p += sizeof(int);
	//*(Condition*)p = node->condition_; 	p += sizeof(Condition);
	*(Attribute*)p = node->condition_.attr_; p += sizeof(Attribute);
	*(int*)p = node->condition_.friend_num_; p += sizeof(int);
	*(int*)p = node->condition_.stage_; p += sizeof(int);
	*(int*)p = node->condition_.login_; p += sizeof(int);
	//int str_len = strlen(node->condition_.gender_);
	strncpy(p, node->condition_.gender_, 10); p += 10;
	//strcpy(p, node->condition_.gender_); p += 5;

	return p - buff;
}

void Cart::Frombuff(TNode* node, char* buff) {
	if (NULL == node) {
		return;
	}
	char* p = buff;
	int item_size = *(int*)p;	p += sizeof(int);
	CLOG(DEBUG) << "Frombuff item_size:" << item_size;
	for (int i = 0; i < item_size; ++i) {
		Item one;
		one.userid_ = *(int*)p;	p += sizeof(int);
		one.friend_num_ = *(int*)p;	p += sizeof(int);
		strncpy(one.gender_, p, 10);	p += 10;
		//strcpy(one.gender_, p);	p += 5;
		one.stage_ = *(int*)p; p += sizeof(int);
		one.login_ = *(int*)p; p += sizeof(int);
		one.type_ = *(int*)p; p += sizeof(int);

		node->item_list_.push_back(one);
	}

	CLOG(DEBUG) << "node->item_list_ size:" << node->item_list_.size();
	//for (ItemList::iterator iter = node->item_list_.begin();
	//		iter != node->item_list_.end(); ++iter) {
	//	printf("userid:%d, friend_num:%d, gender:%s, stage:%d, login:%d, type:%d\n", iter->userid_, iter->friend_num_,
	//			iter->gender_, iter->stage_, iter->login_, iter->type_);
	//}
	node->item_num_ = *(int*)p; p += sizeof(int);
	node->gini_ = *(float*)p; p += sizeof(float);
	node->type_ = *(int*)p; p += sizeof(int);
	node->condition_.attr_ = *(Attribute*)p; p += sizeof(Attribute);
	node->condition_.friend_num_ = *(int*)p; p += sizeof(int);
	node->condition_.stage_ = *(int*)p; p += sizeof(int);
	node->condition_.login_ = *(int*)p; p += sizeof(int);
	strncpy(node->condition_.gender_, p, 10); p += 10;

	node->l_child_ = NULL;
	node->r_child_ = NULL;
}

void Cart::PrintNode(const TNode* node) const {
	for (ItemList::const_iterator iter = node->item_list_.begin();
			iter != node->item_list_.end(); ++iter) {
		CLOG(DEBUG) << "userid:" << iter->userid_ << ", friend_num:" << iter->friend_num_
								<< ", gender:" << iter->gender_ << ", stage:" << iter->stage_ 
								<< ", login:" << iter->login_  << ", type:" << iter->type_;
	}
	CLOG(DEBUG) << "item_num:" << node->item_num_ << ", gini:" << node->gini_ << ", type:" << node->type_;
	CLOG(DEBUG) << "attr:" << node->condition_.attr_ << ", friend_num:" << node->condition_.friend_num_
							<< ", stage:" << node->condition_.stage_ << ", gender:" << node->condition_.gender_;
}

void Cart::Clear(TNode* node) {
	if (node != NULL) {
		Clear(node->l_child_);
		Clear(node->r_child_);
		ItemList().swap(node->item_list_);		//clear vector
		free(node);
		node = NULL;
	}
}

}
}
