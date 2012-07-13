// **********************************************************************
//
// Copyright (c) 2005-2010 Renren, Inc. All rights reserved.
//
// Created on : 2010-01-04
// 	Author: zhanghan
// 	Modify: ServiceI去除了TaskManager、XceStorm、DbCxxPool、ObjectCache的精简版
// 		专门针对XceLogger服务, 去除对中间层代码/库的依赖而进行精简
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// 
//
// **********************************************************************

#ifndef __SERVICE_I_H__
#define __SERVICE_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceBox/IceBox.h>
//#include "IceLogger.h"
#include "Singleton.h"

#ifndef SERVICE_I_API
#   define SERVICE_I_API ICE_DECLSPEC_EXPORT
#endif

extern "C" {
SERVICE_I_API::IceBox::Service* create(::Ice::CommunicatorPtr communicator);
}

namespace MyUtil {

void initialize();
/*
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
*/
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
/*
	const std::string& getBusiness() const {
		return _business;
	}
	Ice::LoggerPtr& getLogger() {
		return _logger;
	}
	template<class Facet, class Servant> void addFacetMapping() {
		_ic->addObjectFactory(new ObjectFactoryI<Servant>,
				Facet::ice_staticId());
	}
*/

	template<class Name, class Category> Ice::Identity createIdentity(
			const Name& name, const Category& category) {
		ostringstream str;
		str << category << "/" << name;
		return _ic->stringToIdentity(str.str());
	}

	//void configure(const Ice::PropertiesPtr& props);

protected:
	virtual void start(const ::std::string&, const Ice::CommunicatorPtr&,
			const Ice::StringSeq&);

	virtual void stop();

private:
	std::string _name;
	//std::string _business;
	Ice::StringSeq _args;

	Ice::CommunicatorPtr _ic;
	Ice::ObjectAdapterPtr _adapter;

	//Ice::LoggerPtr _logger;
};

};

/*
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
*/
#endif
