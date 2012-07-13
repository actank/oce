#ifndef __TIPS_UTIL_H__
#define __TIPS_UTIL_H__

#include <string>
#include <vector>
#include <map>
#include "Tips.h"
#include "UtilCxx/src/Singleton.h"
#include "util/cpp/String.h"

namespace xce {
namespace tps {

const static int UK = 0;	// user click cache
const static int UI = 1;	// user info cache
const static int UA = 2;	// user app cache
const static int UC = 3;	// user complete cache

const static std::string DB_DESC_TIPS = "tips";
const static std::string TABLE_TIPS = "tips";

const static std::string FIELD_TIPSID = "tipsid";
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

const static std::string FIELD_CTX_TPSINTEREST = "tps_interest";
const int MAX_RESULT_LINES = 2000;

const static std::string DB_DESC_USERAPP = "tips_user_app";
const static std::string TABLE_USERAPP = "user_app_";
const static std::string FIELD_USERAPP_USERID = "user_id";
const static std::string FIELD_USERAPP_APPID = "app_id";

const static std::string DB_DESC_RECORD = "tips_record";
const static std::string TABLE_ACTION_RECORD = "record_";
const static std::string FIELD_ACTION_USERID = "userid";
const static std::string FIELD_ACTION_BULLTINID = "tipsid";

const static std::string DESC_TIPS_INTEREST = "tips_users";
const static std::string FIELD_TIPS_INTEREST = "userid";

const static int QUERY_FIELD_WEBSITE = 0;
const static int QUERY_FIELD_STAR = 1;
const static int QUERY_FIELD_VIP = 2;
const static int QUERY_FIELD_GENDER = 3;
const static int QUERY_FIELD_IDMODS = 4;
const static int QUERY_FIELD_STAGE = 5;

const static int CREATE_INDEX_INTERVAL = 600;
const static int MOD_DIV_DEFAULT = 10;
// const static int CREATE_INDEX_INTERVAL = 60;

const static std::string TIPS_SHOW_LOG_PATH = "SHOW";
const static std::string TIPS_CLICK_LOG_PATH = "CLICK";
const static std::string TIPS_CLOSE_LOG_PATH = "CLOSE";

using std::string;

class TipsUtil : public MyUtil::Singleton<TipsUtil> {
public:
	const vector<string>& getFieldList() {
		return fields_;
	}

	TipsPtr emptyTips() {
		TipsPtr tips = new Tips();
		tips->tipsid = -1;
		tips->title = "";
		tips->text = "";
		tips->website = -1;
		tips->star = -1;
		tips->vip = -1;
		tips->gender = -1;
		tips->weight = 0;
		tips->persist = false;
		tips->maxshow = -1;
		tips->online = false;

		return tips;
	}

	int getModDiv(const TipsPtr& tips) {		
		int moddiv = MOD_DIV_DEFAULT;
		if ( tips ) {
			if ( tips->ctxs.find(FIELD_CTX_MODDIV) != tips->ctxs.end()) {
				if ( tips->ctxs[FIELD_CTX_MODDIV].size() >= 1 ) {
					if (tips->ctxs[FIELD_CTX_MODDIV][0] > 0 ) {
						moddiv = tips->ctxs[FIELD_CTX_MODDIV][0];
					}
				}
			}
		}
		return moddiv;
	}

	void setModDiv(const TipsPtr& tips, int moddiv) {
		if ( ( ! tips ) || ( moddiv <= 0 ) ) {
			return;
		}
		if ( tips->ctxs.find(FIELD_CTX_MODDIV) == tips->ctxs.end() ) {
			vector<int> moddivs;
			moddivs.push_back(moddiv);
			tips->ctxs.insert( make_pair<string, vector<int> >(FIELD_CTX_MODDIV, moddivs));
		} else if ( tips->ctxs[FIELD_CTX_MODDIV].size() <= 0 ) {
			tips->ctxs[FIELD_CTX_MODDIV].push_back(moddiv);
		}
	}


	bool tips2Map(map<string, string>& mapTps, const TipsPtr tips) {
		mapTps.clear();
		if ( ! tips ) {
			return false;
		}
		// tipsid, title, text
		ostringstream tmpId;
		tmpId << tips->tipsid;
		// mapTps.insert( pair<string, string>(FIELD_TIPSID, tmpId.str()) ); // 写数据库是不写id的
		mapTps.insert( pair<string, string>(FIELD_TITLE, tips->title) );
		mapTps.insert( pair<string, string>(FIELD_TEXT, tips->text) );
		// website, star, vip, compare, gender
		ostringstream tmpWebsite, tmpStar, tmpVip, tmpGender;
		tmpWebsite << tips->website;
		mapTps.insert( pair<string, string>(FIELD_WEBSITE, tmpWebsite.str()) );
		tmpStar << tips->star;
		mapTps.insert( pair<string, string>(FIELD_STAR, tmpStar.str()) );
		tmpVip << tips->vip;
		mapTps.insert( pair<string, string>(FIELD_VIP, tmpVip.str()) );
		tmpGender << tips->gender;
		mapTps.insert( pair<string, string>(FIELD_GENDER, tmpGender.str()) );
		// ages, idmods, idgiven, onlinetime
		ostringstream tmpAges, tmpIdmods, tmpIdgiven, tmpOnlinetime;
		for ( size_t i = 0; i < tips->ages.size(); ++i ) {
			tmpAges << tips->ages[i]->minAge << "-" << tips->ages[i]->maxAge << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_AGES, tmpAges.str()) );
		// idmods
		for ( size_t i = 0; i < tips->idmods.size(); ++i ) {
			tmpIdmods << tips->idmods[i] << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_IDMODS, tmpIdmods.str()) );
		for ( size_t i = 0; i < tips->idgiven.size(); ++i ) {
			tmpIdgiven << tips->idgiven[i] << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_IDGIVEN, tmpIdgiven.str()) );
		for ( size_t i = 0; i < tips->onlinetime.size(); ++i ) {
			tmpOnlinetime << tips->onlinetime[i]->beginTime 
				<< "-" << tips->onlinetime[i]->endTime << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_ONLINETIME, tmpOnlinetime.str()) );
		// completes, uncompletes
		ostringstream tmpCompletes, tmpUncompletes;
		for ( size_t i = 0; i < tips->completes.size(); ++i ) {
			tmpCompletes << tips->completes[i] << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_COMPLETES, tmpCompletes.str()) );
		for ( size_t i = 0; i < tips->uncompletes.size(); ++i ) {
			tmpUncompletes << tips->uncompletes[i] << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_UNCOMPLETES, tmpUncompletes.str()) );
		// appuse, appunuse
		ostringstream tmpAppuse, tmpAppunuse;
		for ( size_t i = 0; i < tips->appuse.size(); ++i ) {
			tmpAppuse << tips->appuse[i] << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_APPUSE, tmpAppuse.str()) );
		for ( size_t i = 0; i < tips->appunuse.size(); ++i ) {
			tmpAppunuse << tips->appunuse[i] << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_APPUNUSE, tmpAppunuse.str()) );
		// weight, persist, maxshow, online
		ostringstream tmpWeight, tmpMaxShow;
		string strPersist, strOnline;
		tmpWeight << tips->weight;
		tmpMaxShow << tips->maxshow;
		if ( tips->weight < 0 ) {
			tips->weight = 0;
		}
		mapTps.insert( pair<string, string>(FIELD_WEIGHT, tmpWeight.str()) );
		strPersist = tips->persist? "1" : "0";
		mapTps.insert( pair<string, string>(FIELD_PERSIST, strPersist) );
		mapTps.insert( pair<string, string>(FIELD_MAXSHOW, tmpMaxShow.str()) );
		strOnline = tips->online ? "1" : "0";
		mapTps.insert( pair<string, string>(FIELD_ONLINE, strOnline) );
		// ctxs
		ostringstream tmpCtxs;
		for ( map<string, vector<int> >::iterator itCtx = tips->ctxs.begin(); 
				itCtx != tips->ctxs.end(); ++itCtx) {
			tmpCtxs << (*itCtx).first << ":";
			for ( vector<int>::iterator it = (*itCtx).second.begin(); it != (*itCtx).second.end();++it) {
				tmpCtxs << (*it) << ",";
			}
			tmpCtxs << ";";
		}
		mapTps.insert( pair<string, string>(FIELD_CTXS, tmpCtxs.str()) );

		return true;
	};

	TipsPtr map2Tips(const map<string, string>& mapTps) {
		TipsPtr tips = emptyTips();
		try {
			map<string, string>::const_iterator it;
			// tipsid, title, text
			it = mapTps.find(FIELD_TIPSID);
			if ( it != mapTps.end() ) {
				tips->tipsid = atoi((*it).second.c_str());			
			}
			it = mapTps.find(FIELD_TITLE);
			if ( it != mapTps.end() ) {
				tips->title = (*it).second;
			}
			it = mapTps.find(FIELD_TEXT);
			if ( it != mapTps.end() ) {
				tips->text = (*it).second;
			}
			// website, star, vip, complete, gender
			it = mapTps.find(FIELD_WEBSITE); 
			if ( it != mapTps.end() ) {
				tips->website = atoi((*it).second.c_str());	
			}
			it = mapTps.find(FIELD_STAR);
		        if ( it != mapTps.end() ) {
				tips->star = atoi((*it).second.c_str());
			}
			it = mapTps.find(FIELD_VIP);
		        if ( it != mapTps.end() ) {
				tips->vip = atoi((*it).second.c_str());
			}
			it = mapTps.find(FIELD_GENDER);
		        if ( it != mapTps.end() ) {
				tips->gender = atoi((*it).second.c_str());
			}
			// ages, idmods, idgiven, onlinetime
			it = mapTps.find(FIELD_AGES);
			if ( it != mapTps.end() ) {
				vector<string> vecAges = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecAges.size(); ++i ) {
					vector<string> vecSeg = MyUtil::StrUtil::split(vecAges[i], "-");
					if ( vecSeg.size() == 2 ) {
						AgeRangePtr ar = new AgeRange();
						ar->minAge = atoi(vecSeg[0].c_str());
						ar->maxAge = atoi(vecSeg[1].c_str());
						tips->ages.push_back(ar);
					} else if ( vecSeg.size() == 1) {
						AgeRangePtr ar = new AgeRange();
						ar->minAge = atoi(vecSeg[0].c_str());
						ar->maxAge = atoi(vecSeg[0].c_str());
						tips->ages.push_back(ar);
					}
				}
			}
			it = mapTps.find(FIELD_IDMODS);
			if ( it != mapTps.end() ) {
				vector<string> vecIdmods = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecIdmods.size(); ++i ) {
					if ( vecIdmods[i] != "" ) {
						tips->idmods.push_back(atoi(vecIdmods[i].c_str()));
					}
				}				
			}
			it = mapTps.find(FIELD_IDGIVEN);
			if ( it != mapTps.end() ) {
				vector<string> vecIdgiven = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecIdgiven.size(); ++i ) {
					tips->idgiven.push_back(atoi(vecIdgiven[i].c_str()));
				}
			}
			it = mapTps.find(FIELD_ONLINETIME);
			if ( it != mapTps.end() ) {
				vector<string> vecOnlinetime = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecOnlinetime.size(); ++i ) {
					vector<string> vecSeg = MyUtil::StrUtil::split(vecOnlinetime[i], "-");
					if ( vecSeg.size() == 2 ) {
						TimeRangePtr tr = new TimeRange();
						tr->beginTime = atoi(vecSeg[0].c_str());
						tr->endTime = atoi(vecSeg[1].c_str());
						tips->onlinetime.push_back(tr);
					}
				}
			}
			// completes, uncompletes
			it = mapTps.find(FIELD_COMPLETES);
			if ( it != mapTps.end() ) {
				vector<string> vecCompletes = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecCompletes.size(); ++i ) {
					if ( vecCompletes[i] != "" ) {
						tips->completes.push_back(atoi(vecCompletes[i].c_str()));
					}								
				}
			}
			it = mapTps.find(FIELD_UNCOMPLETES);
			if ( it != mapTps.end() ) {
				vector<string> vecUncompletes = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecUncompletes.size(); ++i ) {
					if ( vecUncompletes[i] != "" ) {
						tips->uncompletes.push_back(atoi(vecUncompletes[i].c_str()));
					}
				}
			}
			// appuse, appunuse
			it = mapTps.find(FIELD_APPUSE);
			if ( it != mapTps.end() ) {
				vector<string> vecAppuse = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecAppuse.size(); ++i ) {
					if ( vecAppuse[i] != "" ) {
						tips->appuse.push_back(atoi(vecAppuse[i].c_str()));
					}
				}
			}
			it = mapTps.find(FIELD_APPUNUSE);
			if ( it != mapTps.end() ) {
				vector<string> vecAppunuse = MyUtil::StrUtil::split((*it).second, ";");
				for ( size_t i = 0; i < vecAppunuse.size(); ++i ) {
					if ( vecAppunuse[i] != "" ) {
						tips->appunuse.push_back(atoi(vecAppunuse[i].c_str()));
					}
				}
			}
			// weight, persist, maxshow, online
			it = mapTps.find(FIELD_WEIGHT); 
			if ( it != mapTps.end() ) {
				tips->weight = atoi((*it).second.c_str());
				if ( tips->weight < 0 ) {
					tips->weight = 0;
				}
			}
			it = mapTps.find(FIELD_PERSIST); 
			if ( it != mapTps.end() ) {
				tips->persist = (atoi((*it).second.c_str()) > 0);
			}
			it = mapTps.find(FIELD_MAXSHOW);
			if ( it != mapTps.end() ) {
				tips->maxshow = (atoi((*it).second.c_str()));
			}
			it = mapTps.find(FIELD_ONLINE); 
			if ( it != mapTps.end() ) {
				tips->online = (atoi((*it).second.c_str()) > 0);
			}

			// ctxs
			it = mapTps.find(FIELD_CTXS);
			if ( it != mapTps.end() ) {
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
									MCE_WARN("[TipsUtil::map2Tips] transfer '" << cells[i] << " to int exception");
								}
							}
							if ( vec.size() > 0 ) {
								tips->ctxs.insert(make_pair<string, vector<int> >(ctxField, vec));
							}
						}
					}
				}			
			}

		} catch(...) {
			MCE_WARN("Tips " << tips->tipsid << " format is error, can't convert");
		}

		return tips;
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
	friend class MyUtil::Singleton<TipsUtil>; 
	TipsUtil() {
		// add fields_
		fields_.push_back(FIELD_TIPSID);
		fields_.push_back(FIELD_TITLE);
		fields_.push_back(FIELD_TEXT);
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
