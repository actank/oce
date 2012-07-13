#ifndef DIALOGUE_ICE
#define DIALOGUE_ICE

#include <Util.ice>

module mop {
module svc {
module hi {
module mesg {

	class Message {
		int fromId;
		int toId;
		int messageId;
		string messageContent;
	};

	sequence<Message> MessageSeq;

	class DialogueInfo {
		MessageSeq mesgs;
		int messageCount;
	};

//	class MessageSet {
//		idempotent int getSize();
//		idempotent MessageSeq getAll(int begin, int limit);
//		bool add(Message mesgOne);
//		void merge(MessageSet mesgSet);
//		MessageSeq mesgs;
//	};

	class MessageManager{
		DialogueInfo postMessage(Message mesg);
		MyUtil::IntSeq getLimitUsers(int userId, int begin, int limit);
		int getLimitUserCount(int userId, int begin, int limit);
		MyUtil::IntSeq getHighLight(int userId, int begin, int limit);
		int getHighLightCount(int userId, int begin, int limit);
		DialogueInfo getMessages(int toId, int begin, int limit);
	};

};
};
};
};



#endif

