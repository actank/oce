#ifndef __XCE_DIST_CACHE_ADAPTER_H__
#define __XCE_DIST_CACHE_ADAPTER_H__

#include "ServiceI.h"

#include "distcache/distcache.h"

namespace xce
{
  namespace distcache
  {

    class MemFactory : virtual public IceUtil::Shared {
      public:

        virtual Ice::ObjectPtr create(Ice::Long id){
          return NULL;
        }

        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids){
          MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
          for (size_t i = 0; i < ids.size(); ++i) {
            try {
              Ice::ObjectPtr obj = create(ids.at(i));
              if (obj) {
                res->data[ids.at(i)] = obj;
              }
            } catch (...) {
              //
            }
          }
          return res;
        }

        virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids) {
          MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
          for (size_t i = 0; i < ids.size(); ++i) {
            try {
              Ice::ObjectPtr obj = create(ids.at(i));
              if (obj) {
                res->data[ids.at(i)] = obj;
              }
            } catch (...) {
              //
            }
          }
          return res;
        }

    };

    typedef IceUtil::Handle<MemFactory> MemFactoryPtr; 

    template<class T, class P> class XceDistCacheRAdapter
    {
      public:
        XceDistCacheRAdapter() : 
          _name(""), _factory(NULL){
          }

        virtual ~XceDistCacheRAdapter() {}

      public:

        virtual void init(std::string& name, MemFactoryPtr factory = NULL) {
          if( 0 == name.length() ){
            MCE_WARN("DistCacheReloader::init name is null, it won't connnet to zookeeper correctly.");
          }
          if( !factory){
            MCE_WARN("DistCacheReloader::init factory is null, it won't create obj while get missed.");
          }
          _name = name;
          _factory = factory;
          try {
            while(NULL == (_distCacheAdapterPtr = DistCacheAdapter::connect(name))) {
              MCE_INFO("DistCacheReloader, name: " << name << " connecting distcache ...");
            }
            MCE_INFO("DistCacheReloader, name: " << name << " connecting distcache is success!");
          } catch (...) {
            MCE_WARN("DistCacheReloader::init is fail!");
          }
        }

        P get(long id)
        {
          string dataStr; 
          xce::distcache::DataPtr dataPtr = _distCacheAdapterPtr->get(id); 
          if( NULL == dataPtr ) {
            if( !_factory){
              MCE_WARN("XceDistCacheRAdapter::get not in memcached userid = " << id << " and won't reload.");
              return NULL;
            } else {
              MCE_WARN("XceDistCacheRAdapter::get not in memcached userid = " << id << " and will reload.");
              Ice::ObjectPtr obj = _factory->create(id);
              if(!obj){
                MCE_WARN("XceDistCacheRAdapter::get not in memcached userid = " << id << " and create null object.");
                return NULL;
              }
              P data = P::dynamicCast(obj);
              data->SerializeToString(&dataStr);
              DataPtr result(new Data( dataStr.begin(), dataStr.end() ));
              return data;
            }
          }

          dataStr.assign(dataPtr->begin(), dataPtr->end());
          P data = new T;
          data->ParseFromString(dataStr); 
          return data;
        }

        std::map<long, P> get(const std::vector<long>& ids, std::vector<long>& missIds )
        {
          map<long, P> res;
          map<long, xce::distcache::DataPtr> propMap = _distCacheAdapterPtr->get(ids);
          for(map<long, xce::distcache::DataPtr>::const_iterator it = propMap.begin(); it != propMap.end(); ++it)
          {
            if(it->second == NULL)
            {
              MCE_WARN("XceDistCacheRAdapter::multiget not in memcached userid = " << it->first);
              continue;
            }
            string dataStr;
            dataStr.assign(it->second->begin(), it->second->end());
            P data = new T;
            data->ParseFromString(dataStr);
            res.insert(make_pair(it->first,data));
          }

          //reload missed ids
          ostringstream os;
          os<<"XceDistCacheRAdapter::multiget not in memcached userid ="; 
          if( propMap.size() != ids.size()){
            for( std::vector<long>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
              map<long, xce::distcache::DataPtr>::iterator fit = propMap.find(*it);
              //missed
              if( propMap.end() == fit ){
                missIds.push_back(*it);
                os<<" "<<*it;
              }
              if( missIds.size() >= (ids.size() - propMap.size()) ){
                break;
              }
            }
            if( !_factory){
              MCE_WARN(os.str()<<" and won't reload.");
            } else {
              MCE_WARN(os.str()<<" and will reload.");
              MyUtil::ObjectResultPtr misRes = _factory->create(missIds);
              if(!misRes->data.empty()){
                for(MyUtil::ObjectMap::const_iterator it = misRes->data.begin(); it != misRes->data.end(); ++it) {
                  res.insert(make_pair(it->first, P::dynamicCast(it->second)));
                }
              }
            }
          }
          return res;
        }

      protected:
        std::string _name;
        MemFactoryPtr _factory;	
        xce::distcache::DistCacheAdapterPtr _distCacheAdapterPtr;
    };

    template<class T, class P> class XceDistCacheRWAdapter : virtual public XceDistCacheRAdapter<T, P>
    {
      public:

        virtual ~XceDistCacheRWAdapter() {}

      public:

        bool set(const P& data)
        {
          string dataStr;
          data->SerializeToString(&dataStr);
          xce::distcache::DataPtr dataPtr(new Data(dataStr.begin(), dataStr.end()));
          return XceDistCacheRAdapter<T,P>::_distCacheAdapterPtr->set(data->id(), dataPtr);
        }

        bool set(long id,const P& data)
        {
          string dataStr;
          data->SerializeToString(&dataStr); 
          xce::distcache::DataPtr dataPtr(new Data(dataStr.begin(), dataStr.end()));
          return XceDistCacheRAdapter<T,P>::_distCacheAdapterPtr->set(id, dataPtr);
        }

        std::vector<long> set(const MyUtil::ObjectResultPtr& data)
        {
          map<long, xce::distcache::DataPtr> res;
          for(MyUtil::ObjectMap::const_iterator it = data->data.begin(); it != data->data.end(); ++it)
          {
            P tData = P::dynamicCast(it->second);
            string dataStr;
            tData->SerializeToString(&dataStr);
            xce::distcache::DataPtr dataPtr(new Data(dataStr.begin(), dataStr.end()));
            res.insert(make_pair(it->first, dataPtr));
          }
          return XceDistCacheRAdapter<T,P>::_distCacheAdapterPtr->set(res);
        }
    };
  }
}
#endif
