#include <tcrdb.h>
#include <ServiceI.h>
#include "TokyoTyrantCache.h"

using namespace xce::searchcache;
using namespace MyUtil;

#define TEST 0

TokyoTyrantCache::TokyoTyrantCache(std::string dsn) : db_handle_(NULL), 
    db_host_(), db_port_(0), dsn_(dsn) {
#if TEST
	db_host_ = "127.0.0.1"; 
	db_port_ = 1981;//原来线上1981端口 
#else
/*
	db_host_ = service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.TTHost", "127.0.0.1");
	db_port_ = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SearchCache.TTPort", 1981);
 */
#endif
}

TokyoTyrantCache::~TokyoTyrantCache() {
	if (db_handle_ != NULL) {
		tcrdbdel(db_handle_);
		db_handle_ = NULL;
	}
}

bool TokyoTyrantCache::Initialize() {
    string::size_type start = dsn_.find("//", 0);

    if (start == string::npos) {
		MCE_FATAL("data source name error!");
        return false;
    }

    string protocol = dsn_.substr(0, start+2);

    if (protocol != TOKYOTYRANT_DSN_PREFIX) {
            MCE_FATAL("data source name error!");
            return false;
    }

    string::size_type end = dsn_.find(":", start+2);

    if (end == string::npos) {
        MCE_FATAL("data source name error!");
        return false;
    }

    db_host_ = dsn_.substr(start+2, end-start-2);
    start = end + 1;

    if (start >= dsn_.length()) {
        MCE_FATAL("data source name error!");
        return false;
    }

    db_port_ = boost::lexical_cast<int>(dsn_.substr(start, dsn_.length() - start));

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

	MCE_FATAL("Connecting... " << handle << " " << db_host_ << ":" << db_port_);

	if (!tcrdbopen(handle, db_host_.c_str(), db_port_)) {
		ecode = tcrdbecode(handle);
		MCE_FATAL("open error: " << tcrdberrmsg(ecode));
		tcrdbclose(handle);
		return false;
	}

	if (db_handle_ != NULL) {
		tcrdbdel(db_handle_);
	}

	db_handle_ = handle;

    return true;
}

bool TokyoTyrantCache::Query(long key, SearchMemCacheData *data_ptr) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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

bool TokyoTyrantCache::Query(int key, SearchMemCacheData *data_ptr) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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


bool TokyoTyrantCache::Query(long key, SearchCacheData *data_ptr) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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

bool TokyoTyrantCache::Query(int key, SearchCacheData *data_ptr) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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


bool TokyoTyrantCache::Query(const vector<long> &keys, vector< SearchCacheDataPtr > *data_array_ptr) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

	char key_name[100];
	const char *key_ptr = NULL;
	int key_size = 0;
	const char *value_ptr = NULL;
	int value_size = 0;
	bool res = false;
	int ecode = 0;


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
    SearchCacheDataPtr data(new SearchCacheData);
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

bool TokyoTyrantCache::Query(const vector<int> &keys, vector< SearchCacheDataPtr > *data_array_ptr) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

	char key_name[100];
	const char *key_ptr = NULL;
	int key_size = 0;
	const char *value_ptr = NULL;
	int value_size = 0;
	bool res = false;
	int ecode = 0;


	TCMAP *record_map = tcmapnew();
	if (record_map == NULL) {
		ecode = tcrdbecode(db_handle_);
		MCE_FATAL("Cannot alloc result map:" << tcrdberrmsg(ecode));
		goto _exit;
	}

	for (vector<int>::const_iterator key_iterator = keys.begin();
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
    SearchCacheDataPtr data(new SearchCacheData);
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


bool TokyoTyrantCache::Query(std::string key, std::vector<int>& vec) {
    //assert(db_handle_ != NULL);                                                                                                             
    MCE_DEBUG("[DEBUG] stepping into TotyoTyantCache::Query, key is " + key);                               
    if (db_handle_ == NULL) return false;                                                                                                         

    char *value_ptr = NULL;                                                                        
    int value_size = 0;                                                                      
    int ecode = 0;                                                                                               

    value_ptr = static_cast<char*>(tcrdbget(db_handle_, key.c_str(), key.length(), &value_size));                                      
    if (value_ptr == NULL) {                                                                                         
        ecode = tcrdbecode(db_handle_);                                                                                                           
        MCE_WARN("Cannot find data for " << key<< ":" << tcrdberrmsg(ecode));                                        
        return false;                                                                                                                             
    }                                                                                                               

    MCE_DEBUG("key is " + key + ", value_size is " << value_size/sizeof(int));                                                         

    int* p_uids = (int*)value_ptr;                                                                                   
    for (int i=0; i<value_size/sizeof(int); i++) {                                                                                      
        vec.push_back(*p_uids);p_uids++;                                                                           
    }                                                                                                                                             

    MCE_DEBUG("[DEBUG] stepping out of TT::Query");
    return true;

}                         

bool TokyoTyrantCache::Insert(const char* key_buf, int key_size, 
        const char* value_buf, int value_size) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

	int ecode = 0;

	if (!tcrdbput(db_handle_, key_buf, key_size, value_buf, value_size)) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("tcrdbput() error:" << tcrdberrmsg(ecode));
		return false;
	}

    return true;
}

//连接数据
bool TokyoTyrantCache::Concatenate(const char* key_buf, int key_size, 
        const char* value_buf, int value_size) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

	int ecode = 0;

	if (!tcrdbputcat(db_handle_, key_buf, key_size, value_buf, value_size)) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("tcrdbcat() error:" << tcrdberrmsg(ecode));
		return false;
	}

    return true;
}

bool TokyoTyrantCache::Query(const long key, ByteSeq& value) {
  if (db_handle_ == NULL) return false;

	char key_name[100];
	int key_size = 0;
	char *value_ptr = NULL;
	int value_size = 0;
	int ecode = 0;

	key_size = GenerateKeyName(key, key_name, sizeof(key_name));
	value_ptr = static_cast<char*>(tcrdbget(db_handle_, key_name, key_size, &value_size));
	if (value_ptr == NULL) {
		ecode = tcrdbecode(db_handle_);
		MCE_WARN("Cannot find data for " << key_name << ":" << tcrdberrmsg(ecode));
		return false;
	}

  value.assign(value_ptr, value_ptr + value_size);

	tcfree(value_ptr);

	return true;
}


bool TokyoTyrantCache::Query(const int key, ByteSeq& value) {
  if (db_handle_ == NULL) return false;

	char key_name[100];
	int key_size = 0;
	char *value_ptr = NULL;
	int value_size = 0;
	int ecode = 0;

	key_size = GenerateKeyName(key, key_name, sizeof(key_name));
	value_ptr = static_cast<char*>(tcrdbget(db_handle_, key_name, key_size, &value_size));
	if (value_ptr == NULL) {
		ecode = tcrdbecode(db_handle_);
		MCE_WARN("Cannot find data for " << key_name << ":" << tcrdberrmsg(ecode));
		return false;
	}

  value.assign(value_ptr, value_ptr + value_size);

	tcfree(value_ptr);

	return true;
}


bool TokyoTyrantCache::Query(const vector<long>& keys, map<long, ByteSeq>& id_2_user_info) {
  if (db_handle_ == NULL) return false;

  char key_name[100];
  const char *key_ptr = NULL;
  int key_size = 0;
  const char *value_ptr = NULL;
  int value_size = 0;
  bool res = false;
  int ecode = 0;
  ByteSeq value;
  long user_id;

  TCMAP *record_map = tcmapnew();
  if (record_map == NULL) {
    ecode = tcrdbecode(db_handle_);
    MCE_FATAL("Cannot alloc result map:" << tcrdberrmsg(ecode));
    goto _exit;
  }

  for (LongSeq::const_iterator key_iterator = keys.begin();
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
    if (value_ptr = static_cast<const char*>(tcmapiterval(key_ptr, &value_size))) {
      value.assign(value_ptr, value_ptr + value_size);
      if (0 < DeGenerateKey(key_ptr, user_id)) {
        if (user_id >= 0) {
          id_2_user_info.insert(make_pair(user_id, value));
        } else {
          MCE_ERROR("TokyoTyrantCache::Query Error, user_id=" << user_id);
        }
      }
    } else {
      MCE_ERROR("TokyoTyrantCache::Query Error, value_ptr is null");
    }
  }

  res = true;

_exit:

  if (record_map != NULL) {
    tcmapdel(record_map);
  }
  return res;
}

bool TokyoTyrantCache::Query(const vector<int>& keys, map<int, ByteSeq>& id_2_user_info) {
  if (db_handle_ == NULL) return false;

  char key_name[100];
  const char *key_ptr = NULL;
  int key_size = 0;
  const char *value_ptr = NULL;
  int value_size = 0;
  bool res = false;
  int ecode = 0;
  ByteSeq value;
  int user_id;

  TCMAP *record_map = tcmapnew();
  if (record_map == NULL) {
    ecode = tcrdbecode(db_handle_);
    MCE_FATAL("Cannot alloc result map:" << tcrdberrmsg(ecode));
    goto _exit;
  }

  for (IntSeq::const_iterator key_iterator = keys.begin();
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
    if (value_ptr = static_cast<const char*>(tcmapiterval(key_ptr, &value_size))) {
      value.assign(value_ptr, value_ptr + value_size);
      if (0 < DeGenerateKey(key_ptr, user_id)) {
        if (user_id >= 0) {
          id_2_user_info.insert(make_pair(user_id, value));
        } else {
          MCE_ERROR("TokyoTyrantCache::Query Error, user_id=" << user_id);
        }
      }
    } else {
      MCE_ERROR("TokyoTyrantCache::Query Error, value_ptr is null");
    }
  }

  res = true;

_exit:

  if (record_map != NULL) {
    tcmapdel(record_map);
  }
  return res;
}

bool TokyoTyrantCache::Insert(const long key, const ByteSeq& data) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;


	char key_name[100];
	int key_size = 0;

	int ecode = 0;

	key_size = GenerateKeyName(key, key_name, sizeof(key_name));

  //TODO(zhigang.wu@opi-corp.com): tcrdbput2
	if (!tcrdbput(db_handle_, key_name, key_size, data.data(), data.size())) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("tcrdbput() error:" << tcrdberrmsg(ecode));
		return false;
	}

    return true;
}


bool TokyoTyrantCache::Insert(const int key, const ByteSeq& data) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;


	char key_name[100];
	int key_size = 0;

	int ecode = 0;

	key_size = GenerateKeyName(key, key_name, sizeof(key_name));

  //TODO(zhigang.wu@opi-corp.com): tcrdbput2
	if (!tcrdbput(db_handle_, key_name, key_size, data.data(), data.size())) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("tcrdbput() error:" << tcrdberrmsg(ecode));
		return false;
	}

    return true;
}

bool TokyoTyrantCache::Insert(long key, const SearchCacheData &data) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;


	char key_name[100];
	int key_size = 0;

	char *value_ptr = NULL;
	int value_len = 0;
	int ecode = 0;

	BinStringPtr binary_data = TokyoTyrantCache::Serialize(data);
	key_size = GenerateKeyName(key, key_name, sizeof(key_name));

	binary_data->getBuf(&value_ptr, &value_len);
	if (!tcrdbput(db_handle_, key_name, key_size, value_ptr, value_len)) {
		ecode = tcrdbecode(db_handle_);
		MCE_ERROR("tcrdbput() error:" << tcrdberrmsg(ecode));
		return false;
	}

    return true;
}

bool TokyoTyrantCache::Insert(int key, const SearchCacheData &data) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;


	char key_name[100];
	int key_size = 0;

	char *value_ptr = NULL;
	int value_len = 0;
	int ecode = 0;

	BinStringPtr binary_data = TokyoTyrantCache::Serialize(data);
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
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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

bool TokyoTyrantCache::Insert(const std::map<int, ByteSeq>& id_2_info_map) {
  if (NULL == db_handle_) return false;

  for (std::map<int, ByteSeq>::const_iterator iter=id_2_info_map.begin(); 
      iter!=id_2_info_map.end(); ++iter) {
    if(!Insert(iter->first, iter->second)) {
      MCE_ERROR("Insert data error for userid " << iter->first);
    }
  }

  return true;
}

bool TokyoTyrantCache::Remove(long key) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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
bool TokyoTyrantCache::Remove(int key) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

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
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

	for (vector<long>::const_iterator keys_iterator = keys.begin();
			keys_iterator != keys.end(); ++keys_iterator) {
		if (!this->Remove(*keys_iterator)) {
			MCE_ERROR("removing " << (*keys_iterator) << " failed");
		}
	}

    return true;
}
bool TokyoTyrantCache::Remove(const vector<int> &keys) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return false;

	for (vector<int>::const_iterator keys_iterator = keys.begin();
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

BinStringPtr TokyoTyrantCache::Serialize(const SearchCacheData &data) {
    return data.Serialize();
}

bool TokyoTyrantCache::Unserialize(SearchMemCacheData *data_ptr, const char *buf, int lens) {
    return data_ptr->Unserialize(buf, lens);
}

bool TokyoTyrantCache::Unserialize(SearchCacheData *data_ptr, const char *buf, int lens) {
    return data_ptr->Unserialize(buf, lens);
}

int TokyoTyrantCache::GenerateKeyName(long userid, char *keybuf, int keybuflen)
{
	return snprintf(keybuf, keybuflen, "user_%ld", userid);
}
int TokyoTyrantCache::DeGenerateKey(const char *keybuf, long& userid)
{
	return sscanf(keybuf, "user_%ld", &userid);
}

int TokyoTyrantCache::GenerateKeyName(int userid, char *keybuf, int keybuflen)
{
	return snprintf(keybuf, keybuflen, "user_%d", userid);
}
int TokyoTyrantCache::DeGenerateKey(const char *keybuf, int& userid)
{
	return sscanf(keybuf, "user_%d", &userid);
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

BinString* TokyoTyrantResult::FetchObject(long int& key) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return NULL;

	BinString *data_ptr = NULL;

	char *key_ptr = NULL;
	int key_size = 0;
	char *value_ptr = NULL;
	int value_size = 0;
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
		//goto _exit;
        if (key_ptr != NULL) {
            tcfree(key_ptr);
        }
		goto _next;
	}

    if (value_ptr == NULL) {
		MCE_FATAL("value_ptr is NULL");
        goto _exit;
    }

    char* endptr;
    errno = 0;    /* To distinguish success/failure after call */
    //key_ptr = "user_xxxx"
    key = strtol(key_ptr+5, &endptr, 10);

    /* Check for various possible errors */

    if ((errno == ERANGE && (key == LONG_MAX || key == LONG_MIN))
            || (errno != 0 && key == 0)) {
		MCE_FATAL("key_ptr is error");
        goto _exit;
    }

	data_ptr = new BinString(value_ptr, value_size);
    
_exit:

	if (key_ptr != NULL) {
		tcfree(key_ptr);
	}

	if (value_ptr != NULL) {
		tcfree(value_ptr);
	}

    return data_ptr;
}
BinString* TokyoTyrantResult::FetchObject(int& key) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return NULL;

	BinString *data_ptr = NULL;

	char *key_ptr = NULL;
	int key_size = 0;
	char *value_ptr = NULL;
	int value_size = 0;
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
		//goto _exit;
        if (key_ptr != NULL) {
            tcfree(key_ptr);
        }
		goto _next;
	}

    if (value_ptr == NULL) {
		MCE_FATAL("value_ptr is NULL");
        goto _exit;
    }

    //key_ptr = "user_xxxx"
    key = atoi(key_ptr+5);
    data_ptr = new BinString(value_ptr, value_size);
    
_exit:

	if (key_ptr != NULL) {
		tcfree(key_ptr);
	}

	if (value_ptr != NULL) {
		tcfree(value_ptr);
	}

    return data_ptr;
}


SearchMemCacheData* TokyoTyrantResult::FetchObject(char*) {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return NULL;

	SearchMemCacheData *data_ptr = NULL;

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
		//goto _exit;
        if (key_ptr != NULL) {
            tcfree(key_ptr);
        }
		goto _next;
	}

	data_ptr = new SearchMemCacheData();
	if (data_ptr == NULL) {
		MCE_FATAL("Cannot generate 1 SearchMemCacheData object");
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

SearchCacheData* TokyoTyrantResult::FetchObject() {
	//assert(db_handle_ != NULL); 
    if (db_handle_ == NULL) return NULL;

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
		//goto _exit;
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
        std::cout <<"id=" << s._id << std::endl; \
        std::cout << "name=" << s._name << std::endl; \
        std::cout << "gender=" << s._gender<< std::endl; \
        std::cout << "signature=" << s._signature << std::endl; \
        std::cout << "star=" << s._star << std::endl; \
        std::cout << "status=" << s._status << std::endl; \
        std::cout << "stage=" << s._stage<< std::endl; \
        std::cout << "birthday_year=" << s._birthday_year<< std::endl; \
        std::cout << "birthday_month=" << s._birthday_month<< std::endl; \
        std::cout << "birthday_day=" << s._birthday_day<< std::endl; \
        std::cout << "tinyurl=" << s._tinyurl << std::endl; \
        std::cout << "headurl=" << s._headurl << std::endl; \
        std::cout << "astrology=" << s._astrology << std::endl; \
        std::cout << "browse_config=" << s._browse_config << std::endl; \
        std::cout << "status_config=" << s._status_config << std::endl; \
        std::cout << "home_city=" << s._home_city << std::endl; \
        std::cout << "home_province=" << s._home_province << std::endl; \
        std::cout << "intrest=" << s._interest<< std::endl; \
        std::cout << "music=" << s._music<< std::endl; \
        std::cout << "movie=" << s._movie<< std::endl; \
        std::cout << "game=" << s._game<< std::endl; \
        std::cout << "comic=" << s._comic<< std::endl; \
        std::cout << "sport=" << s._sport<< std::endl; \
        std::cout << "book=" << s._book<< std::endl; \
        std::cout << "society=" << s._society<< std::endl; \
        std::cout << "lastlogintime=" << s._lastlogintime << std::endl; \
        std::cout << "network" << std::endl; \
        for (int i=0; i< s._networks.size(); ++i) { \
            std::cout << "\tid=" << s._networks[i].id << std::endl; \
            std::cout << "\tstage=" << s._networks[i].stage << std::endl; \
            std::cout << "\tname=" << s._networks[i].name<< std::endl; \
        } \
        std::cout << "regions" << std::endl; \
        for (int i=0; i< s._regions.size(); ++i) { \
            std::cout << "\tid=" << s._regions[i].id << std::endl; \
            std::cout << "\tcity=" << s._regions[i].city<< std::endl; \
            std::cout << "\tprovince=" << s._regions[i].province<< std::endl; \
        } \
        std::cout << "workplace" << std::endl; \
        for (int i=0; i< s._workplaces.size(); ++i) { \
            std::cout << "\tid=" << s._workplaces[i].id << std::endl; \
            std::cout << "\tname=" << s._workplaces[i].name<< std::endl; \
        } \
        std::cout << "university" << std::endl; \
        for (int i=0; i< s._universities.size(); ++i) { \
            std::cout << "\tid=" << s._universities[i].id << std::endl; \
            std::cout << "\tyear=" << s._universities[i].year << std::endl; \
            std::cout << "\tname=" << s._universities[i].name<< std::endl; \
            std::cout << "\tdepartment=" << s._universities[i].department << std::endl; \
            std::cout << "\tdorm=" << s._universities[i].dorm<< std::endl; \
        } \
        std::cout << "college" << std::endl; \
        for (int i=0; i< s._colleges.size(); ++i) { \
            std::cout << "\tid=" << s._colleges[i].id << std::endl; \
            std::cout << "\tyear=" << s._colleges[i].year << std::endl; \
            std::cout << "\tname=" << s._colleges[i].name<< std::endl; \
            std::cout << "\tdepartment=" << s._colleges[i].department << std::endl; \
        } \
        std::cout << "highschool" << std::endl; \
        for (int i=0; i< s._highSchools.size(); ++i) { \
            std::cout << "\tid=" << s._highSchools[i].id << std::endl; \
            std::cout << "\tyear=" << s._highSchools[i].year << std::endl; \
            std::cout << "\tname=" << s._highSchools[i].name<< std::endl; \
            std::cout << "\tclass1=" << s._highSchools[i].class1<< std::endl; \
            std::cout << "\tclass2=" << s._highSchools[i].class2<< std::endl; \
            std::cout << "\tclass3=" << s._highSchools[i].class3<< std::endl; \
        } \
        std::cout << "juniorschool" << std::endl; \
        for (int i=0; i< s._juniorSchools.size(); ++i) { \
            std::cout << "\tid=" << s._juniorSchools[i].id << std::endl; \
            std::cout << "\tyear=" << s._juniorSchools[i].year << std::endl; \
            std::cout << "\tname=" << s._juniorSchools[i].name<< std::endl; \
        } \
        std::cout << "elmentaryschool" << std::endl; \
        for (int i=0; i< s._elementarySchools.size(); ++i) { \
            std::cout << "\tid=" << s._elementarySchools[i].id << std::endl; \
            std::cout << "\tyear=" << s._elementarySchools[i].year << std::endl; \
            std::cout << "\tname=" << s._elementarySchools[i].name<< std::endl; \
        } 

bool getUserFromDBCache(int id) {
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotyrant://127.0.0.1:1981");
    if (db_cache->Initialize() == false) {
		MCE_WARN("Db initialize failed");
        return false;
    }

    std::cout << "error userid = " << id << std::endl;

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
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotyrant://127.0.0.1:1981");
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

    College college;
    college.id = 10001;
    college.year = 1998;
    college.name = "电影学院";
    college.department = "导演系";
    scd._colleges.push_back(college);

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

bool puts(MyUtil::IntSeq& ids, int argc, char**argv) {
    Ice::CommunicatorPtr ic;

    try {
        /*
        std::string str = "M:tcp -h 10.22.202.139 -p ";
        str += argv[3];
        cout << str << endl;
        */
        ic = Ice::initialize(argc, argv);
        Ice::ObjectPrx base = ic->stringToProxy("M:tcp -p 9191");
        SearchCacheManagerPrx searchcache = SearchCacheManagerPrx::checkedCast(base);

        if (!searchcache)
            throw "Invalid Proxy";

        Int2ByteSeq resultMap = searchcache->getSearchCacheList2(ids);
        Int2ByteSeq resultMap1;

        for (MyUtil::IntSeq::iterator it=ids.begin(); it!=ids.end(); it++) {
            Int2ByteSeq::iterator itS = resultMap.find(*it);
            cout << "============================================" << (*itS).second.size() << endl;
            SearchMemCacheDataPtr pSCData = new SearchMemCacheData();
            if (pSCData->Unserialize((const char*)(*itS).second.data(), (*itS).second.size())) {
                //序列化成内存buf
                pSCData->_name = pSCData->_name + "ddd";
                BinStringPtr binObj1 = pSCData->Serialize();
                char* pBuf = NULL;
                int len = 0;
                binObj1->getBuf(&pBuf, &len);
                if (pBuf==NULL || len <= 0) continue;
                resultMap1[*it].assign(pBuf, pBuf+len);
            }
        }
        searchcache->puts(resultMap1);

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

bool getSearchCacheList2(MyUtil::IntSeq& ids, int argc, char**argv) {
    Ice::CommunicatorPtr ic;

    try {
        /*
        std::string str = "M:tcp -h 10.22.202.139 -p ";
        str += argv[3];
        cout << str << endl;
        */
        ic = Ice::initialize(argc, argv);
        Ice::ObjectPrx base = ic->stringToProxy("M:tcp -p 9191");
        SearchCacheManagerPrx searchcache = SearchCacheManagerPrx::checkedCast(base);

        if (!searchcache)
            throw "Invalid Proxy";

        Int2ByteSeq resultMap = searchcache->getSearchCacheList2(ids);

        for (MyUtil::IntSeq::iterator it=ids.begin(); it!=ids.end(); it++) {
            Int2ByteSeq::iterator itS = resultMap.find(*it);
            cout << "============================================" << (*itS).second.size() << endl;
            SearchMemCacheDataPtr pSCData = new SearchMemCacheData();
            if (pSCData->Unserialize((const char*)(*itS).second.data(), (*itS).second.size())) {
                SearchCacheResult result = pSCData->getProperties();
                cout << SEARCH_CACHE_ID   << " : " << result.generels[SEARCH_CACHE_ID] << endl;
                cout << SEARCH_CACHE_STATUS << " : " << result.generels[SEARCH_CACHE_STATUS] << endl;
                //cout << SEARCH_CACHE_ISVIP<< " : " << result.generels[SEARCH_CACHE_ISVIP] << endl;
                cout << SEARCH_CACHE_NAME << " : " << result.generels[SEARCH_CACHE_NAME] << endl;
                cout << SEARCH_CACHE_SIGNATURE << " : " << result.generels[SEARCH_CACHE_SIGNATURE] << endl;
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
            }
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

bool getSearchCacheList(MyUtil::IntSeq& ids, int argc, char**argv) {
    Ice::CommunicatorPtr ic;

    try {
        /*
        std::string str = "M:tcp -h 10.22.202.139 -p ";
        str += argv[3];
        cout << str << endl;
        */
        ic = Ice::initialize(argc, argv);
        Ice::ObjectPrx base = ic->stringToProxy("M:tcp -p 9191");
        SearchCacheManagerPrx searchcache = SearchCacheManagerPrx::checkedCast(base);

        if (!searchcache)
            throw "Invalid Proxy";

        Int2SearchCacheResultMap resultMap = searchcache->getSearchCacheList(ids);

        for (MyUtil::IntSeq::iterator it=ids.begin(); it!=ids.end(); it++) {
            Int2SearchCacheResultMap::iterator itS = resultMap.find(*it);
            if (itS == resultMap.end()) continue;
            cout << "============================================" << endl;
            SearchCacheResult& result = (*itS).second;
            cout << SEARCH_CACHE_ID   << " : " << result.generels[SEARCH_CACHE_ID] << endl;
            cout << SEARCH_CACHE_STATUS << " : " << result.generels[SEARCH_CACHE_STATUS] << endl;
            //cout << SEARCH_CACHE_ISVIP<< " : " << result.generels[SEARCH_CACHE_ISVIP] << endl;
            cout << SEARCH_CACHE_NAME << " : " << result.generels[SEARCH_CACHE_NAME] << endl;
            cout << SEARCH_CACHE_SIGNATURE << " : " << result.generels[SEARCH_CACHE_SIGNATURE] << endl;
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

bool getSearchCache(int id, int argc, char**argv) {
    Ice::CommunicatorPtr ic;

    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectPrx base = ic->stringToProxy("M:tcp -p 9191");
        SearchCacheManagerPrx searchcache = SearchCacheManagerPrx::checkedCast(base);

        if (!searchcache)
            throw "Invalid Proxy";

        SearchCacheResult result = searchcache->getSearchCache(id);

        cout << SEARCH_CACHE_ID   << " : " << result.generels[SEARCH_CACHE_ID] << endl;
        cout << SEARCH_CACHE_STATUS << " : " << result.generels[SEARCH_CACHE_STATUS] << endl;
	    cout << SEARCH_CACHE_NAME << " : " << result.generels[SEARCH_CACHE_NAME] << endl;
        cout << SEARCH_CACHE_SIGNATURE << " : " << result.generels[SEARCH_CACHE_SIGNATURE] << endl;
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

void usage(char* message) {
        cout << "Usage: " << message << " Serialize userid" << endl;
        cout << "Usage: " << message << " getUserFromDBCache userid" << endl;
        cout << "Usage: " << message << " getSearchCache userid" << endl;
        cout << "Usage: " << message << " getSearchCacheList userid ... useridn " << endl;
        cout << "Usage: " << message << " getSearchCacheList2 userid ... useridn " << endl;
        cout << "Usage: " << message << " puts userid ... useridn " << endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        usage(argv[0]);
        return 0;
    }

    Ice::LoggerPtr _logger = new IceLogger("./test_log", "INFO");
    std::string strSwitch = argv[1];
    std::string strId = argv[2];
    int id = boost::lexical_cast<int>(strId);
    MyUtil::IntSeq ids;
    ids.push_back(id);
    for (int i=3; i<argc; i++) {
        ids.push_back(boost::lexical_cast<int>(argv[i]));
    }

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
    else if (strSwitch == "getSearchCacheList") {
        if (!getSearchCacheList(ids, argc, argv))
            cerr << strSwitch << "is error" << endl;
    }
    else if (strSwitch == "getSearchCacheList2") {
        if (!getSearchCacheList2(ids, argc, argv))
            cerr << strSwitch << "is error" << endl;
    }
    else if (strSwitch == "puts") {
        if (!puts(ids, argc, argv))
            cerr << strSwitch << "is error" << endl;
    }
    else {
        usage(argv[0]);
    }

}
#endif
