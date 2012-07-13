
#ifndef GENERAL_SUGGESTION 
#define GENERAL_SUGGESTION

#include<Util.ice>
module xce {
module generalsuggestion{

class IndexItem{
   string type;
   MyUtil::Str2StrMapSeq docs; 
};
sequence<string> StrSeq;
sequence<IndexItem> IndexItemSeq;

class SuggestionResult {
	StrSeq missIndexs;
	IndexItemSeq results;
};
sequence<SuggestionResult> SuggestionResultSeq;

interface SearchManager{
 		SuggestionResult search(int userid,string query,int begin,int limit,StrSeq types);
		void load(int userid,IndexItemSeq indexItems,MyUtil::Str2IntMap config);
};

};
};
#endif
