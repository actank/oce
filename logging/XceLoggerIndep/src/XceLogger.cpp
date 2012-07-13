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

#include <XceLogger.h>
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

static const ::std::string __xce__log__Logger__append_name = "append";

::Ice::Object* IceInternal::upCast(::xce::log::Logger* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::xce::log::Logger* p) { return p; }

void
xce::log::__read(::IceInternal::BasicStream* __is, ::xce::log::LoggerPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::xce::log::Logger;
        v->__copyFrom(proxy);
    }
}

bool
xce::log::LogEntry::operator==(const LogEntry& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(categories != __rhs.categories)
    {
        return false;
    }
    if(message != __rhs.message)
    {
        return false;
    }
    return true;
}

bool
xce::log::LogEntry::operator<(const LogEntry& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(categories < __rhs.categories)
    {
        return true;
    }
    else if(__rhs.categories < categories)
    {
        return false;
    }
    if(message < __rhs.message)
    {
        return true;
    }
    else if(__rhs.message < message)
    {
        return false;
    }
    return false;
}

void
xce::log::LogEntry::__write(::IceInternal::BasicStream* __os) const
{
    if(categories.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        __os->write(&categories[0], &categories[0] + categories.size());
    }
    __os->write(message);
}

void
xce::log::LogEntry::__read(::IceInternal::BasicStream* __is)
{
    __is->read(categories);
    __is->read(message);
}

void
xce::log::__writeLogEntries(::IceInternal::BasicStream* __os, const ::xce::log::LogEntry* begin, const ::xce::log::LogEntry* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        begin[i].__write(__os);
    }
}

void
xce::log::__readLogEntries(::IceInternal::BasicStream* __is, ::xce::log::LogEntries& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 2);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        v[i].__read(__is);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

void
IceProxy::xce::log::Logger::append(const ::xce::log::LogEntries& messages, const ::Ice::Context* __ctx)
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
            ::IceDelegate::xce::log::Logger* __del = dynamic_cast< ::IceDelegate::xce::log::Logger*>(__delBase.get());
            __del->append(messages, __ctx);
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
IceProxy::xce::log::Logger::ice_staticId()
{
    return ::xce::log::Logger::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::xce::log::Logger::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::xce::log::Logger);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::xce::log::Logger::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::xce::log::Logger);
}

::IceProxy::Ice::Object*
IceProxy::xce::log::Logger::__newInstance() const
{
    return new Logger;
}

void
IceDelegateM::xce::log::Logger::append(const ::xce::log::LogEntries& messages, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __xce__log__Logger__append_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(messages.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::xce::log::__writeLogEntries(__os, &messages[0], &messages[0] + messages.size());
        }
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
IceDelegateD::xce::log::Logger::append(const ::xce::log::LogEntries& messages, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::xce::log::LogEntries& messages, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_messages(messages)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::xce::log::Logger* servant = dynamic_cast< ::xce::log::Logger*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->append(_m_messages, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        const ::xce::log::LogEntries& _m_messages;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __xce__log__Logger__append_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(messages, __current);
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

::Ice::ObjectPtr
xce::log::Logger::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __xce__log__Logger_ids[2] =
{
    "::Ice::Object",
    "::xce::log::Logger"
};

bool
xce::log::Logger::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__xce__log__Logger_ids, __xce__log__Logger_ids + 2, _s);
}

::std::vector< ::std::string>
xce::log::Logger::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__xce__log__Logger_ids[0], &__xce__log__Logger_ids[2]);
}

const ::std::string&
xce::log::Logger::ice_id(const ::Ice::Current&) const
{
    return __xce__log__Logger_ids[1];
}

const ::std::string&
xce::log::Logger::ice_staticId()
{
    return __xce__log__Logger_ids[1];
}

::Ice::DispatchStatus
xce::log::Logger::___append(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::xce::log::LogEntries messages;
    ::xce::log::__readLogEntries(__is, messages);
    __is->endReadEncaps();
    append(messages, __current);
    return ::Ice::DispatchOK;
}

static ::std::string __xce__log__Logger_all[] =
{
    "append",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

::Ice::DispatchStatus
xce::log::Logger::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__xce__log__Logger_all, __xce__log__Logger_all + 5, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __xce__log__Logger_all)
    {
        case 0:
        {
            return ___append(in, current);
        }
        case 1:
        {
            return ___ice_id(in, current);
        }
        case 2:
        {
            return ___ice_ids(in, current);
        }
        case 3:
        {
            return ___ice_isA(in, current);
        }
        case 4:
        {
            return ___ice_ping(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
xce::log::Logger::__write(::IceInternal::BasicStream* __os) const
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
xce::log::Logger::__read(::IceInternal::BasicStream* __is, bool __rid)
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
xce::log::Logger::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type xce::log::Logger was not generated with stream support";
    throw ex;
}

void
xce::log::Logger::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type xce::log::Logger was not generated with stream support";
    throw ex;
}

void 
xce::log::__patch__LoggerPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::xce::log::LoggerPtr* p = static_cast< ::xce::log::LoggerPtr*>(__addr);
    assert(p);
    *p = ::xce::log::LoggerPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::xce::log::Logger::ice_staticId(), v->ice_id());
    }
}

bool
xce::log::operator==(const ::xce::log::Logger& l, const ::xce::log::Logger& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
xce::log::operator<(const ::xce::log::Logger& l, const ::xce::log::Logger& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}
