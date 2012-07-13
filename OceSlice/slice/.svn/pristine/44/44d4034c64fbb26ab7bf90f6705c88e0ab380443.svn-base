#ifndef _SOCIALGRAPH_MESSAGE_H_
#define _SOCIALGRAPH_MESSAGE_H_

#include <Util.ice>

module xce {
module socialgraph {


enum ActionType {
	GET, DELETE, UNDO, CREATE, APPLY, ACCEPT, IGNORE, DENY, UPDATE, ADD
};

struct Info {
	int hostId;		//通知的主ID
	string type;		//zookerper的类型,如: FFW
	string key;		//memcache中使用的key,如:FFW_{hostId}
	ActionType operation;	//通知的类型

	MyUtil::Str2StrMap detail;
	MyUtil::IntSeq friendIds;
	MyUtil::ByteSeq data;
};

dictionary<string, Info> Str2InfoMap;

struct Message {
	Str2InfoMap content;
};

};
};

#endif
