#ifndef PROXY_ICE
#define PROXY_ICE

module MyUtil
{
    interface Proxy
    {
    	string route(string message);
    };
};
#endif
