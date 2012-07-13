// **********************************************************************
//
// Copyright (c) 2005-2006 DuDu, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef __SERVICE_I_H__
#define __SERVICE_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceBox/IceBox.h>
#include "IceLogger.h"
#include "ObjectCacheI.h"
#include "ConnectionHolder.h"
#include "XceStorm.h"
#include "TaskManagerAPII.h"

#ifndef SERVICE_I_API
#   define SERVICE_I_API ICE_DECLSPEC_EXPORT
#endif

extern "C" {
SERVICE_I_API::IceBox::Service* create(::Ice::CommunicatorPtr communicator);
}

namespace MyUtil {

void initialize();

template<class T> class ObjectFactoryI : virtual public Ice::ObjectFactory {
public:
	virtual Ice::ObjectPtr create(const std::string& type) {
		Ice::ObjectPtr o = new T;
		if (!o) {
			throw Ice::MemoryLimitException(__FILE__, __LINE__);
		}
		return o;
	}
	virtual void destroy() {
	}
	;
};

class Descriptor;
typedef IceUtil::Handle<Descriptor> DescriptorPtr;
//---------------------------------------------------------------------------
class SERVICE_I_API ServiceI : public IceBox::Service, public Singleton<ServiceI> {
public:
	Ice::CommunicatorPtr& getCommunicator() {
		return _ic;
	}
	Ice::ObjectAdapterPtr& getAdapter() {
		return _adapter;
	}
	const std::string& getName() const {
		return _name;
	}
	const std::string& getBusiness() const {
		return _business;
	}
	Ice::LoggerPtr& getLogger() {
		return _logger;
	}

	ObjectCacheIPtr& getObjectCache() {
		return _cache;
	}

	template<class Facet, class Servant> void addFacetMapping() {
		_ic->addObjectFactory(new ObjectFactoryI<Servant>,
				Facet::ice_staticId());
	}

	template<class Name, class Category> Ice::Identity createIdentity(
			const Name& name, const Category& category) {
		ostringstream str;
		str << category << "/" << name;
		return _ic->stringToIdentity(str.str());
	}

	void addObject(const Ice::ObjectPtr& o, Ice::Int category, Ice::Long id) {
		_cache->addObject(category, id, o);
	}

	void removeObject(Ice::Int category, Ice::Long id) {
		_cache->removeObject(category, id);
	}

	bool hasObject(Ice::Int category, Ice::Long id) {
		return _cache->hasObject(category, id);
	}

	template<class T> T findObject(Ice::Int category, Ice::Long id) {
		return T::dynamicCast(_cache->findObject(category, id));
	}

	template<class T> T locateObject(Ice::Int category, Ice::Long id) {
		return T::dynamicCast(_cache->locateObject(category, id));
	}

	void reloadObject(Ice::Int category, Ice::Long id) {
		_cache->reloadObject(category, id);
	}

	void preloadObject(Ice::Int category, Ice::Long id) {
		_cache->preloadObject(category, id);
	}

	void registerObjectCache(Ice::Int category, const string& alias,
			const ServantFactoryPtr& factory, bool useEvictor=true);

	void registerXceObserver(const DescriptorPtr& o);

	void configure(const Ice::PropertiesPtr& props);

protected:
	virtual void start(const ::std::string&, const Ice::CommunicatorPtr&,
			const Ice::StringSeq&);

	virtual void stop();

private:
	std::string _name;
	std::string _business;
	Ice::StringSeq _args;

	Ice::CommunicatorPtr _ic;
	Ice::ObjectAdapterPtr _adapter;

	Ice::LoggerPtr _logger;

	ObjectCacheIPtr _cache;
	xce::svcapi::TaskManagerAPIIPtr _task_api;

	DescriptorPtr _descriptor;
};

class Descriptor: public xce::storm::Observer {
public:
	virtual bool update(const Str2StrMap& context, const Ice::Current& =
			Ice::Current());
protected:
	virtual void configure(const Ice::PropertiesPtr& props) {;}

	string md5sum(const Str2StrMap& context);
	bool changed(const Str2StrMap& context);

	Str2StrMap _context;
};

class DescriptorLoader : public Timer {
public:
	DescriptorLoader(const DescriptorPtr& descriptor) :
	Timer(5*1000), _descriptor(descriptor) {;}
	virtual void handle();
private:
	DescriptorPtr _descriptor;
};
}
;

#define XCE_INFO(msg) \
    do { \
	std::ostringstream __oos; \
	__oos << msg; \
        MyUtil::ServiceI::instance().getLogger()->print(__oos.str()); \
    } while(0);

#define XCE_DEBUG(msg) \
    do { \
	std::ostringstream __oos; \
	__oos << msg; \
        MyUtil::ServiceI::instance().getLogger()->trace("DEBUG", __oos.str()); \
    } while(0);

#define XCE_TRACE(msg) \
    do { \
	std::ostringstream __oos; \
	__oos << msg; \
        MyUtil::ServiceI::instance().getLogger()->trace("TRACE", __oos.str()); \
    } while(0);

#define XCE_WARN(msg) \
    do { \
	std::ostringstream __oos; \
	__oos << msg; \
        MyUtil::ServiceI::instance().getLogger()->warning(__oos.str()); \
    } while(0);

#endif

