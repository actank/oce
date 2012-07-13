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

#include <dbdescriptor.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/BasicStream.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/ScopedArray.h>

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

static const ::std::string __com__xiaonei__xce__DbObserver__updated_name = "updated";

static const ::std::string __com__xiaonei__xce__DbDescriptor__reload_name = "reload";

static const ::std::string __com__xiaonei__xce__DbDescriptor__getDbInstances_name = "getDbInstances";

static const ::std::string __com__xiaonei__xce__DbDescriptor__getDbInstance_name = "getDbInstance";

static const ::std::string __com__xiaonei__xce__DbDescriptor__subscribe_name = "subscribe";

static const ::std::string __com__xiaonei__xce__DbDescriptor__unsubscribe_name = "unsubscribe";

static const ::std::string __com__xiaonei__xce__DbDescriptor__query_name = "query";

static const ::std::string __com__xiaonei__xce__DbDescriptor__update_name = "update";

::Ice::Object* IceInternal::upCast(::com::xiaonei::xce::DbServer* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::com::xiaonei::xce::DbServer* p) { return p; }

::Ice::Object* IceInternal::upCast(::com::xiaonei::xce::Route* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::com::xiaonei::xce::Route* p) { return p; }

::Ice::Object* IceInternal::upCast(::com::xiaonei::xce::DbInstance* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::com::xiaonei::xce::DbInstance* p) { return p; }

::Ice::Object* IceInternal::upCast(::com::xiaonei::xce::DbObserver* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::com::xiaonei::xce::DbObserver* p) { return p; }

::Ice::Object* IceInternal::upCast(::com::xiaonei::xce::DbDescriptor* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::com::xiaonei::xce::DbDescriptor* p) { return p; }

void
com::xiaonei::xce::__read(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbServerPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::com::xiaonei::xce::DbServer;
        v->__copyFrom(proxy);
    }
}

void
com::xiaonei::xce::__read(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::RoutePrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::com::xiaonei::xce::Route;
        v->__copyFrom(proxy);
    }
}

void
com::xiaonei::xce::__read(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbInstancePrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::com::xiaonei::xce::DbInstance;
        v->__copyFrom(proxy);
    }
}

void
com::xiaonei::xce::__read(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbObserverPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::com::xiaonei::xce::DbObserver;
        v->__copyFrom(proxy);
    }
}

void
com::xiaonei::xce::__read(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbDescriptorPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::com::xiaonei::xce::DbDescriptor;
        v->__copyFrom(proxy);
    }
}

com::xiaonei::xce::XceException::XceException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    UserException(),
#else
    ::Ice::UserException(),
#endif
    reason(__ice_reason)
{
}

com::xiaonei::xce::XceException::~XceException() throw()
{
}

static const char* __com__xiaonei__xce__XceException_name = "com::xiaonei::xce::XceException";

::std::string
com::xiaonei::xce::XceException::ice_name() const
{
    return __com__xiaonei__xce__XceException_name;
}

::Ice::Exception*
com::xiaonei::xce::XceException::ice_clone() const
{
    return new XceException(*this);
}

void
com::xiaonei::xce::XceException::ice_throw() const
{
    throw *this;
}

void
com::xiaonei::xce::XceException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::com::xiaonei::xce::XceException"), false);
    __os->startWriteSlice();
    __os->write(reason);
    __os->endWriteSlice();
}

void
com::xiaonei::xce::XceException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->read(reason);
    __is->endReadSlice();
}

void
com::xiaonei::xce::XceException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::XceException was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::XceException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::XceException was not generated with stream support";
    throw ex;
}

struct __F__com__xiaonei__xce__XceException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::com::xiaonei::xce::XceException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__com__xiaonei__xce__XceException__Ptr = new __F__com__xiaonei__xce__XceException;

const ::IceInternal::UserExceptionFactoryPtr&
com::xiaonei::xce::XceException::ice_factory()
{
    return __F__com__xiaonei__xce__XceException__Ptr;
}

class __F__com__xiaonei__xce__XceException__Init
{
public:

    __F__com__xiaonei__xce__XceException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::com::xiaonei::xce::XceException", ::com::xiaonei::xce::XceException::ice_factory());
    }

    ~__F__com__xiaonei__xce__XceException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::com::xiaonei::xce::XceException");
    }
};

static __F__com__xiaonei__xce__XceException__Init __F__com__xiaonei__xce__XceException__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__XceException__initializer() {} }
#endif

com::xiaonei::xce::NoSuchConnectionPoolException::NoSuchConnectionPoolException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException(__ice_reason)
#else
    ::com::xiaonei::xce::XceException(__ice_reason)
#endif
{
}

com::xiaonei::xce::NoSuchConnectionPoolException::~NoSuchConnectionPoolException() throw()
{
}

static const char* __com__xiaonei__xce__NoSuchConnectionPoolException_name = "com::xiaonei::xce::NoSuchConnectionPoolException";

::std::string
com::xiaonei::xce::NoSuchConnectionPoolException::ice_name() const
{
    return __com__xiaonei__xce__NoSuchConnectionPoolException_name;
}

::Ice::Exception*
com::xiaonei::xce::NoSuchConnectionPoolException::ice_clone() const
{
    return new NoSuchConnectionPoolException(*this);
}

void
com::xiaonei::xce::NoSuchConnectionPoolException::ice_throw() const
{
    throw *this;
}

void
com::xiaonei::xce::NoSuchConnectionPoolException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::com::xiaonei::xce::NoSuchConnectionPoolException"), false);
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__write(__os);
#else
    ::com::xiaonei::xce::XceException::__write(__os);
#endif
}

void
com::xiaonei::xce::NoSuchConnectionPoolException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__read(__is, true);
#else
    ::com::xiaonei::xce::XceException::__read(__is, true);
#endif
}

void
com::xiaonei::xce::NoSuchConnectionPoolException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::NoSuchConnectionPoolException was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::NoSuchConnectionPoolException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::NoSuchConnectionPoolException was not generated with stream support";
    throw ex;
}

struct __F__com__xiaonei__xce__NoSuchConnectionPoolException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::com::xiaonei::xce::NoSuchConnectionPoolException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__com__xiaonei__xce__NoSuchConnectionPoolException__Ptr = new __F__com__xiaonei__xce__NoSuchConnectionPoolException;

const ::IceInternal::UserExceptionFactoryPtr&
com::xiaonei::xce::NoSuchConnectionPoolException::ice_factory()
{
    return __F__com__xiaonei__xce__NoSuchConnectionPoolException__Ptr;
}

class __F__com__xiaonei__xce__NoSuchConnectionPoolException__Init
{
public:

    __F__com__xiaonei__xce__NoSuchConnectionPoolException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::com::xiaonei::xce::NoSuchConnectionPoolException", ::com::xiaonei::xce::NoSuchConnectionPoolException::ice_factory());
    }

    ~__F__com__xiaonei__xce__NoSuchConnectionPoolException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::com::xiaonei::xce::NoSuchConnectionPoolException");
    }
};

static __F__com__xiaonei__xce__NoSuchConnectionPoolException__Init __F__com__xiaonei__xce__NoSuchConnectionPoolException__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__NoSuchConnectionPoolException__initializer() {} }
#endif

com::xiaonei::xce::NoSuchConnectionException::NoSuchConnectionException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException(__ice_reason)
#else
    ::com::xiaonei::xce::XceException(__ice_reason)
#endif
{
}

com::xiaonei::xce::NoSuchConnectionException::~NoSuchConnectionException() throw()
{
}

static const char* __com__xiaonei__xce__NoSuchConnectionException_name = "com::xiaonei::xce::NoSuchConnectionException";

::std::string
com::xiaonei::xce::NoSuchConnectionException::ice_name() const
{
    return __com__xiaonei__xce__NoSuchConnectionException_name;
}

::Ice::Exception*
com::xiaonei::xce::NoSuchConnectionException::ice_clone() const
{
    return new NoSuchConnectionException(*this);
}

void
com::xiaonei::xce::NoSuchConnectionException::ice_throw() const
{
    throw *this;
}

void
com::xiaonei::xce::NoSuchConnectionException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::com::xiaonei::xce::NoSuchConnectionException"), false);
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__write(__os);
#else
    ::com::xiaonei::xce::XceException::__write(__os);
#endif
}

void
com::xiaonei::xce::NoSuchConnectionException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__read(__is, true);
#else
    ::com::xiaonei::xce::XceException::__read(__is, true);
#endif
}

void
com::xiaonei::xce::NoSuchConnectionException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::NoSuchConnectionException was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::NoSuchConnectionException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::NoSuchConnectionException was not generated with stream support";
    throw ex;
}

struct __F__com__xiaonei__xce__NoSuchConnectionException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::com::xiaonei::xce::NoSuchConnectionException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__com__xiaonei__xce__NoSuchConnectionException__Ptr = new __F__com__xiaonei__xce__NoSuchConnectionException;

const ::IceInternal::UserExceptionFactoryPtr&
com::xiaonei::xce::NoSuchConnectionException::ice_factory()
{
    return __F__com__xiaonei__xce__NoSuchConnectionException__Ptr;
}

class __F__com__xiaonei__xce__NoSuchConnectionException__Init
{
public:

    __F__com__xiaonei__xce__NoSuchConnectionException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::com::xiaonei::xce::NoSuchConnectionException", ::com::xiaonei::xce::NoSuchConnectionException::ice_factory());
    }

    ~__F__com__xiaonei__xce__NoSuchConnectionException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::com::xiaonei::xce::NoSuchConnectionException");
    }
};

static __F__com__xiaonei__xce__NoSuchConnectionException__Init __F__com__xiaonei__xce__NoSuchConnectionException__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__NoSuchConnectionException__initializer() {} }
#endif

com::xiaonei::xce::ConnectionPoolFullException::ConnectionPoolFullException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException(__ice_reason)
#else
    ::com::xiaonei::xce::XceException(__ice_reason)
#endif
{
}

com::xiaonei::xce::ConnectionPoolFullException::~ConnectionPoolFullException() throw()
{
}

static const char* __com__xiaonei__xce__ConnectionPoolFullException_name = "com::xiaonei::xce::ConnectionPoolFullException";

::std::string
com::xiaonei::xce::ConnectionPoolFullException::ice_name() const
{
    return __com__xiaonei__xce__ConnectionPoolFullException_name;
}

::Ice::Exception*
com::xiaonei::xce::ConnectionPoolFullException::ice_clone() const
{
    return new ConnectionPoolFullException(*this);
}

void
com::xiaonei::xce::ConnectionPoolFullException::ice_throw() const
{
    throw *this;
}

void
com::xiaonei::xce::ConnectionPoolFullException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::com::xiaonei::xce::ConnectionPoolFullException"), false);
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__write(__os);
#else
    ::com::xiaonei::xce::XceException::__write(__os);
#endif
}

void
com::xiaonei::xce::ConnectionPoolFullException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__read(__is, true);
#else
    ::com::xiaonei::xce::XceException::__read(__is, true);
#endif
}

void
com::xiaonei::xce::ConnectionPoolFullException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::ConnectionPoolFullException was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::ConnectionPoolFullException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::ConnectionPoolFullException was not generated with stream support";
    throw ex;
}

struct __F__com__xiaonei__xce__ConnectionPoolFullException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::com::xiaonei::xce::ConnectionPoolFullException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__com__xiaonei__xce__ConnectionPoolFullException__Ptr = new __F__com__xiaonei__xce__ConnectionPoolFullException;

const ::IceInternal::UserExceptionFactoryPtr&
com::xiaonei::xce::ConnectionPoolFullException::ice_factory()
{
    return __F__com__xiaonei__xce__ConnectionPoolFullException__Ptr;
}

class __F__com__xiaonei__xce__ConnectionPoolFullException__Init
{
public:

    __F__com__xiaonei__xce__ConnectionPoolFullException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::com::xiaonei::xce::ConnectionPoolFullException", ::com::xiaonei::xce::ConnectionPoolFullException::ice_factory());
    }

    ~__F__com__xiaonei__xce__ConnectionPoolFullException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::com::xiaonei::xce::ConnectionPoolFullException");
    }
};

static __F__com__xiaonei__xce__ConnectionPoolFullException__Init __F__com__xiaonei__xce__ConnectionPoolFullException__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__ConnectionPoolFullException__initializer() {} }
#endif

com::xiaonei::xce::ConnectFailException::ConnectFailException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException(__ice_reason)
#else
    ::com::xiaonei::xce::XceException(__ice_reason)
#endif
{
}

com::xiaonei::xce::ConnectFailException::~ConnectFailException() throw()
{
}

static const char* __com__xiaonei__xce__ConnectFailException_name = "com::xiaonei::xce::ConnectFailException";

::std::string
com::xiaonei::xce::ConnectFailException::ice_name() const
{
    return __com__xiaonei__xce__ConnectFailException_name;
}

::Ice::Exception*
com::xiaonei::xce::ConnectFailException::ice_clone() const
{
    return new ConnectFailException(*this);
}

void
com::xiaonei::xce::ConnectFailException::ice_throw() const
{
    throw *this;
}

void
com::xiaonei::xce::ConnectFailException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::com::xiaonei::xce::ConnectFailException"), false);
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__write(__os);
#else
    ::com::xiaonei::xce::XceException::__write(__os);
#endif
}

void
com::xiaonei::xce::ConnectFailException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__read(__is, true);
#else
    ::com::xiaonei::xce::XceException::__read(__is, true);
#endif
}

void
com::xiaonei::xce::ConnectFailException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::ConnectFailException was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::ConnectFailException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::ConnectFailException was not generated with stream support";
    throw ex;
}

struct __F__com__xiaonei__xce__ConnectFailException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::com::xiaonei::xce::ConnectFailException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__com__xiaonei__xce__ConnectFailException__Ptr = new __F__com__xiaonei__xce__ConnectFailException;

const ::IceInternal::UserExceptionFactoryPtr&
com::xiaonei::xce::ConnectFailException::ice_factory()
{
    return __F__com__xiaonei__xce__ConnectFailException__Ptr;
}

class __F__com__xiaonei__xce__ConnectFailException__Init
{
public:

    __F__com__xiaonei__xce__ConnectFailException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::com::xiaonei::xce::ConnectFailException", ::com::xiaonei::xce::ConnectFailException::ice_factory());
    }

    ~__F__com__xiaonei__xce__ConnectFailException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::com::xiaonei::xce::ConnectFailException");
    }
};

static __F__com__xiaonei__xce__ConnectFailException__Init __F__com__xiaonei__xce__ConnectFailException__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__ConnectFailException__initializer() {} }
#endif

com::xiaonei::xce::QueryFailException::QueryFailException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException(__ice_reason)
#else
    ::com::xiaonei::xce::XceException(__ice_reason)
#endif
{
}

com::xiaonei::xce::QueryFailException::~QueryFailException() throw()
{
}

static const char* __com__xiaonei__xce__QueryFailException_name = "com::xiaonei::xce::QueryFailException";

::std::string
com::xiaonei::xce::QueryFailException::ice_name() const
{
    return __com__xiaonei__xce__QueryFailException_name;
}

::Ice::Exception*
com::xiaonei::xce::QueryFailException::ice_clone() const
{
    return new QueryFailException(*this);
}

void
com::xiaonei::xce::QueryFailException::ice_throw() const
{
    throw *this;
}

void
com::xiaonei::xce::QueryFailException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::com::xiaonei::xce::QueryFailException"), false);
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__write(__os);
#else
    ::com::xiaonei::xce::XceException::__write(__os);
#endif
}

void
com::xiaonei::xce::QueryFailException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    XceException::__read(__is, true);
#else
    ::com::xiaonei::xce::XceException::__read(__is, true);
#endif
}

void
com::xiaonei::xce::QueryFailException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::QueryFailException was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::QueryFailException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception com::xiaonei::xce::QueryFailException was not generated with stream support";
    throw ex;
}

struct __F__com__xiaonei__xce__QueryFailException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::com::xiaonei::xce::QueryFailException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__com__xiaonei__xce__QueryFailException__Ptr = new __F__com__xiaonei__xce__QueryFailException;

const ::IceInternal::UserExceptionFactoryPtr&
com::xiaonei::xce::QueryFailException::ice_factory()
{
    return __F__com__xiaonei__xce__QueryFailException__Ptr;
}

class __F__com__xiaonei__xce__QueryFailException__Init
{
public:

    __F__com__xiaonei__xce__QueryFailException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::com::xiaonei::xce::QueryFailException", ::com::xiaonei::xce::QueryFailException::ice_factory());
    }

    ~__F__com__xiaonei__xce__QueryFailException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::com::xiaonei::xce::QueryFailException");
    }
};

static __F__com__xiaonei__xce__QueryFailException__Init __F__com__xiaonei__xce__QueryFailException__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__QueryFailException__initializer() {} }
#endif

void
com::xiaonei::xce::__writeDbServerSeq(::IceInternal::BasicStream* __os, const ::com::xiaonei::xce::DbServerPtr* begin, const ::com::xiaonei::xce::DbServerPtr* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        __os->write(::Ice::ObjectPtr(::IceInternal::upCast(begin[i].get())));
    }
}

void
com::xiaonei::xce::__readDbServerSeq(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbServerSeq& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 4);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        __is->read(::com::xiaonei::xce::__patch__DbServerPtr, &v[i]);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

void
com::xiaonei::xce::__writeRouteSeq(::IceInternal::BasicStream* __os, const ::com::xiaonei::xce::RoutePtr* begin, const ::com::xiaonei::xce::RoutePtr* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        __os->write(::Ice::ObjectPtr(::IceInternal::upCast(begin[i].get())));
    }
}

void
com::xiaonei::xce::__readRouteSeq(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::RouteSeq& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 4);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        __is->read(::com::xiaonei::xce::__patch__RoutePtr, &v[i]);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

void
com::xiaonei::xce::__writeDbInstanceSeq(::IceInternal::BasicStream* __os, const ::com::xiaonei::xce::DbInstancePtr* begin, const ::com::xiaonei::xce::DbInstancePtr* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        __os->write(::Ice::ObjectPtr(::IceInternal::upCast(begin[i].get())));
    }
}

void
com::xiaonei::xce::__readDbInstanceSeq(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbInstanceSeq& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 4);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        __is->read(::com::xiaonei::xce::__patch__DbInstancePtr, &v[i]);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

void
com::xiaonei::xce::__writeDbInstanceMap(::IceInternal::BasicStream* __os, const ::com::xiaonei::xce::DbInstanceMap& v)
{
    __os->writeSize(::Ice::Int(v.size()));
    ::com::xiaonei::xce::DbInstanceMap::const_iterator p;
    for(p = v.begin(); p != v.end(); ++p)
    {
        __os->write(p->first);
        __os->write(::Ice::ObjectPtr(::IceInternal::upCast(p->second.get())));
    }
}

void
com::xiaonei::xce::__readDbInstanceMap(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbInstanceMap& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    while(sz--)
    {
        ::std::pair<const  ::std::string, ::com::xiaonei::xce::DbInstancePtr> pair;
        __is->read(const_cast< ::std::string&>(pair.first));
        ::com::xiaonei::xce::DbInstanceMap::iterator __i = v.insert(v.end(), pair);
        __is->read(::com::xiaonei::xce::__patch__DbInstancePtr, &__i->second);
    }
}

void
com::xiaonei::xce::__writeDbObserverSeq(::IceInternal::BasicStream* __os, const ::com::xiaonei::xce::DbObserverPrx* begin, const ::com::xiaonei::xce::DbObserverPrx* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        __os->write(::Ice::ObjectPrx(::IceInternal::upCast(begin[i].get())));
    }
}

void
com::xiaonei::xce::__readDbObserverSeq(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbObserverSeq& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 2);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        ::com::xiaonei::xce::__read(__is, v[i]);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

void
com::xiaonei::xce::__writeDbObserverMap(::IceInternal::BasicStream* __os, const ::com::xiaonei::xce::DbObserverMap& v)
{
    __os->writeSize(::Ice::Int(v.size()));
    ::com::xiaonei::xce::DbObserverMap::const_iterator p;
    for(p = v.begin(); p != v.end(); ++p)
    {
        __os->write(p->first);
        if(p->second.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::com::xiaonei::xce::__writeDbObserverSeq(__os, &p->second[0], &p->second[0] + p->second.size());
        }
    }
}

void
com::xiaonei::xce::__readDbObserverMap(::IceInternal::BasicStream* __is, ::com::xiaonei::xce::DbObserverMap& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    while(sz--)
    {
        ::std::pair<const  ::std::string, ::com::xiaonei::xce::DbObserverSeq> pair;
        __is->read(const_cast< ::std::string&>(pair.first));
        ::com::xiaonei::xce::DbObserverMap::iterator __i = v.insert(v.end(), pair);
        ::com::xiaonei::xce::__readDbObserverSeq(__is, __i->second);
    }
}

const ::std::string&
IceProxy::com::xiaonei::xce::DbServer::ice_staticId()
{
    return ::com::xiaonei::xce::DbServer::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::com::xiaonei::xce::DbServer::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::com::xiaonei::xce::DbServer);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::com::xiaonei::xce::DbServer::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::com::xiaonei::xce::DbServer);
}

::IceProxy::Ice::Object*
IceProxy::com::xiaonei::xce::DbServer::__newInstance() const
{
    return new DbServer;
}

const ::std::string&
IceProxy::com::xiaonei::xce::Route::ice_staticId()
{
    return ::com::xiaonei::xce::Route::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::com::xiaonei::xce::Route::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::com::xiaonei::xce::Route);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::com::xiaonei::xce::Route::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::com::xiaonei::xce::Route);
}

::IceProxy::Ice::Object*
IceProxy::com::xiaonei::xce::Route::__newInstance() const
{
    return new Route;
}

const ::std::string&
IceProxy::com::xiaonei::xce::DbInstance::ice_staticId()
{
    return ::com::xiaonei::xce::DbInstance::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::com::xiaonei::xce::DbInstance::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::com::xiaonei::xce::DbInstance);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::com::xiaonei::xce::DbInstance::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::com::xiaonei::xce::DbInstance);
}

::IceProxy::Ice::Object*
IceProxy::com::xiaonei::xce::DbInstance::__newInstance() const
{
    return new DbInstance;
}

void
IceProxy::com::xiaonei::xce::DbObserver::updated(const ::com::xiaonei::xce::DbInstancePtr& instance, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbObserver* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbObserver*>(__delBase.get());
            __del->updated(instance, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

const ::std::string&
IceProxy::com::xiaonei::xce::DbObserver::ice_staticId()
{
    return ::com::xiaonei::xce::DbObserver::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::com::xiaonei::xce::DbObserver::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::com::xiaonei::xce::DbObserver);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::com::xiaonei::xce::DbObserver::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::com::xiaonei::xce::DbObserver);
}

::IceProxy::Ice::Object*
IceProxy::com::xiaonei::xce::DbObserver::__newInstance() const
{
    return new DbObserver;
}

void
IceProxy::com::xiaonei::xce::DbDescriptor::reload(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            __del->reload(__ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

::com::xiaonei::xce::DbInstanceSeq
IceProxy::com::xiaonei::xce::DbDescriptor::getDbInstances(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__com__xiaonei__xce__DbDescriptor__getDbInstances_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            return __del->getDbInstances(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

::com::xiaonei::xce::DbInstancePtr
IceProxy::com::xiaonei::xce::DbDescriptor::getDbInstance(const ::std::string& name, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__com__xiaonei__xce__DbDescriptor__getDbInstance_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            return __del->getDbInstance(name, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::com::xiaonei::xce::DbDescriptor::subscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            __del->subscribe(name, observer, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::com::xiaonei::xce::DbDescriptor::unsubscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            __del->unsubscribe(name, observer, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

::com::xiaonei::xce::DbObserverMap
IceProxy::com::xiaonei::xce::DbDescriptor::query(const ::std::string& name, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__com__xiaonei__xce__DbDescriptor__query_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            return __del->query(name, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::com::xiaonei::xce::DbDescriptor::update(const ::std::string& name, const ::std::string& action, const ::com::xiaonei::xce::DbInstancePtr& db, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__com__xiaonei__xce__DbDescriptor__update_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::com::xiaonei::xce::DbDescriptor* __del = dynamic_cast< ::IceDelegate::com::xiaonei::xce::DbDescriptor*>(__delBase.get());
            return __del->update(name, action, db, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

const ::std::string&
IceProxy::com::xiaonei::xce::DbDescriptor::ice_staticId()
{
    return ::com::xiaonei::xce::DbDescriptor::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::com::xiaonei::xce::DbDescriptor::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::com::xiaonei::xce::DbDescriptor);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::com::xiaonei::xce::DbDescriptor::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::com::xiaonei::xce::DbDescriptor);
}

::IceProxy::Ice::Object*
IceProxy::com::xiaonei::xce::DbDescriptor::__newInstance() const
{
    return new DbDescriptor;
}

void
IceDelegateM::com::xiaonei::xce::DbObserver::updated(const ::com::xiaonei::xce::DbInstancePtr& instance, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbObserver__updated_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(::Ice::ObjectPtr(::IceInternal::upCast(instance.get())));
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

void
IceDelegateM::com::xiaonei::xce::DbDescriptor::reload(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__reload_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

::com::xiaonei::xce::DbInstanceSeq
IceDelegateM::com::xiaonei::xce::DbDescriptor::getDbInstances(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__getDbInstances_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    ::com::xiaonei::xce::DbInstanceSeq __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        ::com::xiaonei::xce::__readDbInstanceSeq(__is, __ret);
        __is->readPendingObjects();
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::com::xiaonei::xce::DbInstancePtr
IceDelegateM::com::xiaonei::xce::DbDescriptor::getDbInstance(const ::std::string& name, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__getDbInstance_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(name);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::com::xiaonei::xce::DbInstancePtr __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __is->read(::com::xiaonei::xce::__patch__DbInstancePtr, &__ret);
        __is->readPendingObjects();
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::com::xiaonei::xce::DbDescriptor::subscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__subscribe_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(name);
        __os->write(::Ice::ObjectPrx(::IceInternal::upCast(observer.get())));
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

void
IceDelegateM::com::xiaonei::xce::DbDescriptor::unsubscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__unsubscribe_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(name);
        __os->write(::Ice::ObjectPrx(::IceInternal::upCast(observer.get())));
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

::com::xiaonei::xce::DbObserverMap
IceDelegateM::com::xiaonei::xce::DbDescriptor::query(const ::std::string& name, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__query_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(name);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::com::xiaonei::xce::DbObserverMap __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        ::com::xiaonei::xce::__readDbObserverMap(__is, __ret);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

bool
IceDelegateM::com::xiaonei::xce::DbDescriptor::update(const ::std::string& name, const ::std::string& action, const ::com::xiaonei::xce::DbInstancePtr& db, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __com__xiaonei__xce__DbDescriptor__update_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(name);
        __os->write(action);
        __os->write(::Ice::ObjectPtr(::IceInternal::upCast(db.get())));
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    bool __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __is->read(__ret);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateD::com::xiaonei::xce::DbObserver::updated(const ::com::xiaonei::xce::DbInstancePtr& instance, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::com::xiaonei::xce::DbInstancePtr& instance, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_instance(instance)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbObserver* servant = dynamic_cast< ::com::xiaonei::xce::DbObserver*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->updated(_m_instance, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        const ::com::xiaonei::xce::DbInstancePtr& _m_instance;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbObserver__updated_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(instance, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::com::xiaonei::xce::DbDescriptor::reload(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->reload(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__reload_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(__current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::com::xiaonei::xce::DbInstanceSeq
IceDelegateD::com::xiaonei::xce::DbDescriptor::getDbInstances(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::com::xiaonei::xce::DbInstanceSeq& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->getDbInstances(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::com::xiaonei::xce::DbInstanceSeq& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__getDbInstances_name, ::Ice::Normal, __context);
    ::com::xiaonei::xce::DbInstanceSeq __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::com::xiaonei::xce::DbInstancePtr
IceDelegateD::com::xiaonei::xce::DbDescriptor::getDbInstance(const ::std::string& name, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::com::xiaonei::xce::DbInstancePtr& __result, const ::std::string& name, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_name(name)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->getDbInstance(_m_name, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::com::xiaonei::xce::DbInstancePtr& _result;
        const ::std::string& _m_name;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__getDbInstance_name, ::Ice::Normal, __context);
    ::com::xiaonei::xce::DbInstancePtr __result;
    try
    {
        _DirectI __direct(__result, name, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

void
IceDelegateD::com::xiaonei::xce::DbDescriptor::subscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_name(name),
            _m_observer(observer)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->subscribe(_m_name, _m_observer, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        const ::std::string& _m_name;
        const ::com::xiaonei::xce::DbObserverPrx& _m_observer;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__subscribe_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(name, observer, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::com::xiaonei::xce::DbDescriptor::unsubscribe(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& name, const ::com::xiaonei::xce::DbObserverPrx& observer, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_name(name),
            _m_observer(observer)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->unsubscribe(_m_name, _m_observer, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        const ::std::string& _m_name;
        const ::com::xiaonei::xce::DbObserverPrx& _m_observer;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__unsubscribe_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(name, observer, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::com::xiaonei::xce::DbObserverMap
IceDelegateD::com::xiaonei::xce::DbDescriptor::query(const ::std::string& name, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::com::xiaonei::xce::DbObserverMap& __result, const ::std::string& name, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_name(name)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->query(_m_name, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::com::xiaonei::xce::DbObserverMap& _result;
        const ::std::string& _m_name;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__query_name, ::Ice::Normal, __context);
    ::com::xiaonei::xce::DbObserverMap __result;
    try
    {
        _DirectI __direct(__result, name, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

bool
IceDelegateD::com::xiaonei::xce::DbDescriptor::update(const ::std::string& name, const ::std::string& action, const ::com::xiaonei::xce::DbInstancePtr& db, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(bool& __result, const ::std::string& name, const ::std::string& action, const ::com::xiaonei::xce::DbInstancePtr& db, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_name(name),
            _m_action(action),
            _m_db(db)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::com::xiaonei::xce::DbDescriptor* servant = dynamic_cast< ::com::xiaonei::xce::DbDescriptor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->update(_m_name, _m_action, _m_db, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        bool& _result;
        const ::std::string& _m_name;
        const ::std::string& _m_action;
        const ::com::xiaonei::xce::DbInstancePtr& _m_db;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __com__xiaonei__xce__DbDescriptor__update_name, ::Ice::Normal, __context);
    bool __result;
    try
    {
        _DirectI __direct(__result, name, action, db, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

com::xiaonei::xce::DbServer::DbServer(const ::std::string& __ice_type, const ::std::string& __ice_database, const ::std::string& __ice_host, ::Ice::Int __ice_port, const ::std::string& __ice_charset, const ::std::string& __ice_user, const ::std::string& __ice_password) :
    type(__ice_type),
    database(__ice_database),
    host(__ice_host),
    port(__ice_port),
    charset(__ice_charset),
    user(__ice_user),
    password(__ice_password)
{
}

::Ice::ObjectPtr
com::xiaonei::xce::DbServer::ice_clone() const
{
    ::com::xiaonei::xce::DbServerPtr __p = new ::com::xiaonei::xce::DbServer(*this);
    return __p;
}

static const ::std::string __com__xiaonei__xce__DbServer_ids[2] =
{
    "::Ice::Object",
    "::com::xiaonei::xce::DbServer"
};

bool
com::xiaonei::xce::DbServer::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__com__xiaonei__xce__DbServer_ids, __com__xiaonei__xce__DbServer_ids + 2, _s);
}

::std::vector< ::std::string>
com::xiaonei::xce::DbServer::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__com__xiaonei__xce__DbServer_ids[0], &__com__xiaonei__xce__DbServer_ids[2]);
}

const ::std::string&
com::xiaonei::xce::DbServer::ice_id(const ::Ice::Current&) const
{
    return __com__xiaonei__xce__DbServer_ids[1];
}

const ::std::string&
com::xiaonei::xce::DbServer::ice_staticId()
{
    return __com__xiaonei__xce__DbServer_ids[1];
}

void
com::xiaonei::xce::DbServer::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->write(type);
    __os->write(database);
    __os->write(host);
    __os->write(port);
    __os->write(charset);
    __os->write(user);
    __os->write(password);
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
com::xiaonei::xce::DbServer::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->read(type);
    __is->read(database);
    __is->read(host);
    __is->read(port);
    __is->read(charset);
    __is->read(user);
    __is->read(password);
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
com::xiaonei::xce::DbServer::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbServer was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::DbServer::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbServer was not generated with stream support";
    throw ex;
}

class __F__com__xiaonei__xce__DbServer : public ::Ice::ObjectFactory
{
public:

    virtual ::Ice::ObjectPtr
    create(const ::std::string& type)
    {
        assert(type == ::com::xiaonei::xce::DbServer::ice_staticId());
        return new ::com::xiaonei::xce::DbServer;
    }

    virtual void
    destroy()
    {
    }
};

static ::Ice::ObjectFactoryPtr __F__com__xiaonei__xce__DbServer_Ptr = new __F__com__xiaonei__xce__DbServer;

const ::Ice::ObjectFactoryPtr&
com::xiaonei::xce::DbServer::ice_factory()
{
    return __F__com__xiaonei__xce__DbServer_Ptr;
}

class __F__com__xiaonei__xce__DbServer__Init
{
public:

    __F__com__xiaonei__xce__DbServer__Init()
    {
        ::IceInternal::factoryTable->addObjectFactory(::com::xiaonei::xce::DbServer::ice_staticId(), ::com::xiaonei::xce::DbServer::ice_factory());
    }

    ~__F__com__xiaonei__xce__DbServer__Init()
    {
        ::IceInternal::factoryTable->removeObjectFactory(::com::xiaonei::xce::DbServer::ice_staticId());
    }
};

static __F__com__xiaonei__xce__DbServer__Init __F__com__xiaonei__xce__DbServer__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__DbServer__initializer() {} }
#endif

void 
com::xiaonei::xce::__patch__DbServerPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::com::xiaonei::xce::DbServerPtr* p = static_cast< ::com::xiaonei::xce::DbServerPtr*>(__addr);
    assert(p);
    *p = ::com::xiaonei::xce::DbServerPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::com::xiaonei::xce::DbServer::ice_staticId(), v->ice_id());
    }
}

bool
com::xiaonei::xce::operator==(const ::com::xiaonei::xce::DbServer& l, const ::com::xiaonei::xce::DbServer& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
com::xiaonei::xce::operator<(const ::com::xiaonei::xce::DbServer& l, const ::com::xiaonei::xce::DbServer& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

com::xiaonei::xce::Route::Route(const ::std::string& __ice_expression, const ::std::string& __ice_instance) :
    expression(__ice_expression),
    instance(__ice_instance)
{
}

::Ice::ObjectPtr
com::xiaonei::xce::Route::ice_clone() const
{
    ::com::xiaonei::xce::RoutePtr __p = new ::com::xiaonei::xce::Route(*this);
    return __p;
}

static const ::std::string __com__xiaonei__xce__Route_ids[2] =
{
    "::Ice::Object",
    "::com::xiaonei::xce::Route"
};

bool
com::xiaonei::xce::Route::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__com__xiaonei__xce__Route_ids, __com__xiaonei__xce__Route_ids + 2, _s);
}

::std::vector< ::std::string>
com::xiaonei::xce::Route::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__com__xiaonei__xce__Route_ids[0], &__com__xiaonei__xce__Route_ids[2]);
}

const ::std::string&
com::xiaonei::xce::Route::ice_id(const ::Ice::Current&) const
{
    return __com__xiaonei__xce__Route_ids[1];
}

const ::std::string&
com::xiaonei::xce::Route::ice_staticId()
{
    return __com__xiaonei__xce__Route_ids[1];
}

void
com::xiaonei::xce::Route::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->write(expression);
    __os->write(instance);
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
com::xiaonei::xce::Route::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->read(expression);
    __is->read(instance);
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
com::xiaonei::xce::Route::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::Route was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::Route::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::Route was not generated with stream support";
    throw ex;
}

class __F__com__xiaonei__xce__Route : public ::Ice::ObjectFactory
{
public:

    virtual ::Ice::ObjectPtr
    create(const ::std::string& type)
    {
        assert(type == ::com::xiaonei::xce::Route::ice_staticId());
        return new ::com::xiaonei::xce::Route;
    }

    virtual void
    destroy()
    {
    }
};

static ::Ice::ObjectFactoryPtr __F__com__xiaonei__xce__Route_Ptr = new __F__com__xiaonei__xce__Route;

const ::Ice::ObjectFactoryPtr&
com::xiaonei::xce::Route::ice_factory()
{
    return __F__com__xiaonei__xce__Route_Ptr;
}

class __F__com__xiaonei__xce__Route__Init
{
public:

    __F__com__xiaonei__xce__Route__Init()
    {
        ::IceInternal::factoryTable->addObjectFactory(::com::xiaonei::xce::Route::ice_staticId(), ::com::xiaonei::xce::Route::ice_factory());
    }

    ~__F__com__xiaonei__xce__Route__Init()
    {
        ::IceInternal::factoryTable->removeObjectFactory(::com::xiaonei::xce::Route::ice_staticId());
    }
};

static __F__com__xiaonei__xce__Route__Init __F__com__xiaonei__xce__Route__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__Route__initializer() {} }
#endif

void 
com::xiaonei::xce::__patch__RoutePtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::com::xiaonei::xce::RoutePtr* p = static_cast< ::com::xiaonei::xce::RoutePtr*>(__addr);
    assert(p);
    *p = ::com::xiaonei::xce::RoutePtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::com::xiaonei::xce::Route::ice_staticId(), v->ice_id());
    }
}

bool
com::xiaonei::xce::operator==(const ::com::xiaonei::xce::Route& l, const ::com::xiaonei::xce::Route& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
com::xiaonei::xce::operator<(const ::com::xiaonei::xce::Route& l, const ::com::xiaonei::xce::Route& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

com::xiaonei::xce::DbInstance::DbInstance(const ::std::string& __ice_name, ::Ice::Int __ice_timestamp, const ::std::string& __ice_type, const ::com::xiaonei::xce::DbServerPtr& __ice_wserver, const ::com::xiaonei::xce::DbServerSeq& __ice_rservers, const ::com::xiaonei::xce::RouteSeq& __ice_routes) :
    name(__ice_name),
    timestamp(__ice_timestamp),
    type(__ice_type),
    wserver(__ice_wserver),
    rservers(__ice_rservers),
    routes(__ice_routes)
{
}

::Ice::ObjectPtr
com::xiaonei::xce::DbInstance::ice_clone() const
{
    ::com::xiaonei::xce::DbInstancePtr __p = new ::com::xiaonei::xce::DbInstance(*this);
    return __p;
}

static const ::std::string __com__xiaonei__xce__DbInstance_ids[2] =
{
    "::Ice::Object",
    "::com::xiaonei::xce::DbInstance"
};

bool
com::xiaonei::xce::DbInstance::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__com__xiaonei__xce__DbInstance_ids, __com__xiaonei__xce__DbInstance_ids + 2, _s);
}

::std::vector< ::std::string>
com::xiaonei::xce::DbInstance::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__com__xiaonei__xce__DbInstance_ids[0], &__com__xiaonei__xce__DbInstance_ids[2]);
}

const ::std::string&
com::xiaonei::xce::DbInstance::ice_id(const ::Ice::Current&) const
{
    return __com__xiaonei__xce__DbInstance_ids[1];
}

const ::std::string&
com::xiaonei::xce::DbInstance::ice_staticId()
{
    return __com__xiaonei__xce__DbInstance_ids[1];
}

void
com::xiaonei::xce::DbInstance::__incRef()
{
    __gcIncRef();
}

void
com::xiaonei::xce::DbInstance::__decRef()
{
    __gcDecRef();
}

void
com::xiaonei::xce::DbInstance::__addObject(::IceInternal::GCCountMap& _c)
{
    ::IceInternal::GCCountMap::iterator pos = _c.find(this);
    if(pos == _c.end())
    {
        _c[this] = 1;
    }
    else
    {
        ++pos->second;
    }
}

bool
com::xiaonei::xce::DbInstance::__usesClasses()
{
    return true;
}

void
com::xiaonei::xce::DbInstance::__gcReachable(::IceInternal::GCCountMap& _c) const
{
    if(wserver)
    {
        ::IceInternal::upCast(wserver.get())->__addObject(_c);
    }
    {
        for(::com::xiaonei::xce::DbServerSeq::const_iterator _i0 = rservers.begin(); _i0 != rservers.end(); ++_i0)
        {
            if((*_i0))
            {
                ::IceInternal::upCast((*_i0).get())->__addObject(_c);
            }
        }
    }
    {
        for(::com::xiaonei::xce::RouteSeq::const_iterator _i0 = routes.begin(); _i0 != routes.end(); ++_i0)
        {
            if((*_i0))
            {
                ::IceInternal::upCast((*_i0).get())->__addObject(_c);
            }
        }
    }
}

void
com::xiaonei::xce::DbInstance::__gcClear()
{
    if(wserver)
    {
        if(::IceInternal::upCast(wserver.get())->__usesClasses())
        {
            ::IceInternal::upCast(wserver.get())->__decRefUnsafe();
            wserver.__clearHandleUnsafe();
        }
        else
        {
            wserver = 0;
        }
    }
    {
        for(::com::xiaonei::xce::DbServerSeq::iterator _i0 = rservers.begin(); _i0 != rservers.end(); ++_i0)
        {
            if((*_i0))
            {
                if(::IceInternal::upCast((*_i0).get())->__usesClasses())
                {
                    ::IceInternal::upCast((*_i0).get())->__decRefUnsafe();
                    (*_i0).__clearHandleUnsafe();
                }
                else
                {
                    (*_i0) = 0;
                }
            }
        }
    }
    {
        for(::com::xiaonei::xce::RouteSeq::iterator _i0 = routes.begin(); _i0 != routes.end(); ++_i0)
        {
            if((*_i0))
            {
                if(::IceInternal::upCast((*_i0).get())->__usesClasses())
                {
                    ::IceInternal::upCast((*_i0).get())->__decRefUnsafe();
                    (*_i0).__clearHandleUnsafe();
                }
                else
                {
                    (*_i0) = 0;
                }
            }
        }
    }
}

void
com::xiaonei::xce::DbInstance::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->write(name);
    __os->write(timestamp);
    __os->write(type);
    __os->write(::Ice::ObjectPtr(::IceInternal::upCast(wserver.get())));
    if(rservers.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        ::com::xiaonei::xce::__writeDbServerSeq(__os, &rservers[0], &rservers[0] + rservers.size());
    }
    if(routes.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        ::com::xiaonei::xce::__writeRouteSeq(__os, &routes[0], &routes[0] + routes.size());
    }
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
com::xiaonei::xce::DbInstance::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->read(name);
    __is->read(timestamp);
    __is->read(type);
    __is->read(::com::xiaonei::xce::__patch__DbServerPtr, &wserver);
    ::com::xiaonei::xce::__readDbServerSeq(__is, rservers);
    ::com::xiaonei::xce::__readRouteSeq(__is, routes);
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
com::xiaonei::xce::DbInstance::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbInstance was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::DbInstance::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbInstance was not generated with stream support";
    throw ex;
}

class __F__com__xiaonei__xce__DbInstance : public ::Ice::ObjectFactory
{
public:

    virtual ::Ice::ObjectPtr
    create(const ::std::string& type)
    {
        assert(type == ::com::xiaonei::xce::DbInstance::ice_staticId());
        return new ::com::xiaonei::xce::DbInstance;
    }

    virtual void
    destroy()
    {
    }
};

static ::Ice::ObjectFactoryPtr __F__com__xiaonei__xce__DbInstance_Ptr = new __F__com__xiaonei__xce__DbInstance;

const ::Ice::ObjectFactoryPtr&
com::xiaonei::xce::DbInstance::ice_factory()
{
    return __F__com__xiaonei__xce__DbInstance_Ptr;
}

class __F__com__xiaonei__xce__DbInstance__Init
{
public:

    __F__com__xiaonei__xce__DbInstance__Init()
    {
        ::IceInternal::factoryTable->addObjectFactory(::com::xiaonei::xce::DbInstance::ice_staticId(), ::com::xiaonei::xce::DbInstance::ice_factory());
    }

    ~__F__com__xiaonei__xce__DbInstance__Init()
    {
        ::IceInternal::factoryTable->removeObjectFactory(::com::xiaonei::xce::DbInstance::ice_staticId());
    }
};

static __F__com__xiaonei__xce__DbInstance__Init __F__com__xiaonei__xce__DbInstance__i;

#ifdef __APPLE__
extern "C" { void __F__com__xiaonei__xce__DbInstance__initializer() {} }
#endif

void 
com::xiaonei::xce::__patch__DbInstancePtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::com::xiaonei::xce::DbInstancePtr* p = static_cast< ::com::xiaonei::xce::DbInstancePtr*>(__addr);
    assert(p);
    *p = ::com::xiaonei::xce::DbInstancePtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::com::xiaonei::xce::DbInstance::ice_staticId(), v->ice_id());
    }
}

bool
com::xiaonei::xce::operator==(const ::com::xiaonei::xce::DbInstance& l, const ::com::xiaonei::xce::DbInstance& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
com::xiaonei::xce::operator<(const ::com::xiaonei::xce::DbInstance& l, const ::com::xiaonei::xce::DbInstance& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

::Ice::ObjectPtr
com::xiaonei::xce::DbObserver::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __com__xiaonei__xce__DbObserver_ids[2] =
{
    "::Ice::Object",
    "::com::xiaonei::xce::DbObserver"
};

bool
com::xiaonei::xce::DbObserver::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__com__xiaonei__xce__DbObserver_ids, __com__xiaonei__xce__DbObserver_ids + 2, _s);
}

::std::vector< ::std::string>
com::xiaonei::xce::DbObserver::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__com__xiaonei__xce__DbObserver_ids[0], &__com__xiaonei__xce__DbObserver_ids[2]);
}

const ::std::string&
com::xiaonei::xce::DbObserver::ice_id(const ::Ice::Current&) const
{
    return __com__xiaonei__xce__DbObserver_ids[1];
}

const ::std::string&
com::xiaonei::xce::DbObserver::ice_staticId()
{
    return __com__xiaonei__xce__DbObserver_ids[1];
}

::Ice::DispatchStatus
com::xiaonei::xce::DbObserver::___updated(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::com::xiaonei::xce::DbInstancePtr instance;
    __is->read(::com::xiaonei::xce::__patch__DbInstancePtr, &instance);
    __is->readPendingObjects();
    __is->endReadEncaps();
    updated(instance, __current);
    return ::Ice::DispatchOK;
}

static ::std::string __com__xiaonei__xce__DbObserver_all[] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "updated"
};

::Ice::DispatchStatus
com::xiaonei::xce::DbObserver::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__com__xiaonei__xce__DbObserver_all, __com__xiaonei__xce__DbObserver_all + 5, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __com__xiaonei__xce__DbObserver_all)
    {
        case 0:
        {
            return ___ice_id(in, current);
        }
        case 1:
        {
            return ___ice_ids(in, current);
        }
        case 2:
        {
            return ___ice_isA(in, current);
        }
        case 3:
        {
            return ___ice_ping(in, current);
        }
        case 4:
        {
            return ___updated(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
com::xiaonei::xce::DbObserver::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
com::xiaonei::xce::DbObserver::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
com::xiaonei::xce::DbObserver::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbObserver was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::DbObserver::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbObserver was not generated with stream support";
    throw ex;
}

void 
com::xiaonei::xce::__patch__DbObserverPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::com::xiaonei::xce::DbObserverPtr* p = static_cast< ::com::xiaonei::xce::DbObserverPtr*>(__addr);
    assert(p);
    *p = ::com::xiaonei::xce::DbObserverPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::com::xiaonei::xce::DbObserver::ice_staticId(), v->ice_id());
    }
}

bool
com::xiaonei::xce::operator==(const ::com::xiaonei::xce::DbObserver& l, const ::com::xiaonei::xce::DbObserver& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
com::xiaonei::xce::operator<(const ::com::xiaonei::xce::DbObserver& l, const ::com::xiaonei::xce::DbObserver& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

::Ice::ObjectPtr
com::xiaonei::xce::DbDescriptor::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __com__xiaonei__xce__DbDescriptor_ids[2] =
{
    "::Ice::Object",
    "::com::xiaonei::xce::DbDescriptor"
};

bool
com::xiaonei::xce::DbDescriptor::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__com__xiaonei__xce__DbDescriptor_ids, __com__xiaonei__xce__DbDescriptor_ids + 2, _s);
}

::std::vector< ::std::string>
com::xiaonei::xce::DbDescriptor::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__com__xiaonei__xce__DbDescriptor_ids[0], &__com__xiaonei__xce__DbDescriptor_ids[2]);
}

const ::std::string&
com::xiaonei::xce::DbDescriptor::ice_id(const ::Ice::Current&) const
{
    return __com__xiaonei__xce__DbDescriptor_ids[1];
}

const ::std::string&
com::xiaonei::xce::DbDescriptor::ice_staticId()
{
    return __com__xiaonei__xce__DbDescriptor_ids[1];
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___reload(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    reload(__current);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___getDbInstances(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::com::xiaonei::xce::DbInstanceSeq __ret = getDbInstances(__current);
    if(__ret.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        ::com::xiaonei::xce::__writeDbInstanceSeq(__os, &__ret[0], &__ret[0] + __ret.size());
    }
    __os->writePendingObjects();
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___getDbInstance(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string name;
    __is->read(name);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::com::xiaonei::xce::DbInstancePtr __ret = getDbInstance(name, __current);
    __os->write(::Ice::ObjectPtr(::IceInternal::upCast(__ret.get())));
    __os->writePendingObjects();
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___subscribe(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string name;
    ::com::xiaonei::xce::DbObserverPrx observer;
    __is->read(name);
    ::com::xiaonei::xce::__read(__is, observer);
    __is->endReadEncaps();
    subscribe(name, observer, __current);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___unsubscribe(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string name;
    ::com::xiaonei::xce::DbObserverPrx observer;
    __is->read(name);
    ::com::xiaonei::xce::__read(__is, observer);
    __is->endReadEncaps();
    unsubscribe(name, observer, __current);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___query(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string name;
    __is->read(name);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::com::xiaonei::xce::DbObserverMap __ret = query(name, __current);
    ::com::xiaonei::xce::__writeDbObserverMap(__os, __ret);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::___update(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string name;
    ::std::string action;
    ::com::xiaonei::xce::DbInstancePtr db;
    __is->read(name);
    __is->read(action);
    __is->read(::com::xiaonei::xce::__patch__DbInstancePtr, &db);
    __is->readPendingObjects();
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    bool __ret = update(name, action, db, __current);
    __os->write(__ret);
    return ::Ice::DispatchOK;
}

static ::std::string __com__xiaonei__xce__DbDescriptor_all[] =
{
    "getDbInstance",
    "getDbInstances",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "query",
    "reload",
    "subscribe",
    "unsubscribe",
    "update"
};

::Ice::DispatchStatus
com::xiaonei::xce::DbDescriptor::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__com__xiaonei__xce__DbDescriptor_all, __com__xiaonei__xce__DbDescriptor_all + 11, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __com__xiaonei__xce__DbDescriptor_all)
    {
        case 0:
        {
            return ___getDbInstance(in, current);
        }
        case 1:
        {
            return ___getDbInstances(in, current);
        }
        case 2:
        {
            return ___ice_id(in, current);
        }
        case 3:
        {
            return ___ice_ids(in, current);
        }
        case 4:
        {
            return ___ice_isA(in, current);
        }
        case 5:
        {
            return ___ice_ping(in, current);
        }
        case 6:
        {
            return ___query(in, current);
        }
        case 7:
        {
            return ___reload(in, current);
        }
        case 8:
        {
            return ___subscribe(in, current);
        }
        case 9:
        {
            return ___unsubscribe(in, current);
        }
        case 10:
        {
            return ___update(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
com::xiaonei::xce::DbDescriptor::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
com::xiaonei::xce::DbDescriptor::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
com::xiaonei::xce::DbDescriptor::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbDescriptor was not generated with stream support";
    throw ex;
}

void
com::xiaonei::xce::DbDescriptor::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type com::xiaonei::xce::DbDescriptor was not generated with stream support";
    throw ex;
}

void 
com::xiaonei::xce::__patch__DbDescriptorPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::com::xiaonei::xce::DbDescriptorPtr* p = static_cast< ::com::xiaonei::xce::DbDescriptorPtr*>(__addr);
    assert(p);
    *p = ::com::xiaonei::xce::DbDescriptorPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::com::xiaonei::xce::DbDescriptor::ice_staticId(), v->ice_id());
    }
}

bool
com::xiaonei::xce::operator==(const ::com::xiaonei::xce::DbDescriptor& l, const ::com::xiaonei::xce::DbDescriptor& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
com::xiaonei::xce::operator<(const ::com::xiaonei::xce::DbDescriptor& l, const ::com::xiaonei::xce::DbDescriptor& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}
