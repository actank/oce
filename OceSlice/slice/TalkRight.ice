#ifndef __TALK_RIGHT_ICE__
#define __TALK_RIGHT_ICE__

#include <Util.ice>
#include <TalkCommon.ice>

module com
{
module xiaonei
{
module talk
{

class Verify{
	int from;
	int to;
	int type;
	string sysMsgType;
	int index;
};

sequence<Verify> VerifySeq;

/*class VerifySysMsg{
	int to;
	string msgType;
	int index;
}
sequence<VerifySysMsg> VerifySysMsgSeq;

class VerifyResult{
	int index;
	bool isPass;
};
sequence<VerifyResult> VerifyResultSeq;
*/

class UserStatus 
{
	int selected;
	int status;
};
class RightManager
{
	//bool verify(int from, int to , int type);
	MyUtil::IntSeq batchVerify(VerifySeq seq); //return index
	
	
//	VerifyResultSeq batchVerifySysMsg(VerifySysMsgSeq seq);
	
	UserStatus getUserStatus(int userId);
	void reloadUserStatus(int userId);
	bool isHiddenUser(int userId);
	
	void addHiddenUser(int userId);
	void removeHiddenUser(int userId);
	void reloadBlockUser(int userId);
	
	void modifySystemMessageBlock(int userId, MyUtil::StrSeq typeSeq);
	//void removeSystemMessageBlock(int userId, string type);
	MyUtil::StrSeq getSystemMessageBlock(int userId);
};

};
};
};

#endif

