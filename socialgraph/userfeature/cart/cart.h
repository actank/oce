#include <iostream>
#include <vector>
#include <string>

namespace xce {
namespace socialgraph {

const static size_t ITEM_SIZE = 2;
const static int GINI_MAX = 100;
const static int NO_USE_TYPE = 0;
const static int USE_TYPE = 1;
const static int ERR_TYPE = -1;

enum Attribute {
	FRIEND_NUM,
	STAGE,	
	GENDER,
	LOGIN
};

struct Condition {
	Attribute attr_;
	int friend_num_;
	int stage_;
	int login_;
	char gender_[10];
};

struct Item {
	int userid_;
	int friend_num_;
	char gender_[10];
	int stage_;
	int login_;
	int type_; 			//1:recommend user  0:unrecommend user
};
typedef std::vector<Item> ItemList;

struct NodeHead {
	int type_;
	int len_;
};

typedef struct Node {
	ItemList item_list_;
	int item_num_;
	float gini_;
	int type_;
	Condition condition_;
	Node* l_child_;
	Node* r_child_;
} TNode;

//----------------------------------------------------

class Cart {
public:
	Cart(TNode* node) { root_ = node; }
	Cart() { root_ = NULL; }
	Cart(const Cart& obj); 
	~Cart() { Clear(root_); }

	void CreateTree(const ItemList& raw_data);
	void CreateTree(const ItemList& raw_data, TNode* node);
	void PrintTree() const;
	void Travel(const TNode* node) const;
	bool IsPrune() const;
	void Prune();
	int GetLeafNum() const;
	int GetTreeErrNum() const;
	float GetErrRate(const ItemList& raw_data) const;
	int EstimateItemTpye(const Item& item) const;
	int EstimateItemTpye(const TNode* node, const Item& item) const;

	void SaveTreeToDisk(const std::string& target_file) const;
	void LoadTreeFromDisk(const std::string& target_file);

	Cart& operator= (const Cart& obj);

private:
	bool IsPure(const ItemList& raw_data) const;
	int GetNodeType(const ItemList& raw_data) const;	
	float GetBestBranchByFriendNum(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition);
	float GetBestBranchByGender(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition);
	float GetBestBranchByStage(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition);
	float GetBestBranchByLogin(const ItemList& source, ItemList& l_set, ItemList& r_set, Condition& condition);
	float GetGini(const ItemList& left_test, const ItemList& right_test);
	void Copy(TNode* des, TNode* src);
	void Clear(TNode* node); 
	int GetLeafNum(TNode* node) const;
	int EstimateNodeType(const TNode* node) const;
	int GetNodeErrNum(const TNode* node) const;
	int GetTreeErrNum(const TNode* node) const;
	std::vector<TNode*> GetAllNode(TNode* node);
//	std::vector<std::pair<float, TNode*> > GetCCPList(const TNode* node) const;
	bool IsCorrect(const TNode* node, const Item& item) const;
	void SaveTreeToDisk(const TNode* node, FILE* fp) const;
	void LoadTreeFromDisk(TNode* node, FILE* fp);
	int Tobuff(const TNode* node, char* buff) const;
	void Frombuff(TNode* node, char* buff);

	void PrintNode(const TNode* node) const;

	TNode* root_;
};

}
}
