/**********************************************************************
 *27个类
 *
 *
 *
 *
 *
 *
 ***********************************************************************/

#ifndef _BATCHTRAITS_H_
#define _BATCHTRAITS_H_
#include <string>
#include "BatchTraitsBase.h"

namespace mop {
namespace hi {
namespace svc {
namespace batchtraits {
class UserViewTrait: public BatchTraitBase {
public:
	UserViewTrait();
	static std::string& getName();
private:
	static std::string name;
};

class NewMessageTrait: public BatchTraitBase {
public:
	NewMessageTrait();
	static std::string& getName();
private:
	static std::string name;
};

class LoginTrait: public BatchTraitBase {
public:
	LoginTrait();
	static std::string& getName();
private:
	static std::string name;
};

class MessageInTrait: public BatchTraitBase {
public:
	MessageInTrait();
	static std::string& getName();
private:
	static std::string name;
};

class FriendTrait: public BatchTraitBase {
public:
	FriendTrait();
	static std::string& getName();
private:
	static std::string name;
};

class FriendDistTrait: public BatchTraitBase {
public:
	FriendDistTrait();
	static std::string& getName();
private:
	static std::string name;
};

class ExpTrait: public BatchTraitBase {
public:
	ExpTrait();
	static std::string& getName();
private:
	static std::string name;
};

class TribeTrait: public BatchTraitBase {
public:
	TribeTrait();
	static std::string& getName();
private:
	static std::string name;
};

class TribeRequestTrait: public BatchTraitBase {
public:
	TribeRequestTrait();
	static std::string& getName();
private:
	static std::string name;
};

class MessageOutTrait: public BatchTraitBase {
public:
	MessageOutTrait();
	static std::string& getName();
private:
	static std::string name;
};

class TribePageViewTrait: public BatchTraitBase {
public:
	TribePageViewTrait();
	static std::string& getName();
private:
	static std::string name;
};

class FlyerUserAdTrait: public BatchTraitBase {
public:
	FlyerUserAdTrait();
	static std::string& getName();
private:
	static std::string name;
};

class LeavewordTrait: public BatchTraitBase {
public:
	LeavewordTrait();
	static std::string& getName();
private:
	static std::string name;
};

class BlogViewTrait: public BatchTraitBase {
public:
	BlogViewTrait();
	static std::string& getName();
private:
	static std::string name;
};

class PhotoViewTrait: public BatchTraitBase {
public:
	PhotoViewTrait();
	static std::string& getName();
private:
	static std::string name;
};

class PhotoViewNTrait: public BatchTraitBase {
public:
	PhotoViewNTrait();
	static std::string& getName();
private:
	static std::string name;
};

class GuestRequestTrait: public BatchTraitBase {
public:
	GuestRequestTrait();
	static std::string& getName();
private:
	static std::string name;
};

class GeneralUserApiCountTrait: public BatchTraitBase {
public:
	GeneralUserApiCountTrait();
	static std::string& getName();
private:
	static std::string name;
};

class GeneralUserCountTrait: public BatchTraitBase {
public:
	GeneralUserCountTrait();
	static std::string& getName();
private:
	static std::string name;
};

class FeedCommentTrait: public BatchTraitBase {
public:
	FeedCommentTrait();
	static std::string& getName();
private:
	static std::string name;
};

class UgcUserScoreTrait: public BatchTraitBase {
public:
	UgcUserScoreTrait();
	static std::string& getName();
private:
	static std::string name;
};

class UserScoreTrait: public BatchTraitBase {
public:
	UserScoreTrait();
	static std::string& getName();
private:
	static std::string name;
};

class GeneralUserSetterTrait: public BatchTraitBase {
public:
	GeneralUserSetterTrait();
	static std::string& getName();
private:
	static std::string name;
};

class GeneralUserApiSetterTrait: public BatchTraitBase {
public:
	GeneralUserApiSetterTrait();
	static std::string& getName();
private:
	static std::string name;
};

class LastLoginTimeTrait: public BatchTraitBase {
public:
	LastLoginTimeTrait();
	static std::string& getName();
private:
	static std::string name;
};

class LastLogoutTimeTrait: public BatchTraitBase {
public:
	LastLogoutTimeTrait();
	static std::string& getName();
private:
	static std::string name;
};

class LastModifyTimeTrait: public BatchTraitBase {
public:
	LastModifyTimeTrait();
	static std::string& getName();
private:
	static std::string name;
};

class ShareVideoViewCountTrait: public BatchTraitBase {
public:
	ShareVideoViewCountTrait();
	static std::string& getName();
private:
	static std::string name;
};

class NotifyCountTrait: public BatchTraitBase {
public:
        NotifyCountTrait();
        static std::string& getName();
private:
        static std::string name;
};

class NotifySetterTrait: public BatchTraitBase {
public:
        NotifySetterTrait();
        static std::string& getName();
private:
        static std::string name;
};



}
;
}
;
}
;
}
;

#endif

