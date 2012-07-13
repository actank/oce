#include <ctemplate/template.h>
#include <string>

#include "Singleton.h"
#include "NotifyIndexAdapter.h"
//#include "Notify/util/rmessage_util.h"

namespace xce {
namespace notify {

const int kWebViewId		= 0;
const int kPagerViewId	= 1;
const int kAppViewId		= 3;
const int kAppViewId2		= 4;
const int kClientViewId	= 5;
const int kPhoneViewId	= 6;
const int kIphoneViewId	= 7;

/*
const int BIGTYPE_LEFTMOVE = 16;
const int BIGTYPE_ANDVALUE = 16711680;
const int TYPE_ANDVALUE= 65535;


static int GetRMessageType(const int type){
	  return type & TYPE_ANDVALUE;
}
static int GetRMessageBigtype(const int type){
	  return ((type & BIGTYPE_ANDVALUE) >> BIGTYPE_LEFTMOVE);
}
*/

class SchemaManager: public MyUtil::Singleton<SchemaManager> {
public:
	SchemaManager();

	bool reload();
	std::string toReplyTxt(const NotifyBodyPtr& notify, int view, bool is_kaixin,vector<Ice::Long> &fail_nids) const;
	//std::string toReplyTxtTest(const NotifyBodyPtr& notify, int view, bool is_kaixin,vector<Ice::Long> &fail_nids) const;
	std::string toReplyTxtMerge(const vector<NotifyBodyPtr> & nbs, int view ,bool is_kaixin,vector<Ice::Long> &fail_nids) const;

	bool getIndexViewCheck(int view, const NotifyIndex& index);

	bool checkDispatch(const MyUtil::Str2StrMap& props) const;
	int isReply(int schema_id) const;

	int GetBigtypeBytype(int type) const;

	vector<int> GetTypesByBigtype(int bigtype) const;

	bool MustMerge(int type,int view) const;
	void Dump();
	std::string toReplyTxt(const NotifyBodyPtr& notify, int view, bool is_kaixin,map<Ice::Long, int> &fail_nids) const;
	std::string toReplyTxtMerge(const vector<NotifyBodyPtr> & nbs, int view ,bool is_kaixin,map<Ice::Long, int> &fail_nids) const;
private:
	struct Schema {
		int id;
		int bigtype;
		string name;
		bool display;
		int is_reply;
		set<string> keys;
		map<int, ctemplate::Template*> templates;
		int type;
	};

	typedef map<int, Schema> SchemaMap;
	SchemaMap _schema_map;
	map<int, int> _type2bigtype;
	map<int, vector<int> > _bigtype2types;
	time_t _load_time;
	mutable IceUtil::RWRecMutex _mutex;

	set<int> merge_types_;
};

}
}
