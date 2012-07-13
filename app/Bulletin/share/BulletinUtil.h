#ifndef __BULLETIN_UTIL_H__
#define __BULLETIN_UTIL_H__

#include <string>
#include <vector>
#include <map>
#include "Bulletin.h"
#include "UtilCxx/src/Singleton.h"
#include "util/cpp/String.h"

namespace xce {
namespace blt {

const static int UK = 0;	// user click cache
const static int UI = 1;	// user info cache
const static int UA = 2;	// user app cache
const static int UC = 3;	// user complete cache

const static std::string DB_DESC_BULLETIN = "bulletin";
const static std::string TABLE_BULLETIN = "bulletin";

const static std::string FIELD_BULLETINID = "bulletinid";
const static std::string FIELD_TITLE = "title";
const static std::string FIELD_TEXT = "content";
const static std::string FIELD_WEBSITE = "website";
const static std::string FIELD_STAR = "star";
const static std::string FIELD_VIP = "vip";
const static std::string FIELD_COMPLETES = "completes";
const static std::string FIELD_UNCOMPLETES = "uncompletes";
const static std::string FIELD_GENDER = "gender";
const static std::string FIELD_AGES = "ages";
const static std::string FIELD_IDMODS = "idmods";
const static std::string FIELD_IDGIVEN = "idgiven";
const static std::string FIELD_ONLINETIME = "onlinetime";
const static std::string FIELD_APPUSE = "appuse";
const static std::string FIELD_APPUNUSE = "appnouse";
const static std::string FIELD_WEIGHT = "weight";
const static std::string FIELD_PERSIST = "persist";
const static std::string FIELD_MAXSHOW = "maxshow";
const static std::string FIELD_ONLINE = "online";
const static std::string FIELD_CTXS = "ctxs";
const static std::string FIELD_CTX_MODDIV = "moddiv";	// 对用户id取模时的除数

const static std::string FIELD_CTX_BLTINTEREST = "blt_interest";
const int MAX_RESULT_LINES = 2000;

const static std::string DB_DESC_USERAPP = "bulletin_user_app";
const static std::string TABLE_USERAPP = "user_app_";
const static std::string FIELD_USERAPP_USERID = "user_id";
const static std::string FIELD_USERAPP_APPID = "app_id";

const static std::string DB_DESC_RECORD = "bulletin_record";
const static std::string TABLE_ACTION_RECORD = "record_";
const static std::string FIELD_ACTION_USERID = "userid";
const static std::string FIELD_ACTION_BULLTINID = "bulletinid";

const static std::string DESC_BULLETIN_INTEREST = "bulletin_users";
const static std::string FIELD_BULLETIN_INTEREST = "userid";

const static int QUERY_FIELD_WEBSITE = 0;
const static int QUERY_FIELD_STAR = 1;
const static int QUERY_FIELD_VIP = 2;
const static int QUERY_FIELD_GENDER = 3;
const static int QUERY_FIELD_IDMODS = 4;
const static int QUERY_FIELD_STAGE = 5;

const static int CREATE_INDEX_INTERVAL = 600;
const static int MOD_DIV_DEFAULT = 10;
// const static int CREATE_INDEX_INTERVAL = 60;

const static std::string BULLETIN_SHOW_LOG_PATH = "SHOW";
const static std::string BULLETIN_CLICK_LOG_PATH = "CLICK";
const static std::string BULLETIN_CLOSE_LOG_PATH = "CLOSE";

using std::string;

class BulletinUtil : public MyUtil::Singleton<BulletinUtil> {
public:
	const vector<string>& getFieldList() {
		return fields_;
	}

	BulletinPtr emptyBulletin() {
		BulletinPtr bulletin = new Bulletin();
		bulletin->bulletinid = -1;
		bulletin->title = "";
		bulletin->text = "";
		bulletin->website = -1;
		bulletin->star = -1;
		bulletin->vip = -1;
		bulletin->gender = -1;
		bulletin->weight = 0;
		bulletin->persist = false;
		bulletin->maxshow = -1;
		bulletin->online = false;

		return bulletin;
	}

	int getModDiv(const BulletinPtr& bulletin) {		
		int moddiv = MOD_DIV_DEFAULT;
		if ( bulletin ) {
			if ( bulletin->ctxs.find(FIELD_CTX_MODDIV) != bulletin->ctxs.end()) {
				if ( bulletin->ctxs[FIELD_CTX_MODDIV].size() >= 1 ) {
					if (bulletin->ctxs[FIELD_CTX_MODDIV][0] > 0 ) {
						moddiv = bulletin->ctxs[FIELD_CTX_MODDIV][0];
					}
				}
			}
		}
		return moddiv;
	}

	void setModDiv(const BulletinPtr& bulletin, int moddiv) {
		if ( ( ! bulletin ) || ( moddiv <= 0 ) ) {
			return;
		}
		if ( bulletin->ctxs.find(FIELD_CTX_MODDIV) == bulletin->ctxs.end() ) {
			vector<int> moddivs;
			moddivs.push_back(moddiv);
			bulletin->ctxs.insert( make_pair<string, vector<int> >(FIELD_CTX_MODDIV, moddivs));
		} else if ( bulletin->ctxs[FIELD_CTX_MODDIV].size() <= 0 ) {
			bulletin->ctxs[FIELD_CTX_MODDIV].push_back(moddiv);
		}
	}


	bool bulletin2Map(map<string, string>& mapBlt, const BulletinPtr bulletin) {
		mapBlt.clear();
		if ( ! bulletin ) {
			return false;
		}
		// bulletinid, title, text
		ostringstream tmpId;
		tmpId << bulletin->bulletinid;
		// mapBlt.insert( pair<string, string>(FIELD_BULLETINID, tmpId.str()) ); // 写数据库是不写id的
		mapBlt.insert( pair<string, string>(FIELD_TITLE, bulletin->title) );
		mapBlt.insert( pair<string, string>(FIELD_TEXT, bulletin->text) );
		// website, star, vip, compare, gender
		ostringstream tmpWebsite, tmpStar, tmpVip, tmpGender;
		tmpWebsite << bulletin->website;
		mapBlt.insert( pair<string, string>(FIELD_WEBSITE, tmpWebsite.str()) );
		tmpStar << bulletin->star;
		mapBlt.insert( pair<string, string>(FIELD_STAR, tmpStar.str()) );
		tmpVip << bulletin->vip;
		mapBlt.insert( pair<string, string>(FIELD_VIP, tmpVip.str()) );
		tmpGender << bulletin->gender;
		mapBlt.insert( pair<string, string>(FIELD_GENDER, tmpGender.str()) );
		// ages, idmods, idgiven, onlinetime
		ostringstream tmpAges, tmpIdmods, tmpIdgiven, tmpOnlinetime;
		for ( size_t i = 0; i < bulletin->ages.size(); ++i ) {
			tmpAges << bulletin->ages[i]->minAge << "-" << bulletin->ages[i]->maxAge << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_AGES, tmpAges.str()) );
		// idmods
		for ( size_t i = 0; i < bulletin->idmods.size(); ++i ) {
			tmpIdmods << bulletin->idmods[i] << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_IDMODS, tmpIdmods.str()) );
		for ( size_t i = 0; i < bulletin->idgiven.size(); ++i ) {
			tmpIdgiven << bulletin->idgiven[i] << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_IDGIVEN, tmpIdgiven.str()) );
		for ( size_t i = 0; i < bulletin->onlinetime.size(); ++i ) {
			tmpOnlinetime << bulletin->onlinetime[i]->beginTime 
				<< "-" << bulletin->onlinetime[i]->endTime << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_ONLINETIME, tmpOnlinetime.str()) );
		// completes, uncompletes
		ostringstream tmpCompletes, tmpUncompletes;
		for ( size_t i = 0; i < bulletin->completes.size(); ++i ) {
			tmpCompletes << bulletin->completes[i] << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_COMPLETES, tmpCompletes.str()) );
		for ( size_t i = 0; i < bulletin->uncompletes.size(); ++i ) {
			tmpUncompletes << bulletin->uncompletes[i] << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_UNCOMPLETES, tmpUncompletes.str()) );
		// appuse, appunuse
		ostringstream tmpAppuse, tmpAppunuse;
		for ( size_t i = 0; i < bulletin->appuse.size(); ++i ) {
			tmpAppuse << bulletin->appuse[i] << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_APPUSE, tmpAppuse.str()) );
		for ( size_t i = 0; i < bulletin->appunuse.size(); ++i ) {
			tmpAppunuse << bulletin->appunuse[i] << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_APPUNUSE, tmpAppunuse.str()) );
		// weight, persist, maxshow, online
		ostringstream tmpWeight, tmpMaxShow;
		string strPersist, strOnline;
		tmpWeight << bulletin->weight;
		tmpMaxShow << bulletin->maxshow;
		if ( bulletin->weight < 0 ) {
			bulletin->weight = 0;
		}
		mapBlt.insert( pair<string, string>(FIELD_WEIGHT, tmpWeight.str()) );
		strPersist = bulletin->persist? "1" : "0";
		mapBlt.insert( pair<string, string>(FIELD_PERSIST, strPersist) );
		mapBlt.insert( pair<string, string>(FIELD_MAXSHOW, tmpMaxShow.str()) );
		strOnline = bulletin->online ? "1" : "0";
		mapBlt.insert( pair<string, string>(FIELD_ONLINE, strOnline) );
		// ctxs
		ostringstream tmpCtxs;
		for ( map<string, vector<int> >::iterator itCtx = bulletin->ctxs.begin(); 
				itCtx != bulletin->ctxs.end(); ++itCtx) {
			tmpCtxs << (*itCtx).first << ":";
			for ( vector<int>::iterator it = (*itCtx).second.begin(); it != (*itCtx).second.end();++it) {
				tmpCtxs << (*it) << ",";
			}
			tmpCtxs << ";";
		}
		mapBlt.insert( pair<string, string>(FIELD_CTXS, tmpCtxs.str()) );

		return true;
	};

	BulletinPtr map2Bulletin(const map<string, string>& mapBlt) {
		BulletinPtr bulletin = emptyBulletin();
		try {
			map<string, string>::const_iterator it;
			// bulletinid, title, text
			it = mapBlt.find(FIELD_BULLETINID);
			if ( it != mapBlt.end() ) {
				bulletin->bulletinid = atoi((*it).second.c_str());			
			}
			it = mapBlt.find(FIELD_TITLE);
			if ( it != mapBlt.end() ) {
				bulletin->title = (*it).second;
			}
			it = mapBlt.find(FIELD_TEXT);
			if ( it != mapBlt.end() ) {
				bulletin->text = (*it).second;
			}
			// website, star, vip, complete, gender
			it = mapBlt.find(FIELD_WEBSITE); 
			if ( it != mapBlt.end() ) {
				bulletin->website = atoi((*it).second.c_str());	
			}
			it = mapBlt.find(FIELD_STAR);
		        if ( it != mapBlt.end() ) {
				bulletin->star = atoi((*it).second.c_str());
			}
			it = mapBlt.find(FIELD_VIP);
		        if ( it != mapBlt.end() ) {
				bulletin->vip = atoi((*it).second.c_str());
			}
			it = mapBlt.find(FIELD_GENDER);
		        if ( it != mapBlt.end() ) {
				bulletin->gender = atoi((*it).second.c_str());
			}
			// ages, idmods, idgiven, onlinetime
			it = mapBlt.find(FIELD_AGES);
			if ( it != mapBlt.end() ) {
				vector<string> vecAges = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecAges.size(); ++i ) {
					vector<string> vecSeg = MyUtil::StrUtil::split(vecAges[i], "-");
					if ( vecSeg.size() == 2 ) {
						AgeRangePtr ar = new AgeRange();
						ar->minAge = atoi(vecSeg[0].c_str());
						ar->maxAge = atoi(vecSeg[1].c_str());
						bulletin->ages.push_back(ar);
					} else if ( vecSeg.size() == 1) {
						AgeRangePtr ar = new AgeRange();
						ar->minAge = atoi(vecSeg[0].c_str());
						ar->maxAge = atoi(vecSeg[0].c_str());
						bulletin->ages.push_back(ar);
					}
				}
			}
			it = mapBlt.find(FIELD_IDMODS);
			if ( it != mapBlt.end() ) {
				vector<string> vecIdmods = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecIdmods.size(); ++i ) {
					if ( vecIdmods[i] != "" ) {
						bulletin->idmods.push_back(atoi(vecIdmods[i].c_str()));
					}
				}				
			}
			it = mapBlt.find(FIELD_IDGIVEN);
			if ( it != mapBlt.end() ) {
				vector<string> vecIdgiven = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecIdgiven.size(); ++i ) {
					bulletin->idgiven.push_back(atoi(vecIdgiven[i].c_str()));
				}
			}
			it = mapBlt.find(FIELD_ONLINETIME);
			if ( it != mapBlt.end() ) {
				vector<string> vecOnlinetime = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecOnlinetime.size(); ++i ) {
					vector<string> vecSeg = MyUtil::StrUtil::split(vecOnlinetime[i], "-");
					if ( vecSeg.size() == 2 ) {
						TimeRangePtr tr = new TimeRange();
						tr->beginTime = atoi(vecSeg[0].c_str());
						tr->endTime = atoi(vecSeg[1].c_str());
						bulletin->onlinetime.push_back(tr);
					}
				}
			}
			// completes, uncompletes
			it = mapBlt.find(FIELD_COMPLETES);
			if ( it != mapBlt.end() ) {
				vector<string> vecCompletes = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecCompletes.size(); ++i ) {
					if ( vecCompletes[i] != "" ) {
						bulletin->completes.push_back(atoi(vecCompletes[i].c_str()));
					}								
				}
			}
			it = mapBlt.find(FIELD_UNCOMPLETES);
			if ( it != mapBlt.end() ) {
				vector<string> vecUncompletes = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecUncompletes.size(); ++i ) {
					if ( vecUncompletes[i] != "" ) {
						bulletin->uncompletes.push_back(atoi(vecUncompletes[i].c_str()));
					}
				}
			}
			// appuse, appunuse
			it = mapBlt.find(FIELD_APPUSE);
			if ( it != mapBlt.end() ) {
				vector<string> vecAppuse = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecAppuse.size(); ++i ) {
					if ( vecAppuse[i] != "" ) {
						bulletin->appuse.push_back(atoi(vecAppuse[i].c_str()));
					}
				}
			}
			it = mapBlt.find(FIELD_APPUNUSE);
			if ( it != mapBlt.end() ) {
				vector<string> vecAppunuse = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecAppunuse.size(); ++i ) {
					if ( vecAppunuse[i] != "" ) {
						bulletin->appunuse.push_back(atoi(vecAppunuse[i].c_str()));
					}
				}
			}
			// weight, persist, maxshow, online
			it = mapBlt.find(FIELD_WEIGHT); 
			if ( it != mapBlt.end() ) {
				bulletin->weight = atoi((*it).second.c_str());
				if ( bulletin->weight < 0 ) {
					bulletin->weight = 0;
				}
			}
			it = mapBlt.find(FIELD_PERSIST); 
			if ( it != mapBlt.end() ) {
				bulletin->persist = (atoi((*it).second.c_str()) > 0);
			}
			it = mapBlt.find(FIELD_MAXSHOW);
			if ( it != mapBlt.end() ) {
				bulletin->maxshow = (atoi((*it).second.c_str()));
			}
			it = mapBlt.find(FIELD_ONLINE); 
			if ( it != mapBlt.end() ) {
				bulletin->online = (atoi((*it).second.c_str()) > 0);
			}

			// ctxs
			it = mapBlt.find(FIELD_CTXS);
			if ( it != mapBlt.end() ) {
				vector<string> vecCtxs = MyUtil::StrUtil::split((*it).second, ";");
				for ( vector<string>::iterator itCtx = vecCtxs.begin();itCtx != vecCtxs.end();++itCtx ) {
					// 每个ctx项
					if ( (*itCtx) != "" ) {
						vector<string> fls = MyUtil::StrUtil::split((*itCtx), ":");
						if ( ( fls.size() >= 2 ) && ( fls[0] != "") ) {
							string ctxField = fls[0];
							vector<string> cells = MyUtil::StrUtil::split(fls[1], ",");
							vector<int> vec;
							for (size_t i = 0; i < cells.size(); ++i) {
								try {
									int num = atoi(cells[i].c_str());
									vec.push_back(num);
								} catch (...) {
									MCE_WARN("[BulletinUtil::map2Bulletin] transfer '" << cells[i] << " to int exception");
								}
							}
							if ( vec.size() > 0 ) {
								bulletin->ctxs.insert(make_pair<string, vector<int> >(ctxField, vec));
							}
						}
					}
				}			
			}

		} catch(...) {
			MCE_WARN("Bulletin " << bulletin->bulletinid << " format is error, can't convert");
		}

		return bulletin;
	};

	void insertMapVec(map<int, vector<int> >& mapVec, int key, int value) {
		map<int, vector<int> >::iterator it = mapVec.find(key);
		if ( it != mapVec.end() ) {
			(*it).second.push_back(value);
		} else {
			vector<int> vec;
			vec.push_back(value);
			mapVec.insert( pair<int, vector<int> >(key, vec) );
		}
	};

	/* @brief 有序集合求交 
	 * @param vecs	多个有序序列
	 */
	void merge_intersection(vector<int>& result, const vector< vector<int> >& vecs) {
		
		result.clear();
		size_t seqnum = vecs.size(); 

		// 如果有成员为0, 则直接返回
		if ( seqnum <= 0 ) {
			return;
		}
		for ( size_t i = 0; i < seqnum; ++i) {
			if ( vecs[i].size() <= 0 ) {
				return;
			}				
		}

		// 多路归并求交
		vector<int> poses(seqnum, 0);
		while ( true ) {
			int maxOne = -1;	// 当前各录头中最大之值
			size_t maxSame = 0;	// 和最大值相同的数量
			bool reachend = false;
			for ( size_t i = 0; i < seqnum; ++i ) {
				if ( vecs[i][poses[i]] > maxOne ) {
					maxOne = vecs[i][poses[i]];
				}
			}
			for ( size_t i = 0; i < seqnum; ++i ) {
				while (true) {
					if ( poses[i] >= int(vecs[i].size()) ) {
						reachend = true;
						break;
					} else if ( vecs[i][poses[i]] > maxOne ) {
						break;
					} else if ( vecs[i][poses[i]] == maxOne ) {
						++maxSame;
						++poses[i];
					} else {
						++poses[i];
					}
				}
			}
			if ( maxSame == seqnum ) {
				result.push_back(maxOne);
			}
			if ( reachend ) {
				break;
			}
		}
	};

	void merge_union(set<int>& result, const vector< vector<int> >& vecs) {

		result.clear();
		for ( size_t i = 0; i < vecs.size(); ++i ) {
			for (size_t j = 0; j < vecs[i].size(); ++j ) {
				result.insert(vecs[i][j]);
			}
		}	
	};

	void merge_union(vector<int>& result, const vector<int>& vec1, const vector<int>& vec2) {

		result.clear();
		vector<int>::const_iterator it1 = vec1.begin();
		vector<int>::const_iterator it2 = vec2.begin();
		while ( (it1 != vec1.end()) && (it2 != vec2.end()) ) {
			if ( (*it1) == (*it2) ) {
				result.push_back(*it1);
				++it1;
				++it2;
			} else if ( (*it1) < (*it2) ) {
				result.push_back(*it1);
				++it1;
			} else {
				result.push_back(*it2);
				++it2;
			}
		}

		if ( it1 == vec1.end() ) {
			result.insert(result.end(), it2, vec2.end());
		} else {
			result.insert(result.end(), it1, vec1.end());
		}
	};

private:
	friend class MyUtil::Singleton<BulletinUtil>; 
	BulletinUtil() {
		// add fields_
		fields_.push_back(FIELD_BULLETINID);
		fields_.push_back(FIELD_TITLE);
		fields_.push_back(FIELD_TEXT);
		fields_.push_back(FIELD_WEBSITE);
		fields_.push_back(FIELD_STAR);
		fields_.push_back(FIELD_VIP);
		fields_.push_back(FIELD_COMPLETES);
		fields_.push_back(FIELD_UNCOMPLETES);
		fields_.push_back(FIELD_GENDER);
		fields_.push_back(FIELD_AGES);
		fields_.push_back(FIELD_IDMODS);
		fields_.push_back(FIELD_IDGIVEN);
		fields_.push_back(FIELD_ONLINETIME);
		fields_.push_back(FIELD_APPUSE);
		fields_.push_back(FIELD_APPUNUSE);
		fields_.push_back(FIELD_WEIGHT);
		fields_.push_back(FIELD_PERSIST);
		fields_.push_back(FIELD_MAXSHOW);
		fields_.push_back(FIELD_CTXS);
		fields_.push_back(FIELD_ONLINE);
	};

	std::vector<std::string> fields_;
};


};
};













#endif
