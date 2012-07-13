#ifndef _COMPAREINFOI_H_
#define _COMPAREINFOI_H_

#include "CompareInfo.h"
#include <Ice/Ice.h>
#include "TaskManager.h"
#include "ObjectCacheI.h"
#include "../data/CompareInfoData.h"
#include <CompareInfoReloaderAdapter.h>
#include "ServiceI.h"
#include "QueryRunner.h"

namespace xce
{
namespace compareinfo
{
    
    const Ice::Int COMPAREINFO_DATA = 0;
    const string COMPAREINFO_MANAGER = "CM";
    const int TASK_LEVEL_FILL = 2;
    const int TASK_LEVEL_UPDATE = 10;
    
    class CompareInfoManagerI : virtual public CompareInfoManager, virtual public MyUtil::Singleton<CompareInfoManagerI>
    {
    public:
	CompareInfoManagerI(){}
 	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current()); 
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
        virtual void update(const CompareInfoUpdateNewDataPtr& data, const Ice::Current& current=::Ice::Current());
        virtual bool CompareInfo(int userId1, int userId2, const ::Ice::Current& = ::Ice::Current());
        virtual bool CompareInfoByTable(int userId1, int userId2, const ::xce::compareinfo::byteSeq & table , const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const Ice::Current& = ::Ice::Current());

        bool _compare_user_vector(std::vector<long>  vector1,std::vector<long>  vector2);
    private:
        bool _valid;
	IceUtil::RWRecMutex _validMutex;

    };

    class CompareInfoDataFactoryI : virtual public ServantFactory
    {
     public:
         virtual Ice::ObjectPtr create(Ice::Int id);
    };
 

    class FillTask: virtual public ::MyUtil::Task 
    { 
    public:
         FillTask(const ::MyUtil::ObjectResultPtr& userData):Task(TASK_LEVEL_FILL),_userData(userData)
	 {}
	 virtual void handle();

     private:
          ::MyUtil::ObjectResultPtr _userData;
     };  //template FillTask


   class UpdateTaskI: public MyUtil::Task {
   public:
	UpdateTaskI(const CompareInfoUpdateNewDataPtr& data, const Ice::Current& current):Task(TASK_LEVEL_UPDATE),data_(data), current_(current){}
	virtual void handle();
        virtual void UpdateData(CompareInfoDataIPtr& data, const CompareInfoUpdateNewDataPtr& updateddata);

   private:
	//void UpdateData(CompareInfoEntryPtr& data, const Byte2StrMap&);
	CompareInfoUpdateNewDataPtr data_;
	Ice::Current current_;
   };

}
  

}

#endif
