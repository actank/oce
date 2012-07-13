#ifndef XMPPERRORCODE_H_
#define XMPPERRORCODE_H_

namespace talk {
namespace error {

enum XmppErrorCode {
	none,			// 正常返回值
	success,		//创建房间成功
	jid_malformed, // RoomJid 不完整
	not_authorized, // no password provided
	registration_required, //需要注册方可进入，
	//没有member_only权限的人进入memberOnly Room
	forbidden, //被放入黑名单中，不能进入，
	//没有权限修改Room的title 和 subject时，、
	//如果Room关掉了每个人都可以invite的权限，当一个普通用户invite other时
	//非owner想要修改Room的配置
	//非owner想要修改MemberList
	conflict, //NickName冲突了
	service_unavailable, //服务不可用， 当房间人数到上限时
	//服务端没有实现的特性
	item_not_found, //房间没有找到，Locked Room等
	not_acceptable, //修改NickName不被接受
	bad_request, //如果想以message type=groupchat发送 private message
	not_allowed //没有权限kick比自己权限高的人,
	//没有权力修改别人的权限，
	//或是没有权限取消别人的voice
	//没有权限创建一个room


};

inline string error(XmppErrorCode err) {

	ostringstream text;

	switch (err) {
		case jid_malformed: {
			text << "<error type='modify'>"
					<< "<jid-malformed xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case not_authorized: {
			text << "<error type='auth'>"
					<< "<not-authorized xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";;
			break;
		}
		case registration_required: {
			text << "<error code='407' type='auth'>"
					<< "<registration-required xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case forbidden: {
			text << "<error type='auth'>"
					<< "<forbidden xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case conflict: {
			text << "<error type='cancel'>"
					<< "<conflict  xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case service_unavailable: {
			text << "<error type='wait'>"
					<< "<service-unavailable  xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case item_not_found: {
			text << "<error type='cancel'>"
					<< "<item-not-found  xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case not_acceptable: {
			text << "<error type='cancel'>"
					<< "<not-acceptable  xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}

		case bad_request: {
			text << "<error type='modify'>"
					<< "<bad-request  xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case not_allowed: {
			text << "<error type='cancel'>"
					<< "<not-allowed  xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
					<< "</error>";
			break;
		}
		case none:;
		case success:;
		
	};
	return text.str();
}
;
}
;
}
;

#endif /*XMPPERRORCODE_H_*/
