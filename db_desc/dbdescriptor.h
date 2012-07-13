// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1
// Generated from file `dbdescriptor.ice'

#ifndef __dbdesc_dbdescriptor_h__
#define __dbdesc_dbdescriptor_h__

#include <Ice/LocalObjectF.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/Outgoing.h>
#include <Ice/Incoming.h>
#include <Ice/Direct.h>
#include <Ice/UserExceptionFactory.h>
#include <Ice/FactoryTable.h>
#include <Ice/StreamF.h>
#include <Ice/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 303
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 1
#       error Ice patch level mismatch!
#   endif
#endif

namespace IceProxy
{

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer;

class Route;

class DbInstance;

class DbObserver;

class DbDescriptor;

}

}

}

}

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer;
bool operator==(const DbServer&, const DbServer&);
bool operator<(const DbServer&, const DbServer&);

class Route;
bool operator==(const Route&, const Route&);
bool operator<(const Route&, const Route&);

class DbInstance;
bool operator==(const DbInstance&, const DbInstance&);
bool operator<(const DbInstance&, const DbInstance&);

class DbObserver;
bool operator==(const DbObserver&, const DbObserver&);
bool operator<(const DbObserver&, const DbObserver&);

class DbDescriptor;
bool operator==(const DbDescriptor&, const DbDescriptor&);
bool operator<(const DbDescriptor&, const DbDescriptor&);

}

}

}

namespace IceInternal
{

::Ice::Object* upCast(::com::xiaonei::xce::DbServer*);
::IceProxy::Ice::Object* upCast(::IceProxy::com::xiaonei::xce::DbServer*);

::Ice::Object* upCast(::com::xiaonei::xce::Route*);
::IceProxy::Ice::Object* upCast(::IceProxy::com::xiaonei::xce::Route*);

::Ice::Object* upCast(::com::xiaonei::xce::DbInstance*);
::IceProxy::Ice::Object* upCast(::IceProxy::com::xiaonei::xce::DbInstance*);

::Ice::Object* upCast(::com::xiaonei::xce::DbObserver*);
::IceProxy::Ice::Object* upCast(::IceProxy::com::xiaonei::xce::DbObserver*);

::Ice::Object* upCast(::com::xiaonei::xce::DbDescriptor*);
::IceProxy::Ice::Object* upCast(::IceProxy::com::xiaonei::xce::DbDescriptor*);

}

namespace com
{

namespace xiaonei
{

namespace xce
{

typedef ::IceInternal::Handle< ::com::xiaonei::xce::DbServer> DbServerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::com::xiaonei::xce::DbServer> DbServerPrx;

void __read(::IceInternal::BasicStream*, DbServerPrx&);
void __patch__DbServerPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::com::xiaonei::xce::Route> RoutePtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::com::xiaonei::xce::Route> RoutePrx;

void __read(::IceInternal::BasicStream*, RoutePrx&);
void __patch__RoutePtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::com::xiaonei::xce::DbInstance> DbInstancePtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::com::xiaonei::xce::DbInstance> DbInstancePrx;

void __read(::IceInternal::BasicStream*, DbInstancePrx&);
void __patch__DbInstancePtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::com::xiaonei::xce::DbObserver> DbObserverPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::com::xiaonei::xce::DbObserver> DbObserverPrx;

void __read(::IceInternal::BasicStream*, DbObserverPrx&);
void __patch__DbObserverPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::com::xiaonei::xce::DbDescriptor> DbDescriptorPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::com::xiaonei::xce::DbDescriptor> DbDescriptorPrx;

void __read(::IceInternal::BasicStream*, DbDescriptorPrx&);
void __patch__DbDescriptorPtr(void*, ::Ice::ObjectPtr&);

}

}

}

namespace com
{

namespace xiaonei
{

namespace xce
{

const ::Ice::Int CDbRServer = 1;

const ::Ice::Int CDbWServer = 2;

const ::Ice::Int CDbWRServer = 3;

const ::std::string CDbInstanceSingler = "singler";

const ::std::string CDbInstanceRouter = "router";

class XceException : public ::Ice::UserException
{
public:

    XceException() {}
    explicit XceException(const ::std::string&);
    virtual ~XceException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    ::std::string reason;

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

static XceException __XceException_init;

class NoSuchConnectionPoolException : public ::com::xiaonei::xce::XceException
{
public:

    NoSuchConnectionPoolException() {}
    explicit NoSuchConnectionPoolException(const ::std::string&);
    virtual ~NoSuchConnectionPoolException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class NoSuchConnectionException : public ::com::xiaonei::xce::XceException
{
public:

    NoSuchConnectionException() {}
    explicit NoSuchConnectionException(const ::std::string&);
    virtual ~NoSuchConnectionException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class ConnectionPoolFullException : public ::com::xiaonei::xce::XceException
{
public:

    ConnectionPoolFullException() {}
    explicit ConnectionPoolFullException(const ::std::string&);
    virtual ~ConnectionPoolFullException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class ConnectFailException : public ::com::xiaonei::xce::XceException
{
public:

    ConnectFailException() {}
    explicit ConnectFailException(const ::std::string&);
    virtual ~ConnectFailException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class QueryFailException : public ::com::xiaonei::xce::XceException
{
public:

    QueryFailException() {}
    explicit QueryFailException(const ::std::string&);
    virtual ~QueryFailException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

typedef ::std::vector< ::com::xiaonei::xce::DbServerPtr> DbServerSeq;
void __writeDbServerSeq(::IceInternal::BasicStream*, const ::com::xiaonei::xce::DbServerPtr*, const ::com::xiaonei::xce::DbServerPtr*);
void __readDbServerSeq(::IceInternal::BasicStream*, DbServerSeq&);

typedef ::std::vector< ::com::xiaonei::xce::RoutePtr> RouteSeq;
void __writeRouteSeq(::IceInternal::BasicStream*, const ::com::xiaonei::xce::RoutePtr*, const ::com::xiaonei::xce::RoutePtr*);
void __readRouteSeq(::IceInternal::BasicStream*, RouteSeq&);

typedef ::std::vector< ::com::xiaonei::xce::DbInstancePtr> DbInstanceSeq;
void __writeDbInstanceSeq(::IceInternal::BasicStream*, const ::com::xiaonei::xce::DbInstancePtr*, const ::com::xiaonei::xce::DbInstancePtr*);
void __readDbInstanceSeq(::IceInternal::BasicStream*, DbInstanceSeq&);

typedef ::std::map< ::std::string, ::com::xiaonei::xce::DbInstancePtr> DbInstanceMap;
void __writeDbInstanceMap(::IceInternal::BasicStream*, const DbInstanceMap&);
void __readDbInstanceMap(::IceInternal::BasicStream*, DbInstanceMap&);

typedef ::std::vector< ::com::xiaonei::xce::DbObserverPrx> DbObserverSeq;
void __writeDbObserverSeq(::IceInternal::BasicStream*, const ::com::xiaonei::xce::DbObserverPrx*, const ::com::xiaonei::xce::DbObserverPrx*);
void __readDbObserverSeq(::IceInternal::BasicStream*, DbObserverSeq&);

typedef ::std::map< ::std::string, ::com::xiaonei::xce::DbObserverSeq> DbObserverMap;
void __writeDbObserverMap(::IceInternal::BasicStream*, const DbObserverMap&);
void __readDbObserverMap(::IceInternal::BasicStream*, DbObserverMap&);

}

}

}

namespace IceProxy
{

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer : virtual public ::IceProxy::Ice::Object
{
public:
    
    ::IceInternal::ProxyHandle<DbServer> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbServer> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbServer*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<DbServer*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class Route : virtual public ::IceProxy::Ice::Object
{
public:
    
    ::IceInternal::ProxyHandle<Route> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Route> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Route*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<Route*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class DbInstance : virtual public ::IceProxy::Ice::Object
{
public:
    
    ::IceInternal::ProxyHandle<DbInstance> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbInstance> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbInstance*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<DbInstance*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class DbObserver : virtual public ::IceProxy::Ice::Object
{
public:

    void updated(const ::com::xiaonei::xce::DbInstancePtr& instance)
    {
        updated(instance, 0);
    }
    void updated(const ::com::xiaonei::xce::DbInstancePtr& instance, const ::Ice::Context& __ctx)
    {
        updated(instance, &__ctx);
    }
    
private:

    void updated(const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<DbObserver> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbObserver> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbObserver*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<DbObserver*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class DbDescriptor : virtual public ::IceProxy::Ice::Object
{
public:

    void reload()
    {
        reload(0);
    }
    void reload(const ::Ice::Context& __ctx)
    {
        reload(&__ctx);
    }
    
private:

    void reload(const ::Ice::Context*);
    
public:

    ::com::xiaonei::xce::DbInstanceSeq getDbInstances()
    {
        return getDbInstances(0);
    }
    ::com::xiaonei::xce::DbInstanceSeq getDbInstances(const ::Ice::Context& __ctx)
    {
        return getDbInstances(&__ctx);
    }
    
private:

    ::com::xiaonei::xce::DbInstanceSeq getDbInstances(const ::Ice::Context*);
    
public:

    ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string& name)
    {
        return getDbInstance(name, 0);
    }
    ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string& name, const ::Ice::Context& __ctx)
    {
        return getDbInstance(name, &__ctx);
    }
    
private:

    ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string&, const ::Ice::Context*);
    
public:

    void subscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer)
    {
        subscribe(name, observer, 0);
    }
    void subscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context& __ctx)
    {
        subscribe(name, observer, &__ctx);
    }
    
private:

    void subscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*);
    
public:

    void unsubscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer)
    {
        unsubscribe(name, observer, 0);
    }
    void unsubscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context& __ctx)
    {
        unsubscribe(name, observer, &__ctx);
    }
    
private:

    void unsubscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*);
    
public:

    ::com::xiaonei::xce::DbObserverMap query(const ::std::string& name)
    {
        return query(name, 0);
    }
    ::com::xiaonei::xce::DbObserverMap query(const ::std::string& name, const ::Ice::Context& __ctx)
    {
        return query(name, &__ctx);
    }
    
private:

    ::com::xiaonei::xce::DbObserverMap query(const ::std::string&, const ::Ice::Context*);
    
public:

    bool update(const ::std::string& name, const ::std::string& action, const ::com::xiaonei::xce::DbInstancePtr& db)
    {
        return update(name, action, db, 0);
    }
    bool update(const ::std::string& name, const ::std::string& action, const ::com::xiaonei::xce::DbInstancePtr& db, const ::Ice::Context& __ctx)
    {
        return update(name, action, db, &__ctx);
    }
    
private:

    bool update(const ::std::string&, const ::std::string&, const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<DbDescriptor> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<DbDescriptor*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<DbDescriptor*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

}

}

}

}

namespace IceDelegate
{

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer : virtual public ::IceDelegate::Ice::Object
{
public:
};

class Route : virtual public ::IceDelegate::Ice::Object
{
public:
};

class DbInstance : virtual public ::IceDelegate::Ice::Object
{
public:
};

class DbObserver : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void updated(const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*) = 0;
};

class DbDescriptor : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void reload(const ::Ice::Context*) = 0;

    virtual ::com::xiaonei::xce::DbInstanceSeq getDbInstances(const ::Ice::Context*) = 0;

    virtual ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string&, const ::Ice::Context*) = 0;

    virtual void subscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*) = 0;

    virtual void unsubscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*) = 0;

    virtual ::com::xiaonei::xce::DbObserverMap query(const ::std::string&, const ::Ice::Context*) = 0;

    virtual bool update(const ::std::string&, const ::std::string&, const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*) = 0;
};

}

}

}

}

namespace IceDelegateM
{

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer : virtual public ::IceDelegate::com::xiaonei::xce::DbServer,
                 virtual public ::IceDelegateM::Ice::Object
{
public:
};

class Route : virtual public ::IceDelegate::com::xiaonei::xce::Route,
              virtual public ::IceDelegateM::Ice::Object
{
public:
};

class DbInstance : virtual public ::IceDelegate::com::xiaonei::xce::DbInstance,
                   virtual public ::IceDelegateM::Ice::Object
{
public:
};

class DbObserver : virtual public ::IceDelegate::com::xiaonei::xce::DbObserver,
                   virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void updated(const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*);
};

class DbDescriptor : virtual public ::IceDelegate::com::xiaonei::xce::DbDescriptor,
                     virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void reload(const ::Ice::Context*);

    virtual ::com::xiaonei::xce::DbInstanceSeq getDbInstances(const ::Ice::Context*);

    virtual ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string&, const ::Ice::Context*);

    virtual void subscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*);

    virtual void unsubscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*);

    virtual ::com::xiaonei::xce::DbObserverMap query(const ::std::string&, const ::Ice::Context*);

    virtual bool update(const ::std::string&, const ::std::string&, const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*);
};

}

}

}

}

namespace IceDelegateD
{

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer : virtual public ::IceDelegate::com::xiaonei::xce::DbServer,
                 virtual public ::IceDelegateD::Ice::Object
{
public:
};

class Route : virtual public ::IceDelegate::com::xiaonei::xce::Route,
              virtual public ::IceDelegateD::Ice::Object
{
public:
};

class DbInstance : virtual public ::IceDelegate::com::xiaonei::xce::DbInstance,
                   virtual public ::IceDelegateD::Ice::Object
{
public:
};

class DbObserver : virtual public ::IceDelegate::com::xiaonei::xce::DbObserver,
                   virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void updated(const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*);
};

class DbDescriptor : virtual public ::IceDelegate::com::xiaonei::xce::DbDescriptor,
                     virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void reload(const ::Ice::Context*);

    virtual ::com::xiaonei::xce::DbInstanceSeq getDbInstances(const ::Ice::Context*);

    virtual ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string&, const ::Ice::Context*);

    virtual void subscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*);

    virtual void unsubscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Context*);

    virtual ::com::xiaonei::xce::DbObserverMap query(const ::std::string&, const ::Ice::Context*);

    virtual bool update(const ::std::string&, const ::std::string&, const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Context*);
};

}

}

}

}

namespace com
{

namespace xiaonei
{

namespace xce
{

class DbServer : virtual public ::Ice::Object
{
public:

    typedef DbServerPrx ProxyType;
    typedef DbServerPtr PointerType;
    
    DbServer() {}
    DbServer(const ::std::string&, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::std::string&, const ::std::string&);
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();


    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);

    static const ::Ice::ObjectFactoryPtr& ice_factory();

protected:

    virtual ~DbServer() {}

    friend class DbServer__staticInit;

public:

    ::std::string type;

    ::std::string database;

    ::std::string host;

    ::Ice::Int port;

    ::std::string charset;

    ::std::string user;

    ::std::string password;
};

class DbServer__staticInit
{
public:

    ::com::xiaonei::xce::DbServer _init;
};

static DbServer__staticInit _DbServer_init;

class Route : virtual public ::Ice::Object
{
public:

    typedef RoutePrx ProxyType;
    typedef RoutePtr PointerType;
    
    Route() {}
    Route(const ::std::string&, const ::std::string&);
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();


    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);

    static const ::Ice::ObjectFactoryPtr& ice_factory();

protected:

    virtual ~Route() {}

public:

    ::std::string expression;

    ::std::string instance;
};

class DbInstance : virtual public ::Ice::Object
{
public:

    typedef DbInstancePrx ProxyType;
    typedef DbInstancePtr PointerType;
    
    DbInstance() {}
    DbInstance(const ::std::string&, ::Ice::Int, const ::std::string&, const ::com::xiaonei::xce::DbServerPtr&, const ::com::xiaonei::xce::DbServerSeq&, const ::com::xiaonei::xce::RouteSeq&);
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void __incRef();
    virtual void __decRef();
    virtual void __addObject(::IceInternal::GCCountMap&);
    virtual bool __usesClasses();
    virtual void __gcReachable(::IceInternal::GCCountMap&) const;
    virtual void __gcClear();

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);

    static const ::Ice::ObjectFactoryPtr& ice_factory();

protected:

    virtual ~DbInstance() {}

public:

    ::std::string name;

    ::Ice::Int timestamp;

    ::std::string type;

    ::com::xiaonei::xce::DbServerPtr wserver;

    ::com::xiaonei::xce::DbServerSeq rservers;

    ::com::xiaonei::xce::RouteSeq routes;
};

class DbObserver : virtual public ::Ice::Object
{
public:

    typedef DbObserverPrx ProxyType;
    typedef DbObserverPtr PointerType;
    
    DbObserver() {}
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void updated(const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___updated(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class DbDescriptor : virtual public ::Ice::Object
{
public:

    typedef DbDescriptorPrx ProxyType;
    typedef DbDescriptorPtr PointerType;
    
    DbDescriptor() {}
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void reload(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___reload(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::com::xiaonei::xce::DbInstanceSeq getDbInstances(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getDbInstances(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::com::xiaonei::xce::DbInstancePtr getDbInstance(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getDbInstance(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void subscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___subscribe(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void unsubscribe(const ::std::string&, const ::com::xiaonei::xce::DbObserverPrx&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___unsubscribe(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::com::xiaonei::xce::DbObserverMap query(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___query(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual bool update(const ::std::string&, const ::std::string&, const ::com::xiaonei::xce::DbInstancePtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___update(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

}

}

}

#endif
