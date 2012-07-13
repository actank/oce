#include "LoginCacheAdapter.h"

using namespace passport;

void WebLoginCacheAdapter::set(int uid,const string& ticket,int second){
    try {
        getProxy(uid)->set(uid,ticket,second);
    } catch(Ice::Exception& e) {
        MCE_WARN("WebLoginCacheAdapter::set id = " << uid << " ticket = " << ticket << " second = " << second << " Exception : " << e.what());
        throw e;
    }
}

string WebLoginCacheAdapter::id2Ticket(int userId){
    try {
        return getProxy(userId)->id2Ticket(userId);
    } catch(Ice::Exception& e) {
        MCE_WARN("WebLoginCacheAdapter::id2Ticket id = " << userId << " Exception : " << e.what());
        throw e;
    }
}

void WapLoginCacheAdapter::set(int uid,const string& ticket,int second){
    try {
        getProxy(uid)->set(uid,ticket,second);
    } catch(Ice::Exception& e) {
        MCE_WARN("WapLoginCacheAdapter::set id = " << uid << " ticket = " << ticket << " second = " << second << " Exception : " << e.what());
        throw e;
    }
}

string WapLoginCacheAdapter::id2Ticket(int userId){
    try {
        return getProxy(userId)->id2Ticket(userId);
    } catch(Ice::Exception& e) {
        MCE_WARN("WapLoginCacheAdapter::id2Ticket id = " << userId << " Exception : " << e.what());
        throw e;
    }
}

int WapLoginCacheAdapter::ticket2Id(const string& ticket){
    try {
        if(ticket.length()==33){
            return getProxy(ticket.at(32) - '0')->ticket2Id(ticket);
        }else{
            return -1;
        }
    } catch(Ice::Exception& e) {
        MCE_WARN("WapLoginCacheAdapter::ticket2Id ticket = " << ticket << " Exception : " << e.what());
        throw e;
    }
}

void MobileClientLoginCacheAdapter::set(int uid,const string& ticket,int second){
    try {
        getProxy(uid)->set(uid,ticket,second);
    } catch(Ice::Exception& e) {
        MCE_WARN("MobileClientLoginCacheAdapter::set id = " << uid << " ticket = " << ticket << " second = " << second << " Exception : " << e.what());
        throw e;
    }
}

string MobileClientLoginCacheAdapter::id2Ticket(int userId){
    try {
        return getProxy(userId)->id2Ticket(userId);
    } catch(Ice::Exception& e) {
        MCE_WARN("MobileClientLoginCacheAdapter::id2Ticket id = " << userId << " Exception : " << e.what());
        throw e;
    }
}

int MobileClientLoginCacheAdapter::ticket2Id(const string& ticket){
    try {
        if(ticket.length()==33){
            return getProxy(ticket.at(32) - '0')->ticket2Id(ticket);
        }else{
            return -1;
        }
    } catch(Ice::Exception& e) {
        MCE_WARN("MobileClientLoginCacheAdapter::ticket2Id ticket = " << ticket << " Exception : " << e.what());
        throw e;
    }
}
