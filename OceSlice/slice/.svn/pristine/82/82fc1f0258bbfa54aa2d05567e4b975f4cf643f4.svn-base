#ifndef __FEEDQUEUE_ICE__
#define __FEEDQUEUE_ICE__


module xce
{
module feed
{

class SqlInfo{
	string instance;
	int wrflag;
	string pattern;
	string statement;
	int flag;//包含一些特殊信息，个位表示这条信息是否restore
};

class FeedQueue{
	int Push(SqlInfo sql);	
	//恢复的时间格式 "2011-03-3 11:00:00"	
	void Restore(string begin,string end,int thishour);	//thishour表明是否恢复当前这个小时的oce_log
	void TestPush(SqlInfo sql);
};

};
};
#endif