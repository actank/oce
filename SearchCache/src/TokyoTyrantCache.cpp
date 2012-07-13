#include <tcrdb.h>
#include <ServiceI.h>
#include "TokyoTyrantCache.h"

using namespace xce::searchcache;
using namespace MyUtil;

#define TEST 0

TokyoTyrantCache::TokyoTyrantCache() : db_handle_(NULL), db_host_(), db_port_(0) {
#if TEST
	db_host_ = "127.0.0.1"; 
	db_port_ = 1981; 
#else
	ServiceI& service = ServiceI::instance();

	db_host_ = service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.TTHost", "127.0.0.1");
	db_port_ = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SearchCache.TTPort", 1981);
#endif
}

TokyoTyrantCache::~TokyoTyrantCache() {
	if (db_handle_ != NULL) {
		tcrdbdel(db_handle_);
		db_handle_ = NULL;
	}
}

bool TokyoTyrantCache::Initialize() {
    return Connect();
}

bool TokyoTyrantCache::Connect() {
	TCRDB *handle = tcrdbnew();
	int ecode = 0;

	if (handle == NULL) {
		MCE_FATAL("Cannot alloc database handle");
		return false;
	}

	tcrdbtune(handle, 0, RDBTRECON);

	MCE_FATAL("Connecting... " << db_host_ << ":" << db_port_);

	if (!tcrdbopen(handle, db_host_.c_str(), db_port_)) {
		ecode = tcrdbecode(handle);
		MCE_FATAL("open error: " << tcrdberrmsg(ecode));
		tcrdbdel(handle);
		return false;
	}

	if (db_handle_ != NULL) {
		tcrdbdel(db_handle_);
	}

	db_handle_ = handle;

    return true;
}

bool TokyoTyrantCache::Query(long key, SearchCacheData *data_ptr) {
	assert(db_handle_ != NULL); 

	char key_name[100];
	int key_size = 0;
	char *value_ptr = NULL;
	int value_size = 0;
	int ecode = 0;
	bool res = false;

	key_size = GenerateKeyName(key, key_name, sizeof(key_name));
	value_ptr = static_cast<char*>(tcrdbget(db_handle_, key_name, key_size, &value_size));
	if (value_ptr == NULL) {
		ecode = tcrdbecode(db_handle_);
		MCE_WARN("Cannot find data for " << key_name << ":" << tcrdberrmsg(ecode));
		return false;
	}

	res = TokyoTyrantCache::Unserialize(data_ptr, value_ptr, value_size);
	if (!res) {
		MCE_FATAL("unserialize failed");
	}

	tcfree(value_ptr);

	return res;
}

bool TokyoTyrantCache::Query(const vector<long> &keys, vector< boost::shared_ptr<SearchCacheData> > *data_array_ptr) {
	assert(db_handle_ != NULL);

	char key_name[100];
	const char *key_ptr = NULL;
	int key_size = 0;
	const char *value_ptr = NULL;
	int value_size = 0;
	bool res = false;
	int ecode = 0;

	boost::shared_ptr<SearchCacheData> data(new SearchCacheData);

	TCMAP *record_map = tcmapnew();
	if (record_map == NULL) {
		ecode = tcrdbecode(db_handle_);
		MCE_FATAL("Cannot alloc result map:" << tcrdberrmsg(ecode));
		goto _exit;
	}

	for (vector<long>::const_iterator key_iterator = keys.begin();
			key_iterator != keys.end(); ++key_iterator) {
		key_size = GenerateKeyName(*key_iterator, key_name, sizeof(key_name));
		tcmapput(record_map, key_name, key_size, key_name, key_size);
	}

	if (!tcrdbget3(db_handle_, record_map)) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("Cannot get data:" << tcrdberrmsg(ecode));
		goto _exit;
	}

	tcmapiterinit(record_map);
	while ((key_ptr = static_cast<const char*>(tcmapiternext(record_map, &key_size)))) {
		value_ptr = static_cast<const char*>(tcmapiterval(key_ptr, &value_size));
		if (!TokyoTyrantCache::Unserialize(data.get(), value_ptr, value_size)) {
			ecode = tcrdbecode(db_handle_);
			MCE_ERROR("TokyoTyrantCache::Unserialize() error:" << tcrdberrmsg(ecode));
		} else {
			data_array_ptr->push_back(data);
		}
	}

	res = true;

_exit:

	if (record_map != NULL) {
		tcmapdel(record_map);
	}
	return res;
}

bool TokyoTyrantCache::Insert(long key, const SearchCacheData &data) {
	assert(db_handle_ != NULL); 

	char key_name[100];
	int key_size = 0;

	char *value_ptr = NULL;
	int value_len = 0;
	int ecode = 0;

	boost::shared_ptr<BinString> binary_data = TokyoTyrantCache::Serialize(data);
	key_size = GenerateKeyName(key, key_name, sizeof(key_name));

	binary_data->getBuf(&value_ptr, &value_len);
	if (!tcrdbput(db_handle_, key_name, key_size, value_ptr, value_len)) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("tcrdbput() error:" << tcrdberrmsg(ecode));
		return false;
	}

    return true;
}

bool TokyoTyrantCache::Insert(const vector<SearchCacheData> &data_array) {
	assert(db_handle_ != NULL); 

	bool res = false;

	for (vector<SearchCacheData>::const_iterator data_iterator = data_array.begin();
			data_iterator != data_array.end(); ++data_iterator) {
		const SearchCacheData &data = *data_iterator;

		res = Insert(data._id, data);
		if (res == false) {
			MCE_ERROR("Insert data error for userid " << data._id);
		}
	}

    return true;
}

bool TokyoTyrantCache::Remove(long key) {
	assert(db_handle_ != NULL);

	char key_name[100];
	int key_size = 0;
	bool res = false;
	int ecode = 0;

	key_size = GenerateKeyName(key, key_name, sizeof(key_name));

	res = tcrdbout(db_handle_, key_name, key_size);
	if (!res) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("removing " << key << " failed:" << tcrdberrmsg(ecode));
	}

	return res;
}

bool TokyoTyrantCache::Remove(const vector<long> &keys) {
	assert(db_handle_ != NULL); 

	for (vector<long>::const_iterator keys_iterator = keys.begin();
			keys_iterator != keys.end(); ++keys_iterator) {
		if (!this->Remove(*keys_iterator)) {
			MCE_ERROR("removing " << (*keys_iterator) << " failed");
		}
	}

    return true;
}

void TokyoTyrantCache::Close() {
//	assert(db_handle_ != NULL); 

    if (db_handle_ != NULL)
    {
        tcrdbdel(db_handle_);
        db_handle_ = NULL;
    }
}

boost::shared_ptr<BinString> TokyoTyrantCache::Serialize(const SearchCacheData &data) {
    try {
        boost::shared_ptr<BinString> binString(new BinString());
        binString->add(data._id);
        binString->add(data._name);
        binString->add(data._signature);
        binString->add(data._star);
        binString->add(data._selected);
        binString->add(data._status);
        binString->add(data._tinyurl);
        binString->add(data._headurl);
        binString->add(data._browse_config);
        binString->add(data._status_config);
        binString->add(data._home_city);
        binString->add(data._home_province);

        binString->add((int)data._networks.size());
        for(vector<Network>::const_iterator it = data._networks.begin();
                it != data._networks.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).stage);
            binString->add((*it).name);
        }

        binString->add((int)data._regions.size());
        for(vector<Region>::const_iterator it = data._regions.begin();
                it != data._regions.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).city);
            binString->add((*it).province);
        }

        binString->add((int)data._workplaces.size());
        for(vector<Workplace>::const_iterator it = data._workplaces.begin();
                it != data._workplaces.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).name);
        }

        binString->add((int)data._universities.size());
        for(vector<University>::const_iterator it = data._universities.begin();
                it != data._universities.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).year);
            binString->add((*it).name);
            binString->add((*it).department);
        }

        binString->add((int)data._colleges.size());
        for(vector<College>::const_iterator it = data._colleges.begin();
                it != data._colleges.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).year);
            binString->add((*it).name);
            binString->add((*it).department);
        }

        binString->add((int)data._highSchools.size());
        for(vector<HighSchool>::const_iterator it = data._highSchools.begin();
                it != data._highSchools.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).year);
            binString->add((*it).name);
        }

        binString->add((int)data._juniorSchools.size());
        for(vector<JuniorSchool>::const_iterator it = data._juniorSchools.begin();
                it != data._juniorSchools.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).year);
            binString->add((*it).name);
        }

        binString->add((int)data._elementarySchools.size());
        for(vector<ElementarySchool>::const_iterator it = data._elementarySchools.begin();
                it != data._elementarySchools.end(); ++it) {
            binString->add((*it).id);
            binString->add((*it).year);
            binString->add((*it).name);
        }
        //添加总有效字节数
        binString->add(binString->getBufferLen());
        return binString;
    } catch (...) {
        boost::shared_ptr<BinString> binString((BinString*)0);
        MCE_WARN("serialize id =" << data._id << " failed! " );
        return binString;
    }
}

bool TokyoTyrantCache::Unserialize(SearchCacheData *data_ptr, const char *buf, int lens) {
    const char* p = buf;
    int total = 0; 

    //获得总的有效字节数
    total  = *(int*)(p+lens-sizeof(int));
    if (lens-sizeof(int) != total) {
        MCE_WARN("total lens is error! total=" << total << "lens = " << lens);
        return false;
    }

    if (total <= 0) return false;

    GET_INT_VALUE(data_ptr->_id); 
    GET_STRING_VALUE(data_ptr->_name); 
    GET_STRING_VALUE(data_ptr->_signature); 
    GET_INT_VALUE(data_ptr->_star); 
    GET_INT_VALUE(data_ptr->_selected); 
    GET_INT_VALUE(data_ptr->_status); 
    GET_STRING_VALUE(data_ptr->_tinyurl); 
    GET_STRING_VALUE(data_ptr->_headurl); 
    GET_INT_VALUE(data_ptr->_browse_config); 
    GET_INT_VALUE(data_ptr->_status_config); 
    GET_STRING_VALUE(data_ptr->_home_city); 
    GET_STRING_VALUE(data_ptr->_home_province); 

    int vector_size = 0;

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        Network network;
        GET_INT_VALUE(network.id);
        GET_INT_VALUE(network.stage);
        GET_STRING_VALUE(network.name);
        data_ptr->_networks.push_back(network);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        Region region;
        GET_INT_VALUE(region.id);
        GET_STRING_VALUE(region.city);
        GET_STRING_VALUE(region.province);
        data_ptr->_regions.push_back(region);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        Workplace workplace;
        GET_INT_VALUE(workplace.id);
        GET_STRING_VALUE(workplace.name);
        data_ptr->_workplaces.push_back(workplace);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        University university;
        GET_INT_VALUE(university.id);
        GET_INT_VALUE(university.year);
        GET_STRING_VALUE(university.name);
        GET_STRING_VALUE(university.department);
        data_ptr->_universities.push_back(university);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        College college;
        GET_INT_VALUE(college.id);
        GET_INT_VALUE(college.year);
        GET_STRING_VALUE(college.name);
        GET_STRING_VALUE(college.department);
        data_ptr->_colleges.push_back(college);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        HighSchool highSchool;
        GET_INT_VALUE(highSchool.id);
        GET_INT_VALUE(highSchool.year);
        GET_STRING_VALUE(highSchool.name);
        data_ptr->_highSchools.push_back(highSchool);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        JuniorSchool juniorSchool;
        GET_INT_VALUE(juniorSchool.id);
        GET_INT_VALUE(juniorSchool.year);
        GET_STRING_VALUE(juniorSchool.name);
        data_ptr->_juniorSchools.push_back(juniorSchool);
    }

    GET_INT_VALUE(vector_size);
    for(int i = 0; i<vector_size; ++i) {
        ElementarySchool elementarySchool;
        GET_INT_VALUE(elementarySchool.id);
        GET_INT_VALUE(elementarySchool.year);
        GET_STRING_VALUE(elementarySchool.name);
        data_ptr->_elementarySchools.push_back(elementarySchool);
    }
    return true;
}

int TokyoTyrantCache::GenerateKeyName(long userid, char *keybuf, int keybuflen)
{
	return snprintf(keybuf, keybuflen, "user_%ld", userid);
}

IDbResult* TokyoTyrantCache::Query() {
    TokyoTyrantResult *result = new TokyoTyrantResult;
    if (result->Initialize(this->db_host_.c_str(), this->db_port_)) {
		return result;
	} else {
		delete result;
		return NULL;
	}
}

TokyoTyrantResult::TokyoTyrantResult() : db_handle_(NULL) {
}

TokyoTyrantResult::~TokyoTyrantResult() {
	if (db_handle_ != NULL) {
		tcrdbdel(db_handle_);
		db_handle_ = NULL;
	}
}

bool TokyoTyrantResult::Initialize(const char *db_host, int db_port) {
	TCRDB *handle = tcrdbnew();
	int ecode = 0;

	if (handle == NULL) {
		MCE_FATAL("Cannot alloc database handle");
		return false;
	}

	tcrdbtune(handle, 0, RDBTRECON);

	if (!tcrdbopen(handle, db_host, db_port)) {
		ecode = tcrdbecode(handle);
		MCE_FATAL("open error: " << tcrdberrmsg(ecode));
		tcrdbdel(handle);
		return false;
	}

	if (!tcrdbiterinit(handle)) {
		ecode = tcrdbecode(handle);
		MCE_FATAL("open error: " << tcrdberrmsg(ecode));
		tcrdbdel(handle);
		return false;
	}

	db_handle_ = handle;

    return true;
}

SearchCacheData* TokyoTyrantResult::FetchObject() {
	assert(db_handle_ != NULL);

	SearchCacheData *data_ptr = NULL;

	char *key_ptr = NULL;
	int key_size = 0;
	char *value_ptr = NULL;
	int value_size = 0;
	bool res = false;
	int ecode = 0;

_next:
	key_ptr = static_cast<char*>(tcrdbiternext(db_handle_, &key_size));
	if (key_ptr == NULL) {
		return NULL;
	}

	value_ptr = static_cast<char*>(tcrdbget(db_handle_, key_ptr, key_size, &value_size));
	if (value_ptr == NULL) {
		ecode = tcrdbecode(db_handle_);
		MCE_WARN("Cannot find data for " << key_ptr << ":" << tcrdberrmsg(ecode));
        if (key_ptr != NULL) {
            tcfree(key_ptr);
        }
		goto _next;
	}

	data_ptr = new SearchCacheData();
	if (data_ptr == NULL) {
		MCE_FATAL("Cannot generate 1 SearchCacheData object");
		goto _exit;
	}

	res = TokyoTyrantCache::Unserialize(data_ptr, value_ptr, value_size);
	if (!res) {
		MCE_FATAL("unserialize failed" << key_ptr);

/*
		delete data_ptr;
		data_ptr = NULL;

		goto _exit;
 */
        data_ptr->_id = 0;
	}

_exit:

	if (key_ptr != NULL) {
		tcfree(key_ptr);
	}

	if (value_ptr != NULL) {
		tcfree(value_ptr);
	}

    return data_ptr;
}

#if TEST
#include "DatabaseCacheFactory.h"
#include "SearchCacheData.h"
#define PRINT_SEARCHCACHEDATA \
        std::cout << s._id << std::endl; \
        std::cout << s._name << std::endl; \
        std::cout << s._signature << std::endl; \
        std::cout << s._star << std::endl; \
        std::cout << s._selected << std::endl; \
        std::cout << s._status << std::endl; \
        std::cout << s._tinyurl << std::endl; \
        std::cout << s._headurl << std::endl; \
        std::cout << s._browse_config << std::endl; \
        std::cout << s._status_config << std::endl; \
        std::cout << s._home_city << std::endl; \
        std::cout << s._home_province << std::endl; \
        std::cout << "network" << std::endl; \
        for (int i=0; i< s._networks.size(); ++i) { \
            std::cout << "\t" << s._networks[i].id << std::endl; \
            std::cout << "\t" << s._networks[i].stage << std::endl; \
            std::cout << "\t" << s._networks[i].name<< std::endl; \
        } \
        std::cout << "regions" << std::endl; \
        for (int i=0; i< s._regions.size(); ++i) { \
            std::cout << "\t" << s._regions[i].id << std::endl; \
            std::cout << "\t" << s._regions[i].city<< std::endl; \
            std::cout << "\t" << s._regions[i].province<< std::endl; \
        } \
        std::cout << "workplace" << std::endl; \
        for (int i=0; i< s._workplaces.size(); ++i) { \
            std::cout << "\t" << s._workplaces[i].id << std::endl; \
            std::cout << "\t" << s._workplaces[i].name<< std::endl; \
        } \
        std::cout << "university" << std::endl; \
        for (int i=0; i< s._universities.size(); ++i) { \
            std::cout << "\t" << s._universities[i].id << std::endl; \
            std::cout << "\t" << s._universities[i].year << std::endl; \
            std::cout << "\t" << s._universities[i].name<< std::endl; \
            std::cout << "\t" << s._universities[i].department << std::endl; \
        } \
        std::cout << "college" << std::endl; \
        for (int i=0; i< s._colleges.size(); ++i) { \
            std::cout << "\t" << s._colleges[i].id << std::endl; \
            std::cout << "\t" << s._colleges[i].year << std::endl; \
            std::cout << "\t" << s._colleges[i].name<< std::endl; \
            std::cout << "\t" << s._colleges[i].department << std::endl; \
        } \
        std::cout << "highschool" << std::endl; \
        for (int i=0; i< s._highSchools.size(); ++i) { \
            std::cout << "\t" << s._highSchools[i].id << std::endl; \
            std::cout << "\t" << s._highSchools[i].year << std::endl; \
            std::cout << "\t" << s._highSchools[i].name<< std::endl; \
        } \
        std::cout << "juniorschool" << std::endl; \
        for (int i=0; i< s._juniorSchools.size(); ++i) { \
            std::cout << "\t" << s._juniorSchools[i].id << std::endl; \
            std::cout << "\t" << s._juniorSchools[i].year << std::endl; \
            std::cout << "\t" << s._juniorSchools[i].name<< std::endl; \
        } \
        std::cout << "elmentaryschool" << std::endl; \
        for (int i=0; i< s._elementarySchools.size(); ++i) { \
            std::cout << "\t" << s._elementarySchools[i].id << std::endl; \
            std::cout << "\t" << s._elementarySchools[i].year << std::endl; \
            std::cout << "\t" << s._elementarySchools[i].name<< std::endl; \
        } 

bool getUserFromDBCache(int id) {
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
    if (db_cache->Initialize() == false) {
		MCE_WARN("Db initialize failed");
        return false;
    }

    SearchCacheData s;
    if (db_cache->Query(id, &s)) {
        PRINT_SEARCHCACHEDATA
    } else {
        std::cout << "error userid = " << id << std::endl;
        return false;
    }

    db_cache->Close();

    return true;
}

bool Serialize(int id) {
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
    if (db_cache->Initialize() == false) {
		MCE_WARN("Db initialize failed");
        return false;
    }
    
    SearchCacheData scd;
    scd._id = id;
    scd._name = "邵长青";
    scd._signature = 1;
    scd._star = 2;
    scd._status = 3;
    scd._tinyurl = "tinyurl/hd11/20070914/00/11/tiny_7014g169.jpg";
    scd._headurl = "headurl/hd11/20070914/00/11/tiny_7014g169.jpg";
    scd._browse_config = 4;
    scd._status_config = 5;
    scd._home_city = "双鸭山";
    scd._home_province = "黑龙江";
    Network network;
    network.id = 1;
    network.stage = 1;
    network.name = "清华大学";
    scd._networks.push_back(network);
    Network network1;
    network1.id = 2;
    network1.stage = 2;
    network1.name = "北京大学";
    scd._networks.push_back(network1);

    Region region;
    region.id = 1;
    region.city = "北京";
    region.province = "北京";
    scd._regions.push_back(region);

    Region region1;
    region1.id = 2;
    region1.city = "上海";
    region1.province = "黑龙江";
    scd._regions.push_back(region1);

    Workplace workplace;
    workplace.id = 1;
    workplace.name = "静安中心";
    scd._workplaces.push_back(workplace);

    University univ;
    univ.id = 1001;
    univ.year = 1993;
    univ.name = "清华大学";
    univ.department = "计算机系";
    scd._universities.push_back(univ);

    University univ1;
    univ1.id = 1002;
    univ1.year = 1994;
    univ1.name = "北京大学";
    univ1.department = "哲学系";
    scd._universities.push_back(univ1);

    ElementarySchool elmentary;
    elmentary.id = 1;
    elmentary.year = 1982;
    elmentary.name = "北京市第一小学";
    scd._elementarySchools.push_back(elmentary);

    if (db_cache->Insert(id, scd)) {
    }
    else {
        return false;
    }

    SearchCacheData s;
    if (db_cache->Query(id, &s)) {
        PRINT_SEARCHCACHEDATA
    } else {
        std::cout << "error userid = " << id << std::endl;
        return false;
    }

    db_cache->Close();

    return true;
}

bool getSearchCache(int id, int argc, char**argv) {
    Ice::CommunicatorPtr ic;

    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectPrx base = ic->stringToProxy("M:tcp -p 9090");
        SearchCacheManagerPrx searchcache = SearchCacheManagerPrx::checkedCast(base);

        if (!searchcache)
            throw "Invalid Proxy";

        SearchCacheResult result = searchcache->getSearchCache(id);

        cout << SEARCH_CACHE_ID   << " : " << result.generels[SEARCH_CACHE_ID] << endl;
        cout << SEARCH_CACHE_STATUS << " : " << result.generels[SEARCH_CACHE_STATUS] << endl;
	    cout << SEARCH_CACHE_NAME << " : " << result.generels[SEARCH_CACHE_NAME] << endl;
        cout << SEARCH_CACHE_SIGNATURE << " : " << result.generels[SEARCH_CACHE_SIGNATURE] << endl;
        cout << SEARCH_CACHE_SELECTED << " : " << result.generels[SEARCH_CACHE_SELECTED] << endl;
        cout << SEARCH_CACHE_HEADURL << " : " << result.generels[SEARCH_CACHE_HEADURL] << endl;
        cout << SEARCH_CACHE_TINYURL << " : " << result.generels[SEARCH_CACHE_TINYURL] << endl;
        cout << SEARCH_CACHE_STAR << " : " << result.generels[SEARCH_CACHE_STAR] << endl;
        cout << SEARCH_CACHE_HOME_CITY   << " : " << result.generels[SEARCH_CACHE_HOME_CITY] << endl;
        cout << SEARCH_CACHE_HOME_PROVINCE << " : " << result.generels[SEARCH_CACHE_HOME_PROVINCE] << endl;
        cout << SEARCH_CACHE_STATUS_CONFIG << " : " << result.generels[SEARCH_CACHE_STATUS_CONFIG] << endl;
        cout << SEARCH_CACHE_BROWSE_CONFIG << " : " << result.generels[SEARCH_CACHE_BROWSE_CONFIG] << endl;
        
        cout << "Network" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_NETWORK].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_STAGE] << endl;
        }

        cout << "Elementary School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR] << endl;
        }

        cout << "Junior School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_YEAR] << endl;
        }

        cout << "High School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_YEAR] << endl;
        }

        cout << "Colege" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_COLLEGE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_DEPARTMENT] << endl;
        }

        cout << "University" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_UNIVERSITY].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_DEPARTMENT] << endl;
        }

        cout << "Workplace" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_WORKPLACE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_NAME] << endl;
        }

        cout << "Region" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_REGION].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_PROVINCE] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_CITY] << endl;
        }
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        return false;
    } catch (const char* msg) {
        cerr << msg << endl;
        return false;
    }

    if (ic)
        ic->destroy();

    return true;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " Serialize userid" << endl;
        cout << "Usage: " << argv[0] << " getUserFromDBCache userid" << endl;
        cout << "Usage: " << argv[0] << " getSearchCache userid" << endl;
        return 0;
    }

    Ice::LoggerPtr _logger = new IceLogger("./test_log", "INFO");
    std::string strSwitch = argv[1];
    std::string strId = argv[2];
    int id = boost::lexical_cast<int>(strId);

    if (strSwitch == "Serialize") {
        if (!Serialize(id))
            cerr << strSwitch << "is error" << endl;
    }
    else if (strSwitch == "getUserFromDBCache") {
        if (!getUserFromDBCache(id))
            cerr << strSwitch << "is error" << endl;
    }
    else if (strSwitch == "getSearchCache") {
        if (!getSearchCache(id, argc, argv))
            cerr << strSwitch << "is error" << endl;
    }

}
#endif
