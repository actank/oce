#ifndef IPTABLE_ICE
#define IPTABLE_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module iptab
{


    class IPTable
    {
        int id;
        long start;
        long end;
        int univ;
        string ipmask;
    };
    
    class UnivInfo
    {
        int id ;
        string url;
        string name;
        int counta;
        int countf;
        int countm;
        long tribe;
        int countstar;
        int anncount;
        string logourl;
    };
    
    sequence<UnivInfo> UnivInfoSeq;
    sequence<IPTable> IPTableSeq;
    
    
    dictionary<int,UnivInfo> UnivInfoMap;
    dictionary<int,IPTable> ID2IPTableMap; 
    dictionary<int,IPTableSeq> UnivIPTables;
    
	class IPTableManager
	{
        IPTable getIPTable(int id);
        UnivInfoSeq getUnivByIp(string ip);
        MyUtil::IntSeq getUnivIdByIp(string ip);
        IPTableSeq getIPTables(int univid);    
        bool isValid(long ip, int univ);
        void addIPTable(MyUtil::Str2StrMap ipt);
        void addUnivInfo(MyUtil::Str2StrMap univ);
        void remove(int id);
    };
};
};
};
};
#endif

