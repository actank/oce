#ifndef _BATCHTRAITSBASE_H_
#define _BATCHTRAITSBASE_H_
#include <string>
#include "Util.h"
#include "CountUpdater.h"
#include "StateUpdater.h"


namespace mop
{
namespace hi
{
namespace svc
{
namespace batchtraits
{

	class BatchTraitBase
	{
	public:
		virtual void inc(const MyUtil::IntSeq& keyValue,::Ice::Int count){
			
			 MyUtil::LongSeq keyvalue;
			 for(std::vector<int>::const_iterator it = keyValue.begin();it!=keyValue.end();it++){
		                keyvalue.push_back((long)(*it));
		         }
			incLong(keyvalue, count);
		}
		virtual void dec(const MyUtil::IntSeq& keyValue,::Ice::Int count){
			 MyUtil::LongSeq keyvalue;
                         for(std::vector<int>::const_iterator it = keyValue.begin();it!=keyValue.end();it++){
                                keyvalue.push_back((long)(*it));
                         }
			decLong(keyvalue, count);

		}
		virtual void set(const MyUtil::IntSeq& keyValue,const std::string& value){
			 MyUtil::LongSeq keyvalue;
                         for(std::vector<int>::const_iterator it = keyValue.begin();it!=keyValue.end();it++){
                                keyvalue.push_back((long)(*it));
                         }
			setLong(keyvalue, value);
		}
		
		virtual void incLong(const MyUtil::LongSeq& keyValue,::Ice::Int count){
			if (!_counter) return;
                        mop::hi::svc::model::CountUpdater* ptr = dynamic_cast<mop::hi::svc::model::CountUpdater*>(_counter);
                        if (ptr) ptr->fix(keyValue,count);
                }
                virtual void decLong(const MyUtil::LongSeq& keyValue,::Ice::Int count){
                        if (!_counter) return;
                        mop::hi::svc::model::CountUpdater* ptr = dynamic_cast<mop::hi::svc::model::CountUpdater*>(_counter);
                        if (ptr) ptr->fix(keyValue,count*(-1));
                }
                virtual void setLong(const MyUtil::LongSeq& keyValue,const std::string& value){
                        if (!_counter) return;
                        mop::hi::svc::model::StateUpdater* ptr = dynamic_cast<mop::hi::svc::model::StateUpdater*>(_counter);
                        if (ptr) ptr->fix(keyValue,value);
                }

	

		virtual ~BatchTraitBase()
		{
			delete _counter;
		}
	protected:
		IceUtil::Thread* _counter;
	};

};
};
};
};
#endif
