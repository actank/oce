// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1
// Generated from file `XceLogger.ice'

#ifndef __XceLogger_h__
#define __XceLogger_h__

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

namespace xce
{

namespace log
{

class Logger;

}

}

}

namespace xce
{

namespace log
{

class Logger;
bool operator==(const Logger&, const Logger&);
bool operator<(const Logger&, const Logger&);

}

}

namespace IceInternal
{

::Ice::Object* upCast(::xce::log::Logger*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::log::Logger*);

}

namespace xce
{

namespace log
{

typedef ::IceInternal::Handle< ::xce::log::Logger> LoggerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::log::Logger> LoggerPrx;

void __read(::IceInternal::BasicStream*, LoggerPrx&);
void __patch__LoggerPtr(void*, ::Ice::ObjectPtr&);

}

}

namespace xce
{

namespace log
{

typedef ::std::vector< ::std::string> StringSeq;

struct LogEntry
{
    ::xce::log::StringSeq categories;
    ::std::string message;

    bool operator==(const LogEntry&) const;
    bool operator<(const LogEntry&) const;
    bool operator!=(const LogEntry& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const LogEntry& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const LogEntry& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const LogEntry& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

typedef ::std::vector< ::xce::log::LogEntry> LogEntries;
void __writeLogEntries(::IceInternal::BasicStream*, const ::xce::log::LogEntry*, const ::xce::log::LogEntry*);
void __readLogEntries(::IceInternal::BasicStream*, LogEntries&);

}

}

namespace IceProxy
{

namespace xce
{

namespace log
{

class Logger : virtual public ::IceProxy::Ice::Object
{
public:

    void append(const ::xce::log::LogEntries& messages)
    {
        append(messages, 0);
    }
    void append(const ::xce::log::LogEntries& messages, const ::Ice::Context& __ctx)
    {
        append(messages, &__ctx);
    }
    
private:

    void append(const ::xce::log::LogEntries&, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<Logger> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Logger> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Logger*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<Logger*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
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

namespace IceDelegate
{

namespace xce
{

namespace log
{

class Logger : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void append(const ::xce::log::LogEntries&, const ::Ice::Context*) = 0;
};

}

}

}

namespace IceDelegateM
{

namespace xce
{

namespace log
{

class Logger : virtual public ::IceDelegate::xce::log::Logger,
               virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void append(const ::xce::log::LogEntries&, const ::Ice::Context*);
};

}

}

}

namespace IceDelegateD
{

namespace xce
{

namespace log
{

class Logger : virtual public ::IceDelegate::xce::log::Logger,
               virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void append(const ::xce::log::LogEntries&, const ::Ice::Context*);
};

}

}

}

namespace xce
{

namespace log
{

class Logger : virtual public ::Ice::Object
{
public:

    typedef LoggerPrx ProxyType;
    typedef LoggerPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void append(const ::xce::log::LogEntries&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___append(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

}

}

#endif
