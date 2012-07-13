// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1
// Generated from file `replynotify.ice'

#ifndef __replynotify_h__
#define __replynotify_h__

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
#include <oldslice/util.h>
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

namespace reply
{

class ReplyContent;

class ReplyNotifyIndexManager;

class ReplyNotifyContentManager;

class ReplyNotifyNginx;

class ReplyNotifyDispatcher;

class FastcgiTemplateManager;

}

}

}

namespace xce
{

namespace reply
{

class ReplyContent;
bool operator==(const ReplyContent&, const ReplyContent&);
bool operator<(const ReplyContent&, const ReplyContent&);

class ReplyNotifyIndexManager;
bool operator==(const ReplyNotifyIndexManager&, const ReplyNotifyIndexManager&);
bool operator<(const ReplyNotifyIndexManager&, const ReplyNotifyIndexManager&);

class ReplyNotifyContentManager;
bool operator==(const ReplyNotifyContentManager&, const ReplyNotifyContentManager&);
bool operator<(const ReplyNotifyContentManager&, const ReplyNotifyContentManager&);

class ReplyNotifyNginx;
bool operator==(const ReplyNotifyNginx&, const ReplyNotifyNginx&);
bool operator<(const ReplyNotifyNginx&, const ReplyNotifyNginx&);

class ReplyNotifyDispatcher;
bool operator==(const ReplyNotifyDispatcher&, const ReplyNotifyDispatcher&);
bool operator<(const ReplyNotifyDispatcher&, const ReplyNotifyDispatcher&);

class FastcgiTemplateManager;
bool operator==(const FastcgiTemplateManager&, const FastcgiTemplateManager&);
bool operator<(const FastcgiTemplateManager&, const FastcgiTemplateManager&);

}

}

namespace IceInternal
{

::Ice::Object* upCast(::xce::reply::ReplyContent*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::reply::ReplyContent*);

::Ice::Object* upCast(::xce::reply::ReplyNotifyIndexManager*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::reply::ReplyNotifyIndexManager*);

::Ice::Object* upCast(::xce::reply::ReplyNotifyContentManager*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::reply::ReplyNotifyContentManager*);

::Ice::Object* upCast(::xce::reply::ReplyNotifyNginx*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::reply::ReplyNotifyNginx*);

::Ice::Object* upCast(::xce::reply::ReplyNotifyDispatcher*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::reply::ReplyNotifyDispatcher*);

::Ice::Object* upCast(::xce::reply::FastcgiTemplateManager*);
::IceProxy::Ice::Object* upCast(::IceProxy::xce::reply::FastcgiTemplateManager*);

}

namespace xce
{

namespace reply
{

typedef ::IceInternal::Handle< ::xce::reply::ReplyContent> ReplyContentPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::reply::ReplyContent> ReplyContentPrx;

void __read(::IceInternal::BasicStream*, ReplyContentPrx&);
void __patch__ReplyContentPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::xce::reply::ReplyNotifyIndexManager> ReplyNotifyIndexManagerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::reply::ReplyNotifyIndexManager> ReplyNotifyIndexManagerPrx;

void __read(::IceInternal::BasicStream*, ReplyNotifyIndexManagerPrx&);
void __patch__ReplyNotifyIndexManagerPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::xce::reply::ReplyNotifyContentManager> ReplyNotifyContentManagerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::reply::ReplyNotifyContentManager> ReplyNotifyContentManagerPrx;

void __read(::IceInternal::BasicStream*, ReplyNotifyContentManagerPrx&);
void __patch__ReplyNotifyContentManagerPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::xce::reply::ReplyNotifyNginx> ReplyNotifyNginxPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::reply::ReplyNotifyNginx> ReplyNotifyNginxPrx;

void __read(::IceInternal::BasicStream*, ReplyNotifyNginxPrx&);
void __patch__ReplyNotifyNginxPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::xce::reply::ReplyNotifyDispatcher> ReplyNotifyDispatcherPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::reply::ReplyNotifyDispatcher> ReplyNotifyDispatcherPrx;

void __read(::IceInternal::BasicStream*, ReplyNotifyDispatcherPrx&);
void __patch__ReplyNotifyDispatcherPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::xce::reply::FastcgiTemplateManager> FastcgiTemplateManagerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::xce::reply::FastcgiTemplateManager> FastcgiTemplateManagerPrx;

void __read(::IceInternal::BasicStream*, FastcgiTemplateManagerPrx&);
void __patch__FastcgiTemplateManagerPtr(void*, ::Ice::ObjectPtr&);

}

}

namespace xce
{

namespace reply
{

typedef ::std::vector< ::xce::reply::ReplyContentPtr> ReplyContentSeq;
void __writeReplyContentSeq(::IceInternal::BasicStream*, const ::xce::reply::ReplyContentPtr*, const ::xce::reply::ReplyContentPtr*);
void __readReplyContentSeq(::IceInternal::BasicStream*, ReplyContentSeq&);

typedef ::std::map< ::Ice::Long, ::xce::reply::ReplyContentPtr> ReplyContentDict;
void __writeReplyContentDict(::IceInternal::BasicStream*, const ReplyContentDict&);
void __readReplyContentDict(::IceInternal::BasicStream*, ReplyContentDict&);

}

}

namespace IceProxy
{

namespace xce
{

namespace reply
{

class ReplyContent : virtual public ::IceProxy::Ice::Object
{
public:
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyContent> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyContent*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<ReplyContent*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class ReplyNotifyIndexManager : virtual public ::IceProxy::Ice::Object
{
public:

    void addReply(const ::xce::reply::ReplyContentPtr& reply, const ::MyUtil::IntSeq& target)
    {
        addReply(reply, target, 0);
    }
    void addReply(const ::xce::reply::ReplyContentPtr& reply, const ::MyUtil::IntSeq& target, const ::Ice::Context& __ctx)
    {
        addReply(reply, target, &__ctx);
    }
    
private:

    void addReply(const ::xce::reply::ReplyContentPtr&, const ::MyUtil::IntSeq&, const ::Ice::Context*);
    
public:

    void readByReplyId(::Ice::Int uid, ::Ice::Long replyId)
    {
        readByReplyId(uid, replyId, 0);
    }
    void readByReplyId(::Ice::Int uid, ::Ice::Long replyId, const ::Ice::Context& __ctx)
    {
        readByReplyId(uid, replyId, &__ctx);
    }
    
private:

    void readByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*);
    
public:

    void readBySource(::Ice::Int uid, ::Ice::Long source)
    {
        readBySource(uid, source, 0);
    }
    void readBySource(::Ice::Int uid, ::Ice::Long source, const ::Ice::Context& __ctx)
    {
        readBySource(uid, source, &__ctx);
    }
    
private:

    void readBySource(::Ice::Int, ::Ice::Long, const ::Ice::Context*);
    
public:

    void removeByReplyId(::Ice::Int uid, ::Ice::Long replyId)
    {
        removeByReplyId(uid, replyId, 0);
    }
    void removeByReplyId(::Ice::Int uid, ::Ice::Long replyId, const ::Ice::Context& __ctx)
    {
        removeByReplyId(uid, replyId, &__ctx);
    }
    
private:

    void removeByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*);
    
public:

    ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int uid, const ::MyUtil::IntSeq& type, ::Ice::Int begin, ::Ice::Int limit)
    {
        return getReplySeq(uid, type, begin, limit, 0);
    }
    ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int uid, const ::MyUtil::IntSeq& type, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Context& __ctx)
    {
        return getReplySeq(uid, type, begin, limit, &__ctx);
    }
    
private:

    ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*);
    
public:

    ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int uid, const ::MyUtil::IntSeq& type, ::Ice::Int begin, ::Ice::Int limit)
    {
        return getReadedReplySeq(uid, type, begin, limit, 0);
    }
    ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int uid, const ::MyUtil::IntSeq& type, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Context& __ctx)
    {
        return getReadedReplySeq(uid, type, begin, limit, &__ctx);
    }
    
private:

    ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyIndexManager> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyIndexManager*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyIndexManager*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class ReplyNotifyContentManager : virtual public ::IceProxy::Ice::Object
{
public:

    ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq& replyIds)
    {
        return getReplyDict(replyIds, 0);
    }
    ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq& replyIds, const ::Ice::Context& __ctx)
    {
        return getReplyDict(replyIds, &__ctx);
    }
    
private:

    ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq&, const ::Ice::Context*);
    
public:

    void addReply(const ::xce::reply::ReplyContentPtr& reply)
    {
        addReply(reply, 0);
    }
    void addReply(const ::xce::reply::ReplyContentPtr& reply, const ::Ice::Context& __ctx)
    {
        addReply(reply, &__ctx);
    }
    
private:

    void addReply(const ::xce::reply::ReplyContentPtr&, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyContentManager> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyContentManager*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyContentManager*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class ReplyNotifyNginx : virtual public ::IceProxy::Ice::Object
{
public:

    bool reload()
    {
        return reload(0);
    }
    bool reload(const ::Ice::Context& __ctx)
    {
        return reload(&__ctx);
    }
    
private:

    bool reload(const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyNginx> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyNginx*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyNginx*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class ReplyNotifyDispatcher : virtual public ::IceProxy::Ice::Object
{
public:

    void dispatch(const ::MyUtil::Str2StrMap& props)
    {
        dispatch(props, 0);
    }
    void dispatch(const ::MyUtil::Str2StrMap& props, const ::Ice::Context& __ctx)
    {
        dispatch(props, &__ctx);
    }
    
private:

    void dispatch(const ::MyUtil::Str2StrMap&, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<ReplyNotifyDispatcher> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<ReplyNotifyDispatcher*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<ReplyNotifyDispatcher*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class FastcgiTemplateManager : virtual public ::IceProxy::Ice::Object
{
public:
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<FastcgiTemplateManager> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<FastcgiTemplateManager*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<FastcgiTemplateManager*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
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

namespace reply
{

class ReplyContent : virtual public ::IceDelegate::Ice::Object
{
public:
};

class ReplyNotifyIndexManager : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::MyUtil::IntSeq&, const ::Ice::Context*) = 0;

    virtual void readByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*) = 0;

    virtual void readBySource(::Ice::Int, ::Ice::Long, const ::Ice::Context*) = 0;

    virtual void removeByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*) = 0;

    virtual ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*) = 0;

    virtual ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*) = 0;
};

class ReplyNotifyContentManager : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq&, const ::Ice::Context*) = 0;

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::Ice::Context*) = 0;
};

class ReplyNotifyNginx : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual bool reload(const ::Ice::Context*) = 0;
};

class ReplyNotifyDispatcher : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void dispatch(const ::MyUtil::Str2StrMap&, const ::Ice::Context*) = 0;
};

class FastcgiTemplateManager : virtual public ::IceDelegate::Ice::Object
{
public:
};

}

}

}

namespace IceDelegateM
{

namespace xce
{

namespace reply
{

class ReplyContent : virtual public ::IceDelegate::xce::reply::ReplyContent,
                     virtual public ::IceDelegateM::Ice::Object
{
public:
};

class ReplyNotifyIndexManager : virtual public ::IceDelegate::xce::reply::ReplyNotifyIndexManager,
                                virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::MyUtil::IntSeq&, const ::Ice::Context*);

    virtual void readByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*);

    virtual void readBySource(::Ice::Int, ::Ice::Long, const ::Ice::Context*);

    virtual void removeByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*);

    virtual ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*);

    virtual ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*);
};

class ReplyNotifyContentManager : virtual public ::IceDelegate::xce::reply::ReplyNotifyContentManager,
                                  virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq&, const ::Ice::Context*);

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::Ice::Context*);
};

class ReplyNotifyNginx : virtual public ::IceDelegate::xce::reply::ReplyNotifyNginx,
                         virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual bool reload(const ::Ice::Context*);
};

class ReplyNotifyDispatcher : virtual public ::IceDelegate::xce::reply::ReplyNotifyDispatcher,
                              virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void dispatch(const ::MyUtil::Str2StrMap&, const ::Ice::Context*);
};

class FastcgiTemplateManager : virtual public ::IceDelegate::xce::reply::FastcgiTemplateManager,
                               virtual public ::IceDelegateM::Ice::Object
{
public:
};

}

}

}

namespace IceDelegateD
{

namespace xce
{

namespace reply
{

class ReplyContent : virtual public ::IceDelegate::xce::reply::ReplyContent,
                     virtual public ::IceDelegateD::Ice::Object
{
public:
};

class ReplyNotifyIndexManager : virtual public ::IceDelegate::xce::reply::ReplyNotifyIndexManager,
                                virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::MyUtil::IntSeq&, const ::Ice::Context*);

    virtual void readByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*);

    virtual void readBySource(::Ice::Int, ::Ice::Long, const ::Ice::Context*);

    virtual void removeByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Context*);

    virtual ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*);

    virtual ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Context*);
};

class ReplyNotifyContentManager : virtual public ::IceDelegate::xce::reply::ReplyNotifyContentManager,
                                  virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq&, const ::Ice::Context*);

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::Ice::Context*);
};

class ReplyNotifyNginx : virtual public ::IceDelegate::xce::reply::ReplyNotifyNginx,
                         virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual bool reload(const ::Ice::Context*);
};

class ReplyNotifyDispatcher : virtual public ::IceDelegate::xce::reply::ReplyNotifyDispatcher,
                              virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void dispatch(const ::MyUtil::Str2StrMap&, const ::Ice::Context*);
};

class FastcgiTemplateManager : virtual public ::IceDelegate::xce::reply::FastcgiTemplateManager,
                               virtual public ::IceDelegateD::Ice::Object
{
public:
};

}

}

}

namespace xce
{

namespace reply
{

class ReplyContent : virtual public ::Ice::Object
{
public:

    typedef ReplyContentPrx ProxyType;
    typedef ReplyContentPtr PointerType;
    
    ReplyContent() {}
    ReplyContent(::Ice::Long, ::Ice::Long, ::Ice::Int, ::Ice::Int, ::Ice::Int, ::Ice::Int, const ::std::string&);
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

    virtual ~ReplyContent() {}

    friend class ReplyContent__staticInit;

public:

    ::Ice::Long id;

    ::Ice::Long source;

    ::Ice::Int owner;

    ::Ice::Int from;

    ::Ice::Int type;

    ::Ice::Int time;

    ::std::string props;
};

class ReplyContent__staticInit
{
public:

    ::xce::reply::ReplyContent _init;
};

static ReplyContent__staticInit _ReplyContent_init;

class ReplyNotifyIndexManager : virtual public ::Ice::Object
{
public:

    typedef ReplyNotifyIndexManagerPrx ProxyType;
    typedef ReplyNotifyIndexManagerPtr PointerType;
    
    ReplyNotifyIndexManager() {}
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::MyUtil::IntSeq&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___addReply(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void readByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___readByReplyId(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void readBySource(::Ice::Int, ::Ice::Long, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___readBySource(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void removeByReplyId(::Ice::Int, ::Ice::Long, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___removeByReplyId(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::xce::reply::ReplyContentSeq getReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getReplySeq(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::xce::reply::ReplyContentSeq getReadedReplySeq(::Ice::Int, const ::MyUtil::IntSeq&, ::Ice::Int, ::Ice::Int, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getReadedReplySeq(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class ReplyNotifyContentManager : virtual public ::Ice::Object
{
public:

    typedef ReplyNotifyContentManagerPrx ProxyType;
    typedef ReplyNotifyContentManagerPtr PointerType;
    
    ReplyNotifyContentManager() {}
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual ::xce::reply::ReplyContentDict getReplyDict(const ::MyUtil::LongSeq&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getReplyDict(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void addReply(const ::xce::reply::ReplyContentPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___addReply(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class ReplyNotifyNginx : virtual public ::Ice::Object
{
public:

    typedef ReplyNotifyNginxPrx ProxyType;
    typedef ReplyNotifyNginxPtr PointerType;
    
    ReplyNotifyNginx() {}
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual bool reload(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___reload(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class ReplyNotifyDispatcher : virtual public ::Ice::Object
{
public:

    typedef ReplyNotifyDispatcherPrx ProxyType;
    typedef ReplyNotifyDispatcherPtr PointerType;
    
    ReplyNotifyDispatcher() {}
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void dispatch(const ::MyUtil::Str2StrMap&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class FastcgiTemplateManager : virtual public ::Ice::Object
{
public:

    typedef FastcgiTemplateManagerPrx ProxyType;
    typedef FastcgiTemplateManagerPtr PointerType;
    
    FastcgiTemplateManager() {}
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

    virtual ~FastcgiTemplateManager() {}
};

}

}

#endif
