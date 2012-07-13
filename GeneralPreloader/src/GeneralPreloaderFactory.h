#ifndef __GENERALPRELOADERFACTORY_H__
#define __GENERALPRELOADERFACTORY_H__

#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>

namespace xce {
namespace generalpreloader {

class GeneralPreloaderFactory : public IceUtil::Shared {
public:
	enum createtype{
		idlist = 1,
		minmax = 2
	};

	virtual createtype getCreateType() = 0 ;

	virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) = 0; 

  //创建vector<long>型的id列表, 如果用户没有覆盖这个函数，就回调用create函数
  virtual MyUtil::ObjectResultPtr createWithLongIds(const MyUtil::LongSeq& ids) {
    MyUtil::IntSeq intIds(ids.begin(), ids.end());
    return create(intIds);
  }
	
	virtual MyUtil::ObjectResultPtr create(const Ice::Int minId,const Ice::Int maxId) = 0;
  //创建vector<long>型的id列表, 默认情况下这个函数会调用create函数
	virtual MyUtil::ObjectResultPtr createWithLongIds(const Ice::Long minId,const Ice::Long maxId) {
    return create((int)minId, (int)maxId);
  }
};
typedef IceUtil::Handle<GeneralPreloaderFactory> GeneralPreloaderFactoryPtr;

class IdListFactory : public GeneralPreloaderFactory {
public:
	virtual createtype getCreateType(){
		return idlist;
	}
	virtual MyUtil::ObjectResultPtr create(const Ice::Int minId,const Ice::Int maxId){
		return 0;
	}
	/*必须实现create(ids);*/
};

class MinMaxFactory : public GeneralPreloaderFactory {
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
