#ifndef __CACHELOADERFACTORY_H__
#define __CACHELOADERFACTORY_H__

#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>

namespace xce {
namespace cacheloader {

class CacheLoaderFactory : public IceUtil::Shared {
public:
	enum createtype{
		idlist = 1,
		minmax = 2
	};

	virtual createtype getCreateType() = 0 ;

	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) = 0;

        virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId) = 0;
};
typedef IceUtil::Handle<CacheLoaderFactory> CacheLoaderFactoryPtr;

class IdListFactory : public CacheLoaderFactory {
public:
	virtual createtype getCreateType(){
		return idlist;
	}
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId){
		return 0;
	}
	/*必须实现create(ids);*/
};

class MinMaxFactory : public CacheLoaderFactory {
public:
	virtual createtype getCreateType(){
		return minmax;
	}
	/*必须实现create(ids);*/
	/*必须实现create(min,max);*/
};

}
}

#endif
