#ifndef __BUDDYBYNAMERELOADER_H__
#define __BUDDYBYNAMERELOADER_H__

#include <ServiceI.h>
#include "buddy/BuddyByNameCache/share/BuddyByNameTask.h"

namespace xce {
  namespace buddybynameloader {

    using namespace xce::buddy;	

    class BuddyByNameLoaderI : virtual public BuddyByNameLoader,
    virtual public MyUtil::Singleton<BuddyByNameLoaderI> {
      public:
        virtual void reload(::Ice::Int,const Ice::Current&);
        virtual void load(const MyUtil::Int2IntSeqMap&, const Ice::Current&);
        virtual void rename(::Ice::Int, const std::string&, const Ice::Current&);
      private:
        friend class MyUtil::Singleton<BuddyByNameLoaderI>;
    };

    //------------------------------------------------------------------------------

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };

  }
}

#endif

