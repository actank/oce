#include "BatchTraits.h"
using namespace mop::hi::svc::model;
using namespace std;
using namespace MyUtil;
using namespace mop::hi::svc::batchtraits;

//****************************************************************************************************
string UserViewTrait::name;
UserViewTrait::UserViewTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("user_view_count", "view_count", "viewcount", idColumns);
}

std::string& UserViewTrait::getName() {
	return name;
}

//****************************************************************************************************
string NewMessageTrait::name;
NewMessageTrait::NewMessageTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("user_common", "users", "newmessagecount", idColumns, 10);
}

std::string& NewMessageTrait::getName() {
	return name;
}

//****************************************************************************************************
string LoginTrait::name;
LoginTrait::LoginTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	//_counter = new CountUpdater("user_common", "users", "logincount", idColumns);
	_counter = new CountUpdater(
		DescDatabase("user_logincount", "", 0),
		DescSqlbuilder("user_logincount", "logincount", idColumns, true),
		5);
}

std::string& LoginTrait::getName() {
	return name;
}

//****************************************************************************************************
string MessageInTrait::name;
MessageInTrait::MessageInTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("self_message", "message_count", "message_in_count", idColumns, 3);
}

std::string& MessageInTrait::getName() {
	return name;
}

//****************************************************************************************************
string FriendTrait::name;
FriendTrait::FriendTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("user_common", "users", "friendcount", idColumns);
}

std::string& FriendTrait::getName() {
	return name;
}

//****************************************************************************************************
string FriendDistTrait::name;
FriendDistTrait::FriendDistTrait() {
	StrSeq idColumns;
	idColumns.push_back("host");
	idColumns.push_back("univ");
	_counter
			= new CountUpdater("friend_dist", "friend_dist", "count_all", idColumns);
}

std::string& FriendDistTrait::getName() {
	return name;
}

//****************************************************************************************************
string ExpTrait::name;
ExpTrait::ExpTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter = new CountUpdater("user_common", "users", "exp", idColumns);
}

std::string& ExpTrait::getName() {
	return name;
}

//****************************************************************************************************
string TribeTrait::name;
TribeTrait::TribeTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("user_common", "users", "tribe_count", idColumns);
}

std::string& TribeTrait::getName() {
	return name;
}

//****************************************************************************************************
string TribeRequestTrait::name;
TribeRequestTrait::TribeRequestTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("user_common", "users", "tribe_requestcount", idColumns);
}

std::string& TribeRequestTrait::getName() {
	return name;
}

//****************************************************************************************************
string MessageOutTrait::name;
MessageOutTrait::MessageOutTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("self_message", "message_count", "message_out_count", idColumns, 3);
}

std::string& MessageOutTrait::getName() {
	return name;
}

//****************************************************************************************************
string TribePageViewTrait::name;
TribePageViewTrait::TribePageViewTrait() {
	StrSeq idColumns;
	idColumns.push_back("tribe_id");
	_counter = new CountUpdater("tribe", "tribe_pageview", "pv", idColumns, 10);
}

std::string& TribePageViewTrait::getName() {
	return name;
}

//****************************************************************************************************
string FlyerUserAdTrait::name;
FlyerUserAdTrait::FlyerUserAdTrait() {
	StrSeq idColumns;
	idColumns.push_back("userid");
	idColumns.push_back("adid");
	_counter
			= new CountUpdater("flyer", "flyer_user", "count", idColumns);
}

std::string& FlyerUserAdTrait::getName() {
	return name;
}

//****************************************************************************************************
string LeavewordTrait::name;
LeavewordTrait::LeavewordTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("self", "leaveword_count", "leaveword_count", idColumns, 3);
}

std::string& LeavewordTrait::getName() {
	return name;
}

//****************************************************************************************************
string BlogViewTrait::name;
BlogViewTrait::BlogViewTrait() {
	StrSeq idColumns;
	idColumns.push_back("owner");
	idColumns.push_back("id");
	
	_counter
                        = new CountUpdater("blog_view_count", "blog_", "view_count", idColumns, 15, 100);

}
std::string& BlogViewTrait::getName() {
	return name;
}

//****************************************************************************************************
string PhotoViewTrait::name;
PhotoViewTrait::PhotoViewTrait() {
	StrSeq idColumns;
	idColumns.push_back("owner");
	idColumns.push_back("id");
	
	_counter = new CountUpdater("photo_view_count", "photo_", "viewcount", idColumns, 1, 100, false, 1);
}

std::string& PhotoViewTrait::getName() {
	return name;
}

//****************************************************************************************************
string PhotoViewNTrait::name;
PhotoViewNTrait::PhotoViewNTrait() {
	StrSeq idColumns;
	idColumns.push_back("owner");
	idColumns.push_back("id");
	idColumns.push_back("album");
	
	_counter = new CountUpdater("photo_viewcount_new", "photo_viewcount_", "viewcount", idColumns, 1, 100, true, 1);
}

std::string& PhotoViewNTrait::getName() {
	return name;
}

//****************************************************************************************************
string ShareVideoViewCountTrait::name;
ShareVideoViewCountTrait::ShareVideoViewCountTrait() {
	StrSeq idColumns;
	idColumns.push_back("share_id");
	idColumns.push_back("share_owner");
	_counter
                        = new CountUpdater("XceAdapter.DB_SHARE", "share_source_", "view_count", idColumns, 3, 100);

}

std::string& ShareVideoViewCountTrait::getName() {
	return name;
}



//****************************************************************************************************
string GuestRequestTrait::name;
GuestRequestTrait::GuestRequestTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("user_common", "users", "guestrequestcount", idColumns);
}

std::string& GuestRequestTrait::getName() {
	return name;
}

//****************************************************************************************************
string GeneralUserApiCountTrait::name;
GeneralUserApiCountTrait::GeneralUserApiCountTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	idColumns.push_back("type");
	_counter
			= new CountUpdater("user_visit_count", "user_visit_count_", "count", idColumns, 1, 100, true);
}

std::string& GeneralUserApiCountTrait::getName() {
	return name;
}

//****************************************************************************************************
string GeneralUserCountTrait::name;
GeneralUserCountTrait::GeneralUserCountTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	idColumns.push_back("type");
	_counter
			= new CountUpdater("user_count", "user_count_", "count", idColumns, 1, 100, true);
}

std::string& GeneralUserCountTrait::getName() {
	return name;
}

//****************************************************************************************************
string FeedCommentTrait::name;
FeedCommentTrait::FeedCommentTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new CountUpdater("feed_content", "feed_content", "comment_count", idColumns);
}

std::string& FeedCommentTrait::getName() {
	return name;
}

//****************************************************************************************************
string UgcUserScoreTrait::name;
UgcUserScoreTrait::UgcUserScoreTrait() {
	StrSeq idColumns;
	idColumns.push_back("userid");
	idColumns.push_back("type");
	_counter
			= new CountUpdater(DescDatabase("user_score", "user_score_", 100), DescSqlbuilder(
					"ugc_score_record_", "count", idColumns, true), 1);
}

std::string& UgcUserScoreTrait::getName() {
	return name;
}

//****************************************************************************************************
string UserScoreTrait::name;
UserScoreTrait::UserScoreTrait() {
	StrSeq idColumns;
	idColumns.push_back("userid");
	_counter
			= new CountUpdater(DescDatabase("user_score", "user_score_", 0), DescSqlbuilder(
					"user_score", "current_score", idColumns, false), 1);
}

std::string& UserScoreTrait::getName() {
	return name;
}

//****************************************************************************************************
string GeneralUserApiSetterTrait::name;
GeneralUserApiSetterTrait::GeneralUserApiSetterTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	idColumns.push_back("type");
	_counter
			= new StateUpdater("user_visit_count", "user_visit_count_", "count", idColumns, 100);
}

std::string& GeneralUserApiSetterTrait::getName() {
	return name;
}

//****************************************************************************************************
string GeneralUserSetterTrait::name;
GeneralUserSetterTrait::GeneralUserSetterTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	idColumns.push_back("type");
	_counter
			= new StateUpdater("user_count", "user_count_", "count", idColumns, 100, true);
}

std::string& GeneralUserSetterTrait::getName() {
	return name;
}

string LastLoginTimeTrait::name;
LastLoginTimeTrait::LastLoginTimeTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new StateUpdater("user_desc", "user_desc", "lastlogintime", idColumns);
}

std::string& LastLoginTimeTrait::getName() {
	return name;
}

string LastLogoutTimeTrait::name;
LastLogoutTimeTrait::LastLogoutTimeTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new StateUpdater("user_desc", "user_desc", "lastlogouttime", idColumns);
}

std::string& LastLogoutTimeTrait::getName() {
	return name;
}

string LastModifyTimeTrait::name;
LastModifyTimeTrait::LastModifyTimeTrait() {
	StrSeq idColumns;
	idColumns.push_back("id");
	_counter
			= new StateUpdater("active_user", "user_last_modify", "modify_time", idColumns);
}

std::string& LastModifyTimeTrait::getName() {
	return name;
}

//****************************************************************************************************
string NotifyCountTrait::name;
NotifyCountTrait::NotifyCountTrait() {
        StrSeq idColumns;
        idColumns.push_back("id");
        _counter
                        = new CountUpdater("notify_count", "notify_count_", "count", idColumns, 1, 100, true);
}

std::string& NotifyCountTrait::getName() {
        return name;
}

//****************************************************************************************************
string NotifySetterTrait::name;
NotifySetterTrait::NotifySetterTrait() {
        StrSeq idColumns;
        idColumns.push_back("id");
        _counter
                        = new StateUpdater("notify_count", "notify_count_", "count", idColumns, 100, true);
}

std::string& NotifySetterTrait::getName() {
        return name;
}



