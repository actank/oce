#ifndef BATCHFACTORY_H_
#define BATCHFACTORY_H_

#include <map>
#include <string>
#include <utility>
#include "Typelist.h"
#include "BatchTraitsBase.h"
#include "BatchTraits.h"

namespace mop
{
namespace hi
{
namespace svc
{
namespace batchtraits
{
	/********************************************
	 *目前有26个trait类，所以使用了TYPELIST_26，如果trait增加了，可以直接在现面增加个，
	 *并改成TYPELIST_27，或者可以再typedef个Append
	 *
	 *
	 *
	 *
	 *
	 *******************************************/

	typedef TYPELIST_30(mop::hi::svc::batchtraits::UserViewTrait,mop::hi::svc::batchtraits::NewMessageTrait,
			mop::hi::svc::batchtraits::LoginTrait,mop::hi::svc::batchtraits::MessageInTrait,
			mop::hi::svc::batchtraits::FriendTrait,mop::hi::svc::batchtraits::GuestRequestTrait,
			mop::hi::svc::batchtraits::FriendDistTrait,mop::hi::svc::batchtraits::ExpTrait,
			mop::hi::svc::batchtraits::TribeTrait,mop::hi::svc::batchtraits::TribeRequestTrait,
			mop::hi::svc::batchtraits::MessageOutTrait,mop::hi::svc::batchtraits::TribePageViewTrait,
			mop::hi::svc::batchtraits::FlyerUserAdTrait,mop::hi::svc::batchtraits::LeavewordTrait,
			mop::hi::svc::batchtraits::BlogViewTrait,mop::hi::svc::batchtraits::PhotoViewTrait,
			mop::hi::svc::batchtraits::FeedCommentTrait,mop::hi::svc::batchtraits::GeneralUserCountTrait,
			mop::hi::svc::batchtraits::UgcUserScoreTrait,mop::hi::svc::batchtraits::UserScoreTrait,
			mop::hi::svc::batchtraits::GeneralUserSetterTrait,mop::hi::svc::batchtraits::LastLoginTimeTrait,
			mop::hi::svc::batchtraits::LastLogoutTimeTrait,mop::hi::svc::batchtraits::LastModifyTimeTrait,
			mop::hi::svc::batchtraits::GeneralUserApiCountTrait,mop::hi::svc::batchtraits::GeneralUserApiSetterTrait,
			mop::hi::svc::batchtraits::ShareVideoViewCountTrait,mop::hi::svc::batchtraits::NotifyCountTrait,
			mop::hi::svc::batchtraits::NotifySetterTrait,mop::hi::svc::batchtraits::PhotoViewNTrait)
		BatchTraitsList;
	class BatchFactory
	{
	public:
		BatchFactory();
		std::map<std::string,mop::hi::svc::batchtraits::BatchTraitBase*> create();
	private:
		std::map<std::string,mop::hi::svc::batchtraits::BatchTraitBase*> res;
		/*
		template <class T>
		void build()
		{
			res.insert(std::make_pair(T::Head::getName(),(mop::hi::svc::batchtraits::BatchTraitBase*)(new typename T::Head)));
			build<typename T::Tail>();
		}*/


	//	template<> void build<MyUtil::TypelistUtil::NullType>(){}
	};

	//template<> void BatchFactory::build<MyUtil::TypelistUtil::NullType>(){}


};
};
};
};


#endif
