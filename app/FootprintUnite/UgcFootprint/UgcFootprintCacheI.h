#ifndef __FootprintUniteI_h__
#define __FootprintUniteI_h__

#include <FootprintUnite.h>

namespace xce
{

namespace footprint
{

const static int SAVESIZE = 24;


class UgcFootprintCacheI;


typedef FootprintSet<BaseStepPtr,UgcFootprintCacheI> UgcFootprintSet;

typedef IceUtil::Handle<UgcFootprintSet> UgcFootprintSetPtr;


class UgcFootprintCacheI : virtual public UgcFootprintCache,
	public MyUtil::Singleton<UgcFootprintCacheI>{
public:
    UgcFootprintCacheI();
    virtual ~UgcFootprintCacheI();

    virtual void visit(const ::xce::footprint::FootprintInfoPtr&,
                       const Ice::Current& crt = Ice::Current());

    virtual void visitBatch(const ::xce::footprint::FootprintInfoSeq&,
                            const Ice::Current&);

    virtual void remove(const ::xce::footprint::FootprintInfoPtr&,
                        const Ice::Current&);

    virtual ::Ice::Int getSize(::Ice::Long,
                               const Ice::Current&);

    virtual ::xce::footprint::UgcInfoPtr getAll(::Ice::Long,
                                                ::Ice::Int,
                                                ::Ice::Int,
                                                ::Ice::Int,
                                                const Ice::Current& crt=Ice::Current());

    virtual bool isValid(const Ice::Current&);

    virtual void setValid(bool,
                          const Ice::Current&);

    virtual void contral(const ::MyUtil::Str2StrMap&,
                         const Ice::Current&);
    std::string getTableName(long owner);
    UgcFootprintSetPtr getFootprintSet(long id);
//    void evictFootprint(const FootprintInfoPtr& fpt, BaseStepSeq& seq, bool isSec=false);
private:

    bool isValid_;
    IceUtil::RWRecMutex validMutex_;

    SaveBucket<BaseStepPtr, UgcFootprintCacheI>*  saveBucket_;

    size_t cacheSize_;
    size_t dataSize_;
    size_t tableCount_; 
    size_t saveBatchSize_;
    size_t saveBatchTime_;
    std::string tableName_;
};
//SaveBucketPtr UgcFootprintCacheI::saveBucketPtr = NULL;


class FootprintFactoryI : public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(long id);
};

class FootprintBuildTask : public MyUtil::Task {
public:
        FootprintBuildTask(long id):MyUtil::Task(TASK_LEVEL_BUILD),_id(id) {};
        virtual void handle();
private:
        long _id;
};


//template<> int FootprintSet<BaseStep>::maxSave_ = 9;
}

}

#endif
