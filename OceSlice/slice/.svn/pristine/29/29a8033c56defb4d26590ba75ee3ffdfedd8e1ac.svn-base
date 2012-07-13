#ifndef __IM_GATE_FOR_PHONE_ICE__
#define __IM_GATE_FOR_PHONE_ICE__ 

#include <TalkCommon.ice>
module com
{
module xiaonei
{
module talk
{
module gate
{
	class IMGateForPhoneManager
	{
		void SendMessage(int phoneuserid, int touserid, string message);//手机用户发送聊天消息
		void PhoneUserOnline(int phoneuserid);//手机用户上线
		void PhoneUserOffline(int phoneuserid);//手机用户下线
		
		void ServerDisable();//调用后，停止这个服务对IM上层服务的一切调用
		void ServerEnable();//调用后，允许这个服务对IM上层服务进行调用
	};
};
};
};
};

#endif
