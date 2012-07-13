#include "BatchUpdater.h"
#include "BatchFactory.h"

using namespace mop::hi::svc::batchtraits;
using namespace std;



//构造函数中主要是对这些Traits类的名字的赋值

//template<>
//void mop::hi::svc::batchtraits::BatchFactory::build<MyUtil::TypelistUtil::NullType>(){}

template <class T>
void buildres(std::map<std::string,mop::hi::svc::batchtraits::BatchTraitBase*>& res);

BatchFactory::BatchFactory()
{
	UserViewTrait::getName()=UserViewName;
	NewMessageTrait::getName()=NewMessageName;
	LoginTrait::getName()=LoginName;
	MessageInTrait::getName()=MessageInName;
	FriendTrait::getName()=FriendName;
	GuestRequestTrait::getName()=GuestRequestName;
	FriendDistTrait::getName()=FriendDistName;
	ExpTrait::getName()=ExpName;
	TribeTrait::getName()=TribeName;
	TribeRequestTrait::getName()=TribeRequestName;
	MessageOutTrait::getName()=MessageOutName;
	TribePageViewTrait::getName()=TribePageViewName;
	FlyerUserAdTrait::getName()=FlyerUserAdName;
	LeavewordTrait::getName()=LeavewordName;
	BlogViewTrait::getName()=BlogViewName;
	PhotoViewTrait::getName()=PhotoViewName;
	FeedCommentTrait::getName()=FeedCommentName;
	GeneralUserApiCountTrait::getName()=GeneralUserApiCountName;
	GeneralUserCountTrait::getName()=GeneralUserCountName;
	UgcUserScoreTrait::getName()=UgcUserScoreName;
	UserScoreTrait::getName()=UserScoreName;
	GeneralUserSetterTrait::getName()=GeneralUserSetterName;
	GeneralUserApiSetterTrait::getName()=GeneralUserApiSetterName;
	LastLoginTimeTrait::getName()=LastLoginName;
	LastLogoutTimeTrait::getName()=LastLogoutName;
	LastModifyTimeTrait::getName()=LastModifyName;

	ShareVideoViewCountTrait::getName()=ShareVideoViewName;
	NotifyCountTrait::getName()=NotifyCountName;
	NotifySetterTrait::getName()=NotifySetterName;
	PhotoViewNTrait::getName()=PhotoViewNName;
}



map<string,BatchTraitBase*> BatchFactory::create()
{
	buildres<BatchTraitsList>(res);
	return res;
}

template <class T>
inline void buildres(std::map<std::string,mop::hi::svc::batchtraits::BatchTraitBase*>& res)
{
	res.insert(std::make_pair(T::Head::getName(),(mop::hi::svc::batchtraits::BatchTraitBase*)(new typename T::Head)));
	buildres<typename T::Tail>(res);
}

template<>
inline void buildres<MyUtil::TypelistUtil::NullType>(std::map<std::string,mop::hi::svc::batchtraits::BatchTraitBase*>& res)
{}
