#ifndef __BatchUpdaterI_h__
#define __BatchUpdaterI_h__

#include "ServiceI.h"
#include "BatchUpdater.h"
#include "CountUpdater.h"
#include "StateUpdater.h"
//#include "CountCache.h"
#include "BatchFactory.h"
#include "BatchTraitsBase.h"
#include "BatchTraits.h"
namespace mop
{
namespace hi
{
namespace svc
{
namespace model
{

const string MANAGER="M";

class BatchUpdaterManagerI : virtual public BatchUpdaterManager
{
public:
	BatchUpdaterManagerI();

	~BatchUpdaterManagerI();


	virtual void inc(const MyUtil::IntSeq& keyvalue,::Ice::Int count,const std::string& type, const Ice::Current&);
	virtual void dec(const MyUtil::IntSeq& keyvalue,::Ice::Int count,const std::string& type, const Ice::Current&);
	virtual void set(const MyUtil::IntSeq& keyvalue,const std::string& value,const std::string& type, const Ice::Current&);

	virtual void incLong(const MyUtil::LongSeq& keyvalue,::Ice::Int count,const std::string& type, const Ice::Current&);
        virtual void decLong(const MyUtil::LongSeq& keyvalue,::Ice::Int count,const std::string& type, const Ice::Current&);
        virtual void setLong(const MyUtil::LongSeq& keyvalue,const std::string& value,const std::string& type, const Ice::Current&);


    virtual void incUserViewCount(::Ice::Int,
		::Ice::Int count,
				  const Ice::Current&);

    virtual void incLoginCount(::Ice::Int,
		::Ice::Int count,
			       const Ice::Current&);

    virtual void setLastLoginTime(::Ice::Int,
			       const string&,
			       const Ice::Current&);

    virtual void setLastLogoutTime(::Ice::Int,
			       const string&,
			       const Ice::Current&);

    virtual void setLastModifyTime(::Ice::Int,
			       const string&,
			       const Ice::Current&);

	virtual void incFriendDist(::Ice::Int,
			::Ice::Int, ::Ice::Int,
			const Ice::Current&);

	virtual void decFriendDist(::Ice::Int,
			::Ice::Int, ::Ice::Int,
			const Ice::Current&);

    virtual void incExp(::Ice::Int, ::Ice::Int,
			const Ice::Current&);

    virtual void decExp(::Ice::Int, ::Ice::Int,
			const Ice::Current&);

    virtual void incTribeCount(::Ice::Int, ::Ice::Int,
			       const Ice::Current&);

    virtual void decTribeCount(::Ice::Int, ::Ice::Int,
			       const Ice::Current&);

    virtual void incTribeRequestCount(::Ice::Int, ::Ice::Int,
				      const Ice::Current&);

    virtual void decTribeRequestCount(::Ice::Int, ::Ice::Int,
				      const Ice::Current&);

    virtual void incNewMessageCount(::Ice::Int, ::Ice::Int,
				    const Ice::Current&);

    virtual void decNewMessageCount(::Ice::Int, ::Ice::Int,
				    const Ice::Current&);

    virtual void incMessageInCount(::Ice::Int, ::Ice::Int,
				   const Ice::Current&);

    virtual void decMessageInCount(::Ice::Int, ::Ice::Int,
				   const Ice::Current&);

    virtual void incMessageOutCount(::Ice::Int, ::Ice::Int,
				    const Ice::Current&);

    virtual void decMessageOutCount(::Ice::Int, ::Ice::Int,
				    const Ice::Current&);

    virtual void incFriendCount(::Ice::Int,
				::Ice::Int,
				const Ice::Current&);

    virtual void decFriendCount(::Ice::Int,
				::Ice::Int,
				const Ice::Current&);


    virtual void incGuestRequestCount(::Ice::Int,
				      ::Ice::Int,
				      const Ice::Current&);

    virtual void decGuestRequestCount(::Ice::Int,
				      ::Ice::Int,
				      const Ice::Current&);

    virtual void incLeavewordCount(::Ice::Int,
				      ::Ice::Int,
				      const Ice::Current&);

    virtual void decLeavewordCount(::Ice::Int,
				      ::Ice::Int,
				      const Ice::Current&);

	virtual void incBlogViewCount(Ice::Int id,
		   			  Ice::Int userId,
					  Ice::Int count,
					  const Ice::Current&);

	virtual void decBlogViewCount(Ice::Int id,
		   			  Ice::Int userId,
					  Ice::Int count,
					  const Ice::Current&);

	virtual void incPhotoViewCount(Ice::Int id,
		   			  Ice::Int userId,
				      Ice::Int count,
					  const Ice::Current&);

	virtual void decPhotoViewCount(Ice::Int id,
		   			  Ice::Int userId,
					  Ice::Int count,
					  const Ice::Current&);

	virtual void incTribePageViewCount(Ice::Int tribeId,
					  Ice::Int count,
					  const Ice::Current&);
	virtual void incFlyerUserAdCount(Ice::Int userId,
		   			  Ice::Int adId,
					  Ice::Int count,
					  const Ice::Current&);
	

private:

	std::map<std::string,mop::hi::svc::batchtraits::BatchTraitBase*> _mapTrait;
//	StateUpdater* _lastLoginTime;
//	StateUpdater* _lastLogoutTime;
//	StateUpdater* _lastModifyTime;

};
};
};
};
};

#endif
