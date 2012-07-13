#ifndef __CLIENT_INTERFACE_H__
#define __CLIENT_INTERFACE_H__

#include "ClusterStateSubscriber.h"
#include "ClusterClient.h"
#include "LogWrapper.h"
#include "XceAlert.h"

#include <IceLogger.h>

namespace xce{
namespace clusterstate{

using namespace std;
using namespace Ice;
using namespace xce::serverstate;

class ValidProxyMap : public IceUtil::Mutex {
public:
	ValidProxyMap() {
		srand(unsigned(time(0)));
	}

	ValidProxyMap(std::map<int, std::vector<Ice::ObjectPrx> > data):
       		_data(data)	{
	}

	void insert(int  mod, std::vector<Ice::ObjectPrx> prxs) {
		IceUtil::Mutex::Lock lock(*this);
		_data.insert( pair<int, vector<Ice::ObjectPrx> >(mod, prxs) );
	}

	void swap(std::map<int, std::vector<Ice::ObjectPrx> > tmp) {
		IceUtil::Mutex::Lock lock(*this);
		_data.swap(tmp);
	}

        std::vector<Ice::ObjectPrx> getList(int mod) {
		IceUtil::Mutex::Lock lock(*this);
		std::vector<Ice::ObjectPrx> vec;
                map<int, vector<Ice::ObjectPrx> >::iterator it = _data.find(mod);
                if ( it != _data.end() ) {
			vec = (*it).second;
                }
                return vec; 
	}	

	Ice::ObjectPrx getOne(int mod) {
		IceUtil::Mutex::Lock lock(*this);
		map<int, vector<Ice::ObjectPrx> >::iterator it = _data.find(mod);
		if ( it != _data.end() ) {
			if (  (*it).second.size() > 0 ) {
				return (*it).second.at(getRandom((*it).second.size()));
			}
			else {
				return NULL;
			}
		}
		else {
			return NULL;
		}	
	}

private:
	inline int getRandom(int total) {
                return rand()%total;
	}

	std::map<int, std::vector<Ice::ObjectPrx> > _data;
};

template<class T> class ClientInterface : ClusterClient {
public:
	/* @brief 	构造函数
	 * @param	ctr_endpoints	controller的endpoints
	 * @param	interval	注册时间间隔(秒)
	 * @param	timeout		ice连接的超时时间(毫秒)
	 */
	ClientInterface(string ctlEndpoints, int interval=120,  int timeout=300,const MyUtil::ChannelPtr& channel= new MyUtil::OceChannel,
					bool isMutilServices = false, const string& identifier = ""): 
							_ctlEndpoints(ctlEndpoints),
							_cluster(-1),
							_interval(interval),
							_timeout(timeout),
							_isMutilServices(isMutilServices),
							_serviceIdentifier(identifier),
              _channel(channel)
	{
	};

	void intialize() {
		_subscriber = new ClusterStateSubscriber(*this, _ctlEndpoints, _interval, _channel);
		MyUtil::ChannelPtr ocechannel = new MyUtil::OceChannel;
		Ice::CommunicatorPtr ic = ocechannel->getCommunicator();
		_alert = com::xiaonei::xce::XceAlertPrx::uncheckedCast(ic->stringToProxy("XA@XceAlert"));
		// _subscriber->init();
	};

	void intialize(int timeout) {
		if ( timeout > 0 ) {
			_timeout = timeout;
		}
		_subscriber = new ClusterStateSubscriber(*this, _ctlEndpoints, _interval);
		// _subscriber->init();
	};



	/* @brief	接口函数, 由注册到的CLusterStateSubscriber触发
	 * @param	clusterState	当前的服务状况
	 */
	void renew(const ServerStateMap& clusterState) {
	
		IceUtil::Mutex::Lock lock(_mutex);
		checkServerStateMap(clusterState);
		
		_clusterState = clusterState;
		
		if( _isMutilServices )
		{
			getServerStateFromMutil();
		}else
		{
			getServerStateFromSingle();
		}
	}


	std::map<int, std::vector<T> > getAvailableProxies() {		
		
		IceUtil::Mutex::Lock lock(_mutex);
	
		std::map<int, std::vector<T> > results;
		for (ServerStateMap::iterator it = _clusterState.begin(); 
				it != _clusterState.end(); ++it ) {
			int mod = (*it).first;
			std::vector<T> vecTmp;
			for (ServerStateSeq::iterator it2 = (*it).second.begin(); 
					it2 != (*it).second.end(); ++it2) {
				// if ( ((*it2)->prx != NULL) && ((*it2)->status == 1)) {
				// if ( (*it2)->prx != NULL) {
				if ( (*it2)->prx ) {

					vecTmp.push_back(T::uncheckedCast((*it2)->prx->ice_timeout(_timeout)));
				}
			}
			results.insert( pair<int, std::vector<T> >(mod, vecTmp) );
		}
		return results;
	}

	
	std::map<int, std::vector<T> > getAvailableProxiesOneway() { 
		
		IceUtil::Mutex::Lock lock(_mutex);
	
		std::map<int, std::vector<T> > results;
		for (ServerStateMap::iterator it = _clusterState.begin(); 
				it != _clusterState.end(); ++it ) {
			int mod = (*it).first;
			std::vector<T> vecTmp;
			for (ServerStateSeq::iterator it2 = (*it).second.begin(); 
				it2 != (*it).second.end(); ++it2) {
				// if ( ((*it2)->prx != NULL) && ((*it2)->status == 1)) {
				// if ( (*it2)->prx != NULL) {
				if ( (*it2)->prx ) {
					vecTmp.push_back(T::uncheckedCast((*it2)->prx->ice_timeout(_timeout)->ice_oneway()));
				}
			}
			results.insert( pair<int, std::vector<T> >(mod, vecTmp) );
		}
		return results;
	}

	std::vector<Ice::ObjectPrx> getObjectPrxList(int mod) {
		std::vector<Ice::ObjectPrx> vec;
		vec = _validProxys.getList(mod);
		if(vec.size()==0)
			return _validBackupProxys.getList(mod);
		return vec;
	}

	std::vector<Ice::ObjectPrx> getValidObjectPrxList(int mod){
		return _validProxys.getList(mod);
	}

	std::vector<Ice::ObjectPrx> getInvalidObjectPrxList(int mod){
		return _validBackupProxys.getList(mod);
	}
	
	Ice::ObjectPrx getObjectPrx(int mod) {
		Ice::ObjectPrx prx = _validProxys.getOne(mod);
		if(!prx)
			return _validBackupProxys.getOne(mod);	
		return prx;
	}

	T getProxy(long userid) {
		Ice::ObjectPrx object = getObjectPrx(userid % _cluster);
		if ( object ) {
			return T::uncheckedCast(object->ice_timeout(_timeout)->ice_twoway());
		} else {
			return NULL;
		}
	}

	vector<T> getProxySeq( long userid) {
		vector<T> vec;
		vector<Ice::ObjectPrx> vecPrx;
		vecPrx = getObjectPrxList(userid % _cluster);
		if(vecPrx.size()>0){
			for(vector<Ice::ObjectPrx>::const_iterator it = vecPrx.begin(); it!= vecPrx.end();it++){
				vec.push_back(T::uncheckedCast((*it)->ice_timeout(_timeout)->ice_twoway()));
			}	
		}
		return vec;
	}

	vector<T> getValidProxySeq( long userid) {
		vector<T> vec;
		vector<Ice::ObjectPrx> vecPrx;
		vecPrx = getValidObjectPrxList(userid % _cluster);
		if(vecPrx.size()>0){
			for(vector<Ice::ObjectPrx>::const_iterator it = vecPrx.begin(); it!= vecPrx.end();it++){
				vec.push_back(T::uncheckedCast((*it)->ice_timeout(_timeout)->ice_twoway()));
			}
		}
		return vec;
	}

	vector<T> getInvalidProxySeq( long userid) {
		vector<T> vec;
		vector<Ice::ObjectPrx> vecPrx;
		vecPrx = getInvalidObjectPrxList(userid % _cluster);
		if(vecPrx.size()>0){
			for(vector<Ice::ObjectPrx>::const_iterator it = vecPrx.begin(); it!= vecPrx.end();it++){
				vec.push_back(T::uncheckedCast((*it)->ice_timeout(_timeout)->ice_twoway()));
			}
		}
		return vec;
	}

	vector<T> getAllProxySeq(long userid) {
		vector<T> vecPrx = getValidProxySeq(userid);
		vector<T> vecPrxBackup = getInvalidProxySeq(userid);
		vecPrx.insert(vecPrx.end(), vecPrxBackup.begin(), vecPrxBackup.end());
		return vecPrx;
	}

	vector<T> getValidProxySeqOneway( long userid) {
		vector<T> vec;
		vector<Ice::ObjectPrx> vecPrx;
		vecPrx = getValidObjectPrxList(userid % _cluster);
		if(vecPrx.size()>0){
			for(vector<Ice::ObjectPrx>::const_iterator it = vecPrx.begin(); it!= vecPrx.end();it++){
				vec.push_back(T::uncheckedCast((*it)->ice_timeout(_timeout)->ice_oneway()));
			}
		}
		return vec;
	}

	vector<T> getInvalidProxySeqOneway( long userid) {
		vector<T> vec;
		vector<Ice::ObjectPrx> vecPrx;
		vecPrx = getInvalidObjectPrxList(userid % _cluster);
		if(vecPrx.size()>0){
			for(vector<Ice::ObjectPrx>::const_iterator it = vecPrx.begin(); it!= vecPrx.end();it++){
				vec.push_back(T::uncheckedCast((*it)->ice_timeout(_timeout)->ice_oneway()));
			}
		}
		return vec;
	}

	vector<T> getAllProxySeqOneway(long userid) {
		vector<T> vecPrx = getValidProxySeqOneway(userid);
		vector<T> vecPrxBackup = getInvalidProxySeqOneway(userid);
		vecPrx.insert(vecPrx.end(), vecPrxBackup.begin(), vecPrxBackup.end());
		return vecPrx;
	}
	
	T getProxyOneway(long userid) {
		//Ice::ObjectPrx object = _validProxys.getOne(userid % _cluster);
		Ice::ObjectPrx object = getObjectPrx(userid % _cluster);
		// if ( object != NULL ) {
		if ( object  ) {
			return T::uncheckedCast(object->ice_timeout(_timeout)->ice_oneway());
		} else {
			return NULL;
		}
	}
	
	T getProxyBatchOneway(long userid) {
		//Ice::ObjectPrx object = _validProxys.getOne(userid % _cluster);
		Ice::ObjectPrx object = getObjectPrx(userid % _cluster);
		// if ( object != NULL ) {
		if ( object ) {
			return T::uncheckedCast(object->ice_timeout(_timeout)->ice_batchOneway());
		} else {
			return NULL;
		}
	}
	
	T getProxyDatagram(long userid) {
		//Ice::ObjectPrx object = _validProxys.getOne(userid % _cluster);
		Ice::ObjectPrx object = getObjectPrx(userid % _cluster);
		// if ( object != NULL ) {
		if ( object ) {
			return T::uncheckedCast(object->ice_timeout(_timeout)->ice_datagram());
		} else {
			return NULL;
		}
	}

	int getCluster(){
		return _cluster;
	}

protected:

	void getServerStateFromSingle()
	{
		map<int, vector<Ice::ObjectPrx> > ssmTmp;
		map<int, vector<Ice::ObjectPrx> > ssmBackupTmp;

		for ( ServerStateMap::const_iterator it = _clusterState.begin(); 
			it != _clusterState.end(); ++it ) {

			int mod = (*it).first;
			vector<Ice::ObjectPrx> vecPrx;
			vector<Ice::ObjectPrx> vecBackupPrx;
			for ( ServerStateSeq::const_iterator it2 = (*it).second.begin();
					it2 != (*it).second.end(); ++it2 ) {

				if ( (*it2)->status == 1 ) {
					vecPrx.push_back((*it2)->prx);
					if ( (*it2)->cluster > 0 ) {
						_cluster = (*it2)->cluster;
					}
				}
				if ( (*it2)->status == 0 ) {
					vecBackupPrx.push_back((*it2)->prx);
					if ( (*it2)->cluster > 0 ) {
						_cluster = (*it2)->cluster;
					}
				}
			}
			ssmTmp.insert(pair<int, vector<Ice::ObjectPrx> >(mod, vecPrx));
			ssmBackupTmp.insert(pair<int, vector<Ice::ObjectPrx> >(mod, vecBackupPrx));
		}
		_validProxys.swap(ssmTmp);
		_validBackupProxys.swap(ssmBackupTmp);
	}

	void getServerStateFromMutil()
	{
		map<int, vector<Ice::ObjectPrx> > ssmTmp;
		map<int, vector<Ice::ObjectPrx> > ssmBackupTmp;

		for ( ServerStateMap::const_iterator it = _clusterState.begin(); 
			it != _clusterState.end(); ++it )
		{
			int mod = (*it).first;
			vector<Ice::ObjectPrx> vecPrx;
			vector<Ice::ObjectPrx> vecBackupPrx;
			for ( ServerStateSeq::const_iterator it2 = (*it).second.begin();
					it2 != (*it).second.end(); ++it2 ) 
			{
				
					ServerStateInfoMutilPtr ssimp = ServerStateInfoMutilPtr::dynamicCast(*it2);
		
					int serviceStatus = 0;
					
					ServerPrxStatusMap::const_iterator statusFindIt = ssimp->statusMap.find(_serviceIdentifier);
					if(statusFindIt != ssimp->statusMap.end())
					{
						serviceStatus = statusFindIt->second;
					}else
					{
						MCE_WARN("ClientInterface::getServerStateFromMutil serviceIdentifier = " << _serviceIdentifier << " there is no status of prx! ");
					}
				
					Ice::ObjectPrx servicePrx = NULL;
					
					ServerPrxMap::const_iterator prxFindIt = ssimp->prxMap.find(_serviceIdentifier);
					if(prxFindIt!= ssimp->prxMap.end())
					{
						servicePrx = prxFindIt->second;
					}else
					{
						MCE_WARN("ClientInterface::getServerStateFromMutil serviceIdentifier = " << _serviceIdentifier << " there is no prx! ");
						continue;
					}
					
					if ( serviceStatus == 1 ) 
					{
						vecPrx.push_back( servicePrx);
					}else
					{
						vecBackupPrx.push_back( servicePrx );
					}
				
					if ( ssimp->cluster > 0 )
					{
						_cluster = ssimp->cluster;
					}
				
			}
			ssmTmp.insert(pair<int, vector<Ice::ObjectPrx> >(mod, vecPrx));
			ssmBackupTmp.insert(pair<int, vector<Ice::ObjectPrx> >(mod, vecBackupPrx));

		}
		_validProxys.swap(ssmTmp);
		_validBackupProxys.swap(ssmBackupTmp);
	}

private:
	void checkServerStateMap(const ServerStateMap& clusterState) {
		ServerStateMap tempClusterState = _clusterState;
		for (ServerStateMap::iterator oi = tempClusterState.begin(); oi != tempClusterState.end(); ++oi) {
			ServerStateMap::const_iterator ni = clusterState.find(oi->first);
			if (ni == clusterState.end()) {
				for (ServerStateSeq::iterator oj = oi->second.begin(); oj != oi->second.end();) {
					try {
						(*oj)->prx->ice_ping();
					} catch (...) {
						ostringstream oss;
						oss << " for mod: " << oi->first << " for proxy: " << (*oj)->prx->ice_toString() << " old exist but new no";
						MCE_WARN("ClientInterface::checkServerStateMap -> for controller: " << _ctlEndpoints  << oss.str());
						oi->second.erase(oj);
						MyUtil::StrSeq strseq;
						strseq.push_back(_ctlEndpoints);
						try {
							_alert->notifyAlert(strseq, oss.str());
						} catch (const Ice::Exception &e) {
							MCE_WARN("ClientInterface::checkServerStateMap -> " << e);
						}
						continue;
					}
					++oj;
				}
			} else {
				for (ServerStateSeq::iterator oj = oi->second.begin(); oj != oi->second.end();) {
					ServerStateSeq::const_iterator nj = ni->second.begin();
					for (; nj != ni->second.end() && (*oj)->prx->ice_toString() != (*nj)->prx->ice_toString(); ++nj);
					if (nj == ni->second.end()) {
						try {
							(*oj)->prx->ice_ping();
						} catch (...) {
							ostringstream oss;
                                                	oss << " for mod: " << oi->first << " for proxy: " <<
(*oj)->prx->ice_toString() << " old exist but new no";
							MCE_WARN("ClientInterface::checkServerStateMap -> for controller: " << _ctlEndpoints << oss.str());
							oi->second.erase(oj);
							MyUtil::StrSeq strseq;
	                                                strseq.push_back(_ctlEndpoints);
							try {
								_alert->notifyAlert(strseq, oss.str());
							} catch (const Ice::Exception& e) {
								MCE_WARN("ClientInterface::checkServerStateMap -> " << e);
							}
							continue;
						}
					}
					++oj;
				}
			}
		}
		for (ServerStateMap::const_iterator ni = clusterState.begin(); ni != clusterState.end(); ++ni) {
			ServerStateMap::iterator oi = tempClusterState.find(ni->first);
			if (oi == tempClusterState.end()) {
				tempClusterState.insert(*ni);
				MCE_INFO("ClientInterface::checkServerStateMap -> for controller: "<< _ctlEndpoints<< " for mod: "<< ni->first << " new exist but old no");
			} else {
				for (ServerStateSeq::const_iterator nj = ni->second.begin(); nj != ni->second.end(); ++nj) {
					ServerStateSeq::iterator oj = oi->second.begin();
					for (; oj != oi->second.end() && (*oj)->prx->ice_toString() != (*nj)->prx->ice_toString(); ++oj);
					if (oj == oi->second.end()) {
						oi->second.push_back(*nj);
						MCE_INFO("ClientInterface::checkServerStateMap -> for controller: "<< _ctlEndpoints<<" for mod: "<< ni->first << " for proxy: " << (*nj)->prx->ice_toString() << " new exist but old no");
					}
				}
			}
		}
		//_clusterState = tempClusterState;
	}
	
	ClusterStateSubscriberPtr _subscriber;

	/* @brief 当前服务器状态信息表 */
	ServerStateMap _clusterState;
	
	/* @brief 可用的proxy组合 */
	ValidProxyMap _validProxys;	
	ValidProxyMap _validBackupProxys;
	/* @brief 当前选用的_proxyList */
	vector<T> _proxyList;
	string _ctlEndpoints;
	int _cluster;
	int _interval;
	int _timeout;

	com::xiaonei::xce::XceAlertPrx _alert;
	bool _isMutilServices;
	string _serviceIdentifier;

	IceUtil::Mutex _mutex;
  MyUtil::ChannelPtr _channel;
};
};
};

#endif
