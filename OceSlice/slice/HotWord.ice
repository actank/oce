#ifndef __HOTWORD_ICE__
#define __HOTWORD_ICE__

#include <Util.ice>
module xce
{
module hotwords 
{

struct DoingItem{
	long id;
	int uid;
	int time;
	string content;
	string meta; 
};
["java:type:java.util.ArrayList"] sequence<DoingItem> DoingItemSeq;

struct HotWord {
		string key;
		string alias;
		string detail;
		MyUtil::StrSeq likes;
		int docFreq;
		int uidFreq;
		int history;
		float score;
		int rank;
		int preRank;
};

["java:type:java.util.ArrayList"] sequence<HotWord> HotWordSeq;

interface HotWordManager{
		void addDoings(DoingItemSeq doings);
//		void setAliasWord(string key,string value);
//		void setBlackWord(string word);
//		void setWhiteWord(string word);
		HotWordSeq lastHotWords(string unit);
		DoingItemSeq searchDoings(string unit,string key);
};

};
};
#endif
