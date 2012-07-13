#ifndef __IPTABLEI_H__
#define __IPTABLEI_H__

#include <IPTable.h>
#include <map>
#include <vector>
#include <deque>
#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceBox/IceBox.h>
#include "ServiceI.h"
#include "Channel.h"


#ifndef IPTABLE_API
#   define IPTABLE_API ICE_DECLSPEC_EXPORT
#endif

extern "C"
{
IPTABLE_API ::IceBox::Service*
        create(::Ice::CommunicatorPtr communicator);
}


namespace mop
{
namespace hi
{
namespace svc
{
namespace iptab
{

    using namespace std;
    using namespace MyUtil;

    const string UNIV_TAB ="university";
    const string UNIV_DB= "fb_shark";
    const string IP_TAB = "ip_table";
    const string IP_DB = "fb_state";
    const string IPTABLE_MANAGER = "IPTM";


    //typedef map<long ,UnivInfoMap::iterator> UInt2UnivInfoItMap;
    typedef map<UnivInfoMap::iterator, int> UnivCount;
    typedef vector<UnivInfoMap::iterator> UItSeq;
    class IPoint : public IceUtil::Shared
    {
        public:
        UnivCount bits;
        UnivCount eits;
    };

    typedef IceUtil::Handle<IPoint> IPointPtr;

class IPSorter : public MyUtil::Singleton<IPSorter>{
        map<long long ,IPointPtr> ipMap;
        UnivInfoMap univMap;
        ID2IPTableMap id2Iptable;
        UnivIPTables univ2ips;




        void addIP(long long begin, long long end, UnivInfoMap::iterator it);
        void remove(long long begin, long long end , UnivInfoMap::iterator it);
        void loadIPTable();
        void loadUnivInfo();
public:
        void initialize();

        UnivInfoSeq get(long long ip);
        UnivInfoSeq get(const string& ip);
        IPTablePtr getIPTable(int id);
        IPTableSeq getIPTables(int univid);

        UnivInfoPtr getUnivInfo(int uid);

        void addIPTable(const Str2StrMap& ipt);
        void addUnivInfo(const Str2StrMap& univ);
        void removeIpById(int id);
        IPTablePtr parseIpTable(Str2StrMap& ipt);
        IPTablePtr parseIpTable(const DefaultResultSetPtr& rs, int row);
        UnivInfoPtr parseUnivInfo(Str2StrMap& univ);
        UnivInfoPtr parseUnivInfo(const DefaultResultSetPtr& rs, int row);

        long long parseIp(const string& ip)
        {
            MCE_DEBUG("-----parseIP   ip="<<ip);
            long long res = 0;
            int j = 0;

            for(int i=0;i<ip.length(); ++i)
            {
                if(ip[i]=='.') {
                    res = res*256 + (long long)j;
                    j=0;
                }
                else{
                    j = j*10 +((int)ip[i]-(int)'0');
                }
            }
            MCE_DEBUG("ip = "<<res*256 + (long long)j);
            return res*256 + (long long)j;
        };
} ;



class IPTABLE_API IPTableServiceI : public MyUtil::ServiceI,
                            public MyUtil::Singleton<IPTableServiceI>
{
public:
    virtual void initialize();

};

//---------------------------------------------------------------------------
/*class ServantFactoryI : public MyUtil::ServantFactory
{
public:
    virtual Ice::ObjectPtr create(const Ice::Identity& id);
private:
    Ice::ObjectPtr createIPTable(const Ice::Identity& id);
    Ice::ObjectPtr createTribeMember(const Ice::Identity& id);
    Ice::ObjectPtr createTribeAdmin(const Ice::Identity& id);
    Ice::ObjectPtr createTribeBlock(const Ice::Identity& id);
};*/


class IPTableManagerI: virtual public IPTableManager
{
public:
    virtual IPTablePtr getIPTable(Ice::Int id,const Ice::Current& = Ice::Current());
    virtual UnivInfoSeq getUnivByIp(const string& ip,const Ice::Current& = Ice::Current());
    virtual IntSeq getUnivIdByIp(const string& ip,const Ice::Current& = Ice::Current());
    virtual IPTableSeq getIPTables(Ice::Int univid,const Ice::Current& = Ice::Current());
    virtual bool isValid(Ice::Long ip, Ice::Int univ,const Ice::Current& = Ice::Current());
    virtual void addIPTable(const Str2StrMap& ipt,const Ice::Current& = Ice::Current());
    virtual void addUnivInfo(const Str2StrMap& univ,const Ice::Current& = Ice::Current());
    virtual void remove(Ice::Int id,const Ice::Current& = Ice::Current());
};


bool operator<(const UnivInfoMap::iterator& it1, const UnivInfoMap::iterator& it2)
{
    return it1->first < it2->first;
};


};
};
};
};
#endif
