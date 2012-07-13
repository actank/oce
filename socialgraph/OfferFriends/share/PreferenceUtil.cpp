#include "PreferenceUtil.h"
#include "ObjectCacheI.h"

using namespace std;
using namespace xce::offerfriends;
using namespace MyUtil;

//***************************************************************************

PreferenceData::PreferenceData(){
}

std::string PreferenceData::set(const std::map<int, int>& weights, 
																const std::map<int, int>& quotas) 
{
	ostringstream res;
	IceUtil::Mutex::Lock lock(*this);
	for (map<int,int>::const_iterator itWeight = weights.begin(); 
			itWeight != weights.end(); ++itWeight) {
		int quota_tmp;
		int weight_tmp;
		if (itWeight->second > 9) {
			weight_tmp = 90;
			quota_tmp = 9;
		} else if (itWeight->second >= 1) {
			weight_tmp = (itWeight->second) * 10;
			quota_tmp = itWeight->second;
		} else {
			continue;
		}
		_weights.insert(std::pair<int, int>(itWeight->first, weight_tmp));

		map<int,int>::const_iterator itQuota = quotas.find(itWeight->first);
		if(itQuota != quotas.end()){
			quota_tmp = itQuota->second;
		}
		_quotas.insert(std::pair<int, int>(itWeight->first, quota_tmp));

		res << "t" << itWeight->first << "-" << weight_tmp << "-" << quota_tmp << "/";
	}
	return res.str();
}

void PreferenceData::set(int type, int weight, int quota ) {
	IceUtil::Mutex::Lock lock(*this);
	int new_weight;
	if( weight > 9 ){
		new_weight = 90;
	} else if( weight >= 1 ){
		new_weight = weight*10;
	} else {
		return;
	}
	int new_quota;
	if( quota > 9 ){
		new_quota = 9;
	} else if( quota >= 1 ){
		new_quota = quota;
	} else {
		new_quota = weight;
	}

	map<int,int>::iterator itWeight = _weights.find( type );
	if( itWeight == _weights.end() ){
		_weights.insert(pair<int,int>( type, new_weight ));
	} else {
		itWeight->second =  new_weight;
	}

	map<int,int>::iterator itQuota = _quotas.find( type );
	if( itQuota == _quotas.end() ){
		_quotas.insert(pair<int,int>( type, new_quota ));
	} else {
		itQuota->second = new_quota;
	}
}

map<int,int> PreferenceData::getWeights() {
	return _weights;
}

multimap<int,int> PreferenceData::getQuotas(int total) {
	multimap<int,int> res;
	int quota_total = 0;
	for( map<int,int>::iterator it = _quotas.begin(); it != _quotas.end(); ++it ) {
		quota_total += it->second;
	}
	if( quota_total <= 0 ) return res;

	int quota_count = 0;
	for( map<int,int>::iterator it = _quotas.begin(); it != _quotas.end(); ++it ) {
		int quota = total*(it->second)/quota_total;
		quota_count += quota;
		res.insert( multimap<int,int>::value_type( quota, it->first ) );
	}

	if( quota_count < total ) {
		multimap<int,int>::iterator it = res.end();
		--it;
		int quota_max = it->first + total - quota_count;
		//res.erase((++rit).base());
		res.erase( it );
		res.insert( multimap<int,int>::value_type( quota_max, it->second ) );
	}
	return res;
}

map<int,int> PreferenceData::getQuotasFrom(int total,const map<int,multimap<int,OfferDataN> >& typedWeightMap) {
	MCE_INFO("[PreferenceData] getQuotasFrom total:" << total << " typedWeightMap size:" << typedWeightMap.size());
	map<int, int> res;
	map<int, int> quotas;
	map<int, int> sources;
	int quota_total = 0;
	for( map<int, int>::iterator it = _quotas.begin(); it != _quotas.end(); ++it ) {
		map<int, multimap<int, OfferDataN> >::const_iterator iter = typedWeightMap.find( it->first );
		if( iter != typedWeightMap.end() ){
			quotas[it->first] = it->second;
			sources[it->first] = iter->second.size();
			quota_total += it->second;
			res.insert( map<int,int>::value_type( it->first, 0 ) );
		}
	}
	//for (map<int, int>::iterator iter = quotas.begin();
	//		iter != quotas.end(); ++iter) {
	//	MCE_INFO("quotas " << iter->first << " : " << iter->second);
	//}
	//for (map<int, int>::iterator iter = sources.begin();
	//		iter != sources.end(); ++iter) {
	//	MCE_INFO("sources " << iter->first << " : " << iter->second);
	//}
	//for (map<int, int>::iterator iter = res.begin();
	//		iter != res.end(); ++iter) {
	//	MCE_INFO("res " << iter->first << " : " << iter->second);
	//}
	int bak_total = total;

	bool change = true;
	while( change ){
		if( quota_total<=0 || bak_total<=0 ){
			break;
		}
		change = false;
		//here: quota_total>0 && bak_total>0;
		int bak_tmp = bak_total;
		int quota_tmp = quota_total;
		for (map<int, int>::iterator it = quotas.begin(); it != quotas.end(); ++it) {
			int quota = bak_tmp*(it->second)/quota_tmp;//calc
			//MCE_INFO("quota : " << quota);
			if( quota <= 0 ){//little quota || distributed || no source;
				continue;
			}
			//else: to be distribute;
			int& source = sources[it->first];
			if( source <= 0 ){//but no sources
				quota_total -= it->second;//quota_total reduce;
				it->second = 0;//pass this type
				continue;
			}
			//here: quota>0 and source>0
			if( quota<=source ){
				res[it->first] += quota;
				bak_total -= quota;
				source -= quota;
				change = true;
			}else{//0<source<quota
				res[it->first] += source;
				bak_total -= source;
				source = 0;
				change = true;
				quota_total -= it->second;
				it->second = 0;
			}
		}
	}

	for (map<int, int>::iterator iter = res.begin();
			iter != res.end(); ++iter) {
		MCE_INFO("final res " << iter->first << " : " << iter->second);
	}
	return res;
}

string PreferenceData::toString() {
	ostringstream res;
	for( map<int,int>::iterator it = _weights.begin(); it != _weights.end(); ++it ) {
		res << "t" << it->first << "-" << it->second << "-" << _quotas[it->first] << "/";
	}
	return res.str();
}

//**************************************************************************************************************

PreferenceDataPtr PreferenceProducer::create(int userId, std::ostringstream& createPrefLog) {
	PreferenceDataPtr pref = new PreferenceData();

	map<int, int> base_weights;
	base_weights.insert(pair<int, int>(BaseTypeCommonFriend, 5));
	base_weights.insert(pair<int, int>(BaseTypeSameInfoWorkPlace, 5));
	base_weights.insert(pair<int, int>(BaseTypeSameInfoUniv, 6));
	base_weights.insert(pair<int, int>(BaseTypeSameInfoHighSchool, 6));
	base_weights.insert(pair<int, int>(BaseTypeSameInfoJuniorSchool, 5));
	base_weights.insert(pair<int, int>(BaseTypeSameInfoElementarySchool, 5));
	base_weights.insert(pair<int, int>(BaseTypeIPSame, 6));
	base_weights.insert(pair<int, int>(BaseTypeAreaEnrollSame, 6)); //为中学推人使用

	map<int,int> base_quotas;
	base_quotas.insert(pair<int, int>(BaseTypeCommonFriend, 6));
	base_quotas.insert(pair<int, int>(BaseTypeSameInfoWorkPlace, 4));
	base_quotas.insert(pair<int, int>(BaseTypeSameInfoUniv, 6));
	base_quotas.insert(pair<int, int>(BaseTypeSameInfoHighSchool, 6));
	base_quotas.insert(pair<int, int>(BaseTypeSameInfoJuniorSchool, 4));
	base_quotas.insert(pair<int, int>(BaseTypeSameInfoElementarySchool, 3));
	base_quotas.insert(pair<int, int>(BaseTypeIPSame, 1));
	base_quotas.insert(pair<int, int>(BaseTypeAreaEnrollSame, 6)); //为中学推人使用

	createPrefLog	<< " pref(" << pref->set(base_weights, base_quotas) << ")";
	return pref;
}

