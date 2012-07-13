#include "IPTableI.h"
#include <boost/lexical_cast.hpp>

using namespace mop::hi::svc::iptab;
using namespace MyUtil;

extern "C"
{

//
// Factory function
//
IPTABLE_API ::IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return &IPTableServiceI::instance();
}

}


void
IPTableServiceI::initialize()
{
    //ServantFactoryPtr factory = new SyncServantFactory(new ServantFactoryI);


	getAdapter()->add(new IPTableManagerI,
		createIdentity(IPTABLE_MANAGER,""));


	IPSorter::instance().initialize();
}


//---------------------------------

IPTablePtr
IPTableManagerI::getIPTable(Ice::Int id, const Ice::Current& current)
{
    return IPSorter::instance().getIPTable(id);
}

UnivInfoSeq
IPTableManagerI::getUnivByIp(const string& ip, const Ice::Current& current)
{
    return IPSorter::instance().get(const_cast<string&>(ip));
}

IntSeq
IPTableManagerI::getUnivIdByIp(const string& ip, const Ice::Current& current)
{
    UnivInfoSeq seq = IPSorter::instance().get(ip);
    IntSeq res;
    for(int i =0; i<seq.size(); ++i)
    {
        res.push_back(seq.at(i)->id);
    }
    return res;
}

IPTableSeq
IPTableManagerI::getIPTables(Ice::Int univid,const Ice::Current& current)
{
    return IPSorter::instance().getIPTables(univid);
}

bool
IPTableManagerI::isValid(Ice::Long ip, Ice::Int univ, const Ice::Current& current)
{
    UnivInfoSeq seq = IPSorter::instance().get(ip);
    for(int i = 0; i<seq.size(); i++)
    {
        if(seq.at(i)->id == univ) return true;
    }
    return false;
}

void
IPTableManagerI::addIPTable(const Str2StrMap& ipt,const Ice::Current& current)
{
    IPSorter::instance().addIPTable(ipt);
}

void
IPTableManagerI::remove(Ice::Int id,const Ice::Current& current)
{
    IPSorter::instance().removeIpById(id);
    MCE_DEBUG("do delete sql");
    ostringstream sql ;
    sql<< "delete from ip_table where id = " << id;
    DatabaseManager::instance().getDatabase(IP_DB)->doDelete(sql.str());

}

void
IPTableManagerI::addUnivInfo(const Str2StrMap& univ,const Ice::Current& current)
{
    IPSorter::instance().addUnivInfo(univ);
}

//---------------------------------
void
IPSorter::initialize()
{
    MCE_DEBUG("----call loadUnivInfo ----");
    loadUnivInfo();
    MCE_DEBUG("----call loadIPTable -----");
    loadIPTable();
}

IPTablePtr
IPSorter::getIPTable(int id)
{
    ID2IPTableMap::iterator it = id2Iptable.find(id);
    if(it != id2Iptable.end()) return it->second;
    else return 0;
}



UnivInfoPtr
IPSorter::getUnivInfo(int uid)
{
    UnivInfoMap::iterator it = univMap.find(uid);
    if(it != univMap.end()) return it->second;
    else{
        ostringstream sql;
        sql << "select * from university where id = " << uid;
        DefaultResultSetPtr rs = DefaultResultSetPtr::dynamicCast(
        	DatabaseManager::instance().doSelect(UNIV_DB, sql.str()));

		if (rs->empty()) {
			return 0;
		}
        UnivInfoPtr ptr = parseUnivInfo(rs, 0);
        if (!ptr) {
        	return 0;
        }
        univMap[uid] = ptr;
        return ptr;
    }
}

void
IPSorter::addIPTable(const Str2StrMap& ipt)
{
    IPTablePtr iptab = parseIpTable(const_cast<Str2StrMap&>(ipt));

    iptab->id = EntityHelper::instance().doInsert(IP_TAB, ipt);

    id2Iptable[iptab->id] = iptab;

    UnivIPTables::iterator ui = univ2ips.find(iptab->univ);
    if(ui != univ2ips.end())
    {
        ui->second.push_back(iptab);
    }else
    {
        IPTableSeq seq;
        seq.push_back(iptab);
        univ2ips[iptab->univ] = seq;
    }

    if(getUnivInfo(iptab->univ)!=0) addIP(iptab->start, iptab->end,univMap.find(iptab->univ) );
}



void
IPSorter::addUnivInfo(const Str2StrMap& univ)
{
    UnivInfoPtr uip = parseUnivInfo(const_cast<Str2StrMap&>(univ));
    EntityHelper::instance().doInsert(UNIV_TAB, univ);
    univMap[uip->id] = uip;
}

void
IPSorter::loadIPTable()
{
    MCE_DEBUG("----- in loadIPTable() -----");
    string sql = "select * from ip_table";
    DefaultResultSetPtr rs = DefaultResultSetPtr::dynamicCast(
    	DatabaseManager::instance().doSelect(IP_DB, sql));

    for(int i=0;i< rs->size(); ++i)
    {
        MCE_DEBUG("--------------------------------------");
        IPTablePtr iptab = parseIpTable(rs, i);
		if (!iptab) {
			continue;
		}
        id2Iptable[iptab->id] = iptab;
        UnivIPTables::iterator ui = univ2ips.find(iptab->univ);
        if(ui != univ2ips.end())
        {
            MCE_DEBUG("univ is exist");
            ui->second.push_back(iptab);
        }else
        {
            MCE_DEBUG("add a univ to univ2ips");
            IPTableSeq seq;
            seq.push_back(iptab);
            univ2ips[iptab->univ] = seq;
        }

        if(getUnivInfo(iptab->univ)!=0)
        {
            MCE_DEBUG("call addIP     start="<<iptab->start<<"     end="<<iptab->end<<"    univid="<<iptab->univ);
            addIP(iptab->start, iptab->end, univMap.find(iptab->univ));
        }
    }
}



void
IPSorter::loadUnivInfo()
{
    string sql = "select * from university";
    DefaultResultSetPtr rs = DefaultResultSetPtr::dynamicCast(
    	DatabaseManager::instance().doSelect(UNIV_DB, sql));
    for(int i=0; i<rs->size();++i)
    {

        UnivInfoPtr uip = parseUnivInfo(rs, i);
        univMap[uip->id] = uip;
        MCE_DEBUG("UIP->ID =" <<uip->id <<"   "<< uip->name );
    }
}


IPTablePtr
IPSorter::parseIpTable(Str2StrMap& ipt)
{
    IPTablePtr res = new IPTable();

    try
    {
        res->id = boost::lexical_cast<int>(ipt["ID"]);
    }catch(std::exception e)
    {
        MCE_WARN("id can't cast to int");
    }
    try{
        MCE_DEBUG("boost::lexical_cast<long>(ipt[\"START\"])   = "<<ipt["START"]);
        res->start = boost::lexical_cast<long long>(ipt["START"]);

    }catch(std::exception e)
    {
        MCE_WARN("start can't cast to long");
    }
    try
    {
        MCE_DEBUG("boost::lexical_cast<long>(ipt[\"END\"])   = "<<ipt["END"]);
        res->end = boost::lexical_cast<long long>(ipt["END"]);
    }catch(std::exception e)
    {
        MCE_WARN("end can't cast to long");
    }
    try
    {
        res->univ = boost::lexical_cast<int>(ipt["UNIV"]);
    }catch(std::exception e)
    {
        MCE_WARN("univ can't cast to int");
    }
    res->ipmask = ipt["IP_MASK"];
    return res;
}

IPTablePtr
IPSorter::parseIpTable(const DefaultResultSetPtr& rs, int row)
{
    IPTablePtr res = new IPTable();
    try {
        res->id = rs->getField<int>(row, "ID");
        res->start = rs->getField<long long>(row, "START");
	    res->end = rs->getField<long long>(row, "END");
        res->univ = rs->getField<int>(row, "UNIV");
	   	res->ipmask = rs->getField(row, "IP_MASK");
	   	return res;
	} catch (...) {
		return 0;
	}
}


UnivInfoPtr
IPSorter::parseUnivInfo(Str2StrMap& univ)
{
    UnivInfoPtr res = new UnivInfo();
    try{
        res->id = boost::lexical_cast<int>(univ["ID"]);
    }catch(std::exception e)
    {
        MCE_WARN("id can't cast to int");
    }
    res->url = univ["URL"];
    res->name = univ["NAME"];
    try{
        res->counta = boost::lexical_cast<int>(univ["COUNT_A"]);
    }catch(std::exception e)
    {
        MCE_WARN("count_a can't cast to int");
    }

    try{
        res->countf = boost::lexical_cast<int>(univ["COUNT_F"]);
    }catch(std::exception e)
    {
        MCE_WARN("count_f can't cast to int");
    }
    try{
        res->countm = boost::lexical_cast<int>(univ["COUNT_M"]);
    }catch(std::exception e)
    {
        MCE_WARN("count_m can't cast to int");
    }
    try{
        res->tribe = boost::lexical_cast<long long>(univ["TRIBE"]);
    }catch(std::exception e)
    {
        MCE_WARN("tribe can't cast to long");
    }
    try{
        res->countstar = boost::lexical_cast<int>(univ["COUNT_STAR"]);
    }catch(std::exception e)
    {
        MCE_WARN("count_star can't cast to int");
    }
    try{
        res->anncount = boost::lexical_cast<int>(univ["ANN_COUNT"]);
    }catch(std::exception e)
    {
        MCE_WARN("ann_count can't cast to int");
    }
    res->logourl = univ["LOGO_URL"];

    return res;
}


UnivInfoPtr
IPSorter::parseUnivInfo(const DefaultResultSetPtr& rs, int row)
{
    UnivInfoPtr res = new UnivInfo();
    try{
        res->id = rs->getField<int>(row, "ID");
	    res->url = rs->getField(row, "URL");
	    res->name = rs->getField(row, "NAME");
        res->counta = rs->getField<int>(row, "COUNT_A");
        res->countf = rs->getField<int>(row, "COUNT_F");
        res->countm = rs->getField<int>(row, "COUNT_M");
        res->tribe = rs->getField<long long>(row, "TRIBE");
        res->countstar = rs->getField<int>(row, "COUNT_STAR");
        res->anncount = rs->getField<int>(row, "ANN_COUNT");
    	res->logourl = rs->getField(row, "LOGO_URL");
    	return res;
	} catch (...) {
		return 0;
	}
}


void
IPSorter::addIP(long long begin, long long end, UnivInfoMap::iterator it)
{
    MCE_DEBUG("call addIP");
    if(begin > end) return;

    IPointPtr p1 = new IPoint();
    IPointPtr p2 = new IPoint();

    pair< map<long long, IPointPtr>::iterator , bool > pr1 = ipMap.insert(pair<long long, IPointPtr>(begin,p1));
    pair< map<long long, IPointPtr>::iterator , bool > pr2 = ipMap.insert(pair<long long, IPointPtr>(end,p2));

    map<long long, IPointPtr>::iterator it0 = ipMap.find(begin);
    map<long long, IPointPtr>::iterator itmp0 = it0;
    map<long long, IPointPtr>::iterator it1 = ipMap.find(end);
    MCE_DEBUG("pr1->second = "<< pr1.second);
    MCE_DEBUG("pr2->second = "<< pr2.second);


    UnivCount ipdeq1;
    UnivCount ipdeq2;


    do{
        MCE_DEBUG("while1   it0->first =" << it0->first <<"     it0->second="<<it0->second);


        if((!it0->second->eits.empty()) && (it0!=itmp0) )
        {
            MCE_DEBUG("if(!it0->second->eits.empty())");
            for(UnivCount::iterator i = it0->second->eits.begin();
                    i != it0->second->eits.end(); ++i)
            {
                UnivCount::iterator itmp = ipdeq2.find(i->first);
                if(itmp!= ipdeq2.end()) itmp->second += i->second;
                else ipdeq2[i->first] = i->second;
            }
        }

        if(!it0->second->bits.empty())
        {
            MCE_DEBUG("!it0->second->bits.empty()");
            ipdeq1 = it0->second->bits;
            if(!ipdeq2.empty()){
                for(UnivCount::iterator i = ipdeq2.begin();
                        i!= ipdeq2.end(); ++i )
                {
                    UnivCount::iterator itmp = ipdeq1.find(i->first);
                    itmp->second -= i->second;
                    if(itmp->second == 0) ipdeq1.erase(itmp);
                }
            }
            break;

        }

        MCE_DEBUG(2)
    }while(it0-- != ipMap.begin());
    MCE_DEBUG(3);




    //���ò�����Begin End��Ϣ
    it0 = ipMap.find(begin);
    it1 = ipMap.find(end);

    if(it0->second->bits.empty())
    {
        it0->second->bits = ipdeq1;
    }
    MCE_DEBUG(4);
    UnivCount::iterator uci1 = it0->second->bits.find(it);
    UnivCount::iterator uci2 = it1->second->eits.find(it);



    MCE_DEBUG(5);
    if(uci1 != it0->second->bits.end())
    {
        MCE_DEBUG("---------chanage into bits    uci1 -> second ="<<uci1 -> second<<"-----------");
        uci1 -> second ++;
    }else
    {
        MCE_DEBUG("---------insert into bits------------");
        it0->second->bits.insert(pair<UnivInfoMap::iterator, int>(it,1));
    }

    if(uci2 != it1->second->eits.end())
    {
        MCE_DEBUG("---------chanage into eits    uci2 -> second="<<uci2 -> second<<"-------------");
        uci2 -> second ++;
    }else
    {
        MCE_DEBUG("---------insert into eits------------");
        it1 -> second->eits.insert(pair<UnivInfoMap::iterator, int>(it,1));
    }

    MCE_DEBUG(6);
//����begin end֮���begin���

    MCE_DEBUG("ip between: "<<it0->first <<" and "<<it1->first);


    if(it0 != it1 ){

        it0++;
        it1++;

        for(map<long long, IPointPtr>::iterator i = it0;
            i!= it1; i++)
        {
            MCE_DEBUG("i =="<<i->first);
            if(!i->second->bits.empty())
            {
                //MCE_DEBUG(9);
                UnivCount::iterator uci = i->second->bits.find(it);
                //MCE_DEBUG(10);
                if(uci != i->second->bits.end())
                {
                    uci->second++;
                }else
                {
                    i->second->bits.insert(pair<UnivInfoMap::iterator, int>(it,1));
                }
            }
            //MCE_DEBUG(18);
        }
    }



}


void
IPSorter::remove(long long begin, long long end , UnivInfoMap::iterator it)
{
    MCE_DEBUG("--- call remove -----");
    addIP(begin, end,it);

    int count=0;
    map<long long ,IPointPtr>::iterator it0,it1;
    it0= ipMap.find(begin);
    it1= ipMap.find(end);


    do{
        MCE_DEBUG("remove:1");
        if(!it0->second->eits.empty())
        {
            MCE_DEBUG("remove:2");
            UnivCount::iterator i = it0->second->eits.find(it);
            if(i != it0->second->eits.end()) count-= i->second;
        }

        if(!it0->second->bits.empty())
        {
            MCE_DEBUG("remove:3");
            UnivCount::iterator i = it0->second->bits.find(it);
            if(i != it0->second->bits.end()) count+= i->second;
            break;
        }
    }while(it0-- != ipMap.begin());



    if((begin<end) && (count>1))
    {
        MCE_DEBUG("remove:4");
        IPointPtr pt = new IPoint();
        pt->eits.insert(pair<UnivInfoMap::iterator, int>(it,count));
        ipMap.insert(pair<long long ,IPointPtr>(begin-1,pt));
    }else if((count>0) && (begin == end))
    {
        MCE_DEBUG("remove:5");
        IPointPtr pt = new IPoint();
        pt->eits.insert(pair<UnivInfoMap::iterator, int>(it,count));
        ipMap.insert(pair<long long ,IPointPtr>(begin-1,pt));
    }

    //���Begin end����Count
    it0= ipMap.find(begin);
    it1= ipMap.find(end);
    count = 0;


        MCE_DEBUG("remove:6");
        UnivCount ipdeq1;
        UnivCount ipdeq2;
       // it1--;

        do{
            MCE_DEBUG("remove:7");
            if(!it1->second->eits.empty())
            {
                MCE_DEBUG("remove:8");
                for(UnivCount::iterator i = it1->second->eits.begin();
                        i != it1->second->eits.end(); ++i)
                {
                    UnivCount::iterator itmp = ipdeq2.find(i->first);
                    if(itmp!= ipdeq2.end()) itmp->second += i->second;
                    else ipdeq2[i->first] = i->second;
                }
            }

            if(!it1->second->bits.empty())
            {
                MCE_DEBUG("remove:9");
                ipdeq1 = it1->second->bits;
                if(!ipdeq2.empty()){
                    for(UnivCount::iterator i = ipdeq2.begin();
                            i!= ipdeq2.end(); ++i )
                    {
                        UnivCount::iterator itmp = ipdeq1.find(i->first);
                        itmp->second -= i->second;
                        if(itmp->second == 0) ipdeq1.erase(itmp);
                    }
                }
                break;

            }
            MCE_DEBUG("remove:10");

        }while(it1-- != it0);
        MCE_DEBUG("remove:11");
        IPointPtr p1 = new IPoint();
        p1->bits = ipdeq1;
        //p1->bits[it] = count;
        ipMap.insert(pair<long long, IPointPtr>(end +1,p1));
    //}
    //��� begin end ���it

    it0= ipMap.find(begin);
    it1= ipMap.find(end);
    it1++;
    MCE_DEBUG("remove:12");
    for(map<long long ,IPointPtr>::iterator i = it0; i!= it1;)
    {

        if(!i->second->eits.empty())
        {
            MCE_DEBUG("remove:13");
            UnivCount::iterator itmp = i->second->eits.find(it);
            if(itmp != i->second->eits.end()) i->second->eits.erase(itmp);
        }

        if(!i->second->bits.empty())
        {
            MCE_DEBUG("remove:14");
            UnivCount::iterator itmp = i->second->bits.find(it);
            if(itmp != i->second->bits.end()) i->second->bits.erase(itmp);

        }

        if(i->second->eits.empty() && i->second->bits.empty() )
        {
            MCE_DEBUG("remove:15");
            ipMap.erase(i++);
        }else i++;
        MCE_DEBUG("remove:16");
    }


}

UnivInfoSeq
IPSorter::get(const string & ip)
{
    return get(parseIp(ip));
}

UnivInfoSeq
IPSorter::get(long long ip)
{
    MCE_DEBUG("-----call get-------");
    map<long long, IPointPtr>::iterator it = ipMap.find(ip);
    map<long long, IPointPtr>::iterator itmp =it;
    MCE_DEBUG("get:1");
    UnivInfoSeq res;

    if(it == ipMap.end())
    {
        MCE_DEBUG("get:2");
        IPointPtr iptr = new IPoint();
        pair< map<long long, IPointPtr>::iterator , bool > pr = ipMap.insert(pair<long long, IPointPtr>(ip,iptr));
        itmp = pr.first;
        it = itmp;
        MCE_DEBUG("get:3");
    }else{
        if(!it->second->bits.empty())
        {
           for(UnivCount::iterator i = it ->second->bits.begin();
                i != it->second->bits.end(); ++ i)
            {
                res.push_back(i->first->second);
                MCE_DEBUG("UNIV Count:"<<i->first->first<<" = "<<i->second);
            }
            return res;
        }
    }



    it--;
    UnivCount ipdeq1;
    UnivCount ipdeq2;
    do{
        MCE_DEBUG("get:4");


        /*if(!it->second->bits.empty())
        {
             MCE_DEBUG("get:5");
            for(UnivCount::iterator i = it ->second->bits.begin();
                i != it->second->bits.end(); ++ i)
            {
                res.push_back(i->first->second);
                MCE_DEBUG("UNIV Count:"<<i->first->first<<" = "<<i->second);
            }
            break;
        }
        MCE_DEBUG("get:44");*/

            if(!it->second->eits.empty())
            {
                for(UnivCount::iterator i = it->second->eits.begin();
                        i != it->second->eits.end(); ++i)
                {
                    UnivCount::iterator itmp = ipdeq2.find(i->first);
                    if(itmp!= ipdeq2.end()) itmp->second += i->second;
                    else ipdeq2[i->first] = i->second;
                }
            }
            if(!it->second->bits.empty())
            {
                ipdeq1 = it->second->bits;
                if(!ipdeq2.empty()){
                    for(UnivCount::iterator i = ipdeq2.begin();
                            i!= ipdeq2.end(); ++i )
                    {
                        UnivCount::iterator itmp = ipdeq1.find(i->first);
                        itmp->second -= i->second;
                        if(itmp->second == 0) ipdeq1.erase(itmp);
                    }
                }

                for(UnivCount::iterator i = ipdeq1.begin();
                    i != ipdeq1.end(); ++ i)
                {
                    res.push_back(i->first->second);
                    MCE_DEBUG("UNIV Count:"<<i->first->first<<" = "<<i->second);
                }
                break;

            }

    }while(it-- != ipMap.begin());

    MCE_DEBUG("get:6   itmp" <<(itmp != ipMap.end()));
    if(itmp == ipMap.end() )ipMap.erase(itmp);

    return res;
}


IPTableSeq
IPSorter::getIPTables(int univid)
{
    UnivIPTables::iterator it = univ2ips.find(univid);
    if(it != univ2ips.end()) return it ->second;
    else return IPTableSeq();
}

void
IPSorter::removeIpById(int id)
{
    IPTablePtr ipt = getIPTable(id);
    if(ipt == 0 ) return ;
    UnivInfoMap::iterator it = univMap.find(ipt->univ);
    MCE_DEBUG("remove: start="<<ipt->start<<"  end="<<ipt->end<< "  univ="<< it->first);
    if(it != univMap.end()) remove(ipt->start,ipt->end, it);
}

//-------------------------------




