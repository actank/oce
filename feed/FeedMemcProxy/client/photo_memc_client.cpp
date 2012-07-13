
#include "photo_memc_client.h"

#include <boost/lexical_cast.hpp>

#include "compress_wrap.h"
#include "photo.pb.h"

#include "ServiceI.h"
#include "LogWrapper.h"
#include<stdio.h>
#include <libmemcached/memcached.h>

namespace xce {
namespace feed {

// libmemcached mget() bug : kEmptyPhotoContent 不可以是空串
static char * kEmptyPhotoContent = "EMPTY_PHOTO_CONTENT";

static void GetPhotoContentKey(Ice::Long fid, char * key, size_t len) {
  snprintf(key, len, "PHOTO_CONTENT#%ld", fid);
  key[len - 1] = 0;
}

static Ice::Long GetPhotoId4Content(const char * key, size_t len) {
  if (len < 5 || strncmp("PHOTO_CONTENT#", key, 5) != 0) {
    return -1;
  }

  Ice::Long id = -1;
  try {
    id = boost::lexical_cast<Ice::Long>(string(key + 5, len - 5));
  } catch(boost::bad_lexical_cast &) {
    return -1;
  }
  return id;
}


static void PhotoContentPbToIce(const PbPhotoContent & pb, PhotoContentPtr content) {

  content->photoid = pb.photo();
  content->url = pb.url();
  content->title = pb.title();
  content->ownerid = pb.ownerid();
  content->albumname = pb.albumname().c_str();

  for (int i = 0; i < pb.taguser_size(); i++) {
    int user = pb.taguser(i);
    content->tagusers.push_back(user);
  }

}

PhotoMemcClient::PhotoMemcClient() {
  pool_.Init(100);
}

PhotoMemcClient::~PhotoMemcClient() {
}

PhotoContentDict PhotoMemcClient::GetPhotoContentDict(const MyUtil::LongSeq& ids) {
  vector<string> keys; 
  for(size_t i = 0; i < ids.size(); ++i){
    char key[32];
    GetPhotoContentKey(ids[i], key, 32);
    keys.push_back(key);
  }
  pair<int, MemcachedClient*> cli = pool_.GetClient(ids.at(0)); 
  if(!cli.second){
    return PhotoContentDict();
  }
  map<string, string> res;
  map<string, int> flags;
  cli.second->Get(keys, res, flags);
  pool_.ReleaseClient(cli.first, cli.second);

  PhotoContentDict dict;
  map<string, string>::iterator it = res.begin();
  for(; it!=res.end(); ++it){
    Ice::Long id = GetPhotoId4Content(it->first.c_str(), it->first.size());
    if(it->second == kEmptyPhotoContent){
      dict.insert(make_pair(id, PhotoContentPtr(NULL)));
      continue;
    }

    int flag = flags[it->first.c_str()];
    PbPhotoContent pb;
    if (flag & 0x01) {
      string decompressed;
      CompressWrap::instance().Decompress(it->second.c_str(), &decompressed);
      pb.ParseFromArray(decompressed.data(),decompressed.size());
    } else {
      pb.ParseFromArray(it->second.c_str(), it->second.size());
    }

    PhotoContentPtr content = new PhotoContent();
    PhotoContentPbToIce(pb, content);
    dict.insert(make_pair(content->photoid, content));
  }
  return dict;
}

PhotoContentPtr PhotoMemcClient::GetPhotoContent(Ice::Long photo_id) {
  MyUtil::LongSeq ids;
  ids.push_back(photo_id);
  PhotoContentDict dict = GetPhotoContentDict(ids);
  if(dict.empty()){
    return 0;
  }
  return dict[photo_id]; 
}

// 返回值, 标识是否压缩了
bool PhotoMemcClient::SerializePhotoContent(const PhotoContentPtr& content, string * serialized) {
  if (!content) {
    return false;
  }
  PbPhotoContent pb;
  // TODO : 哪些字段是optional的? 可以优化
  pb.set_photo(content->photoid);
  pb.set_url(content->url);
  pb.set_title(content->title);
  pb.set_albumname(content->albumname);
  pb.set_ownerid(content->ownerid);



  for (int i = 0; i < content->tagusers.size(); i++) {
    pb.add_taguser(content->tagusers.at(i));
    }

  pb.SerializeToString(serialized);

  std::string compressed;
  bool b = CompressWrap::instance().Compress(*serialized, &compressed);
  if (b) {
    serialized->swap(compressed);
  }

  return b;

}

bool PhotoMemcClient::SetEmptyPhotoContent(Ice::Long id) {
  char key[32];
  GetPhotoContentKey(id, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(id); 
  bool res = cli.second->Set(key, kEmptyPhotoContent, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}


bool PhotoMemcClient::SetPhotoContent(const PhotoContentPtr& content) {
  char key[32];
  GetPhotoContentKey(content->photoid, key, 32);

  string value;
  bool b = SerializePhotoContent(content, &value);
  pair<int, MemcachedClient*> cli = pool_.GetClient(content->photoid); 
  bool res = cli.second->Set(key, value, b ? 0x01 : 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

bool PhotoMemcClient::SetPhotoContentSeq(const PhotoContentSeq& contentSeq)
{
	bool res = true;

	  for(int i=0;i<contentSeq.size();i++)
	    {
//	    MCE_INFO("PhotoMemcClient::SetPhotoContentSeq ---> photoid: "<<contentSeq[i]->photoid<<" line: "<<__LINE__);
	      if(!SetPhotoContent(contentSeq[i]))
	      {
	        //MCE_INFO("PhotoMemcClient::SetPhotoContentSeq FAILED!! photoid: "<<contentSeq[i]->photoid<<"  line: "<<__LINE__);
	        res = false;
	      }

	    }
	  return res;
}

/*----------------------------------------------------------------------------*/


static char * kEmptyPhotoRankInfo = "EMPTY_PHOTO_RANK";

static void GetPhotoRankInfoKey(Ice::Long fid, char * key, size_t len) {
  snprintf(key, len, "PHOTO_RANK#%ld", fid);
  key[len - 1] = 0;
}

static Ice::Long GetPhotoId4RankInfo(const char * key, size_t len) {
  if (len < 5 || strncmp("PHOTO_RANK#", key, 5) != 0) {
    return -1;
  }

  Ice::Long id = -1;
  try {
    id = boost::lexical_cast<Ice::Long>(string(key + 5, len - 5));
  } catch(boost::bad_lexical_cast &) {
    return -1;
  }
  return id;
}


static void PhotoRankInfoPbToIce(const PbPhotoRankInfo & pb, PhotoRankInfoPtr rankInfo) {


    rankInfo->clickcount = pb.clickcount();
    rankInfo->commentcount  =pb.commentcount();
    rankInfo->tagcount = pb.tagcount();
    rankInfo->time = pb.time();
}


PhotoRankInfoDict PhotoMemcClient::GetPhotoRankInfoDict(const MyUtil::LongSeq& ids) {
  vector<string> keys; 
  for(size_t i = 0; i < ids.size(); ++i){
    char key[32];
    GetPhotoRankInfoKey(ids[i], key, 32);
    keys.push_back(key);
  }
  pair<int, MemcachedClient*> cli = pool_.GetClient(ids.at(0)); 
  if(!cli.second){
    return PhotoRankInfoDict();
  }
  map<string, string> res;
  map<string, int> flags;
  cli.second->Get(keys, res, flags);
  pool_.ReleaseClient(cli.first, cli.second);

  PhotoRankInfoDict dict;
  map<string, string>::iterator it = res.begin();
  for(; it!=res.end(); ++it){
    Ice::Long id = GetPhotoId4RankInfo(it->first.c_str(), it->first.size());
    if(it->second == kEmptyPhotoRankInfo){
      dict.insert(make_pair(id, PhotoRankInfoPtr(NULL)));
      continue;
    }
    int flag = flags[it->first.c_str()];
    PbPhotoRankInfo pb;
    if (flag & 0x01) {
      string decompressed;
      CompressWrap::instance().Decompress(it->second.c_str(), &decompressed);
      pb.ParseFromArray(decompressed.data(),decompressed.size());
    } else {
      pb.ParseFromArray(it->second.c_str(), it->second.size());
    }
    PhotoRankInfoPtr rankInfo = new PhotoRankInfo();
    PhotoRankInfoPbToIce(pb, rankInfo);
    dict.insert(make_pair(rankInfo->photoid, rankInfo));
  }
  return dict; 
}

PhotoRankInfoPtr PhotoMemcClient::GetPhotoRankInfo(Ice::Long photo_id) {
  MyUtil::LongSeq ids;
  ids.push_back(photo_id);
  PhotoRankInfoDict dict = GetPhotoRankInfoDict(ids);
  if(dict.empty()){
    return 0;
  }
  return dict[photo_id]; 
}

// 返回值, 标识是否压缩了
bool PhotoMemcClient::SerializePhotoRankInfo(const PhotoRankInfoPtr& rankInfo, string * serialized) {
  if (!rankInfo) {
    return false;
  }
  PbPhotoRankInfo pb;
  // TODO : 哪些字段是optional的? 可以优化
  pb.set_photo(rankInfo->photoid);
  pb.set_time(rankInfo->time);
  pb.set_clickcount(rankInfo->clickcount);
  pb.set_commentcount(rankInfo->commentcount);
  pb.set_tagcount(rankInfo->tagcount);


  pb.SerializeToString(serialized);

  std::string compressed;
  bool b = CompressWrap::instance().Compress(*serialized, &compressed);
  if (b) {
    serialized->swap(compressed);
  }
  return b;
}

bool PhotoMemcClient::SetEmptyPhotoRankInfo(Ice::Long id) {
  char key[32];
  GetPhotoRankInfoKey(id, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(id); 
  bool res = cli.second->Set(key, kEmptyPhotoRankInfo, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

bool PhotoMemcClient::SetPhotoRankInfo(const PhotoRankInfoPtr& rankInfo) {
  char key[32];
  GetPhotoRankInfoKey(rankInfo->photoid, key, 32);

  string value;
  bool b = SerializePhotoRankInfo(rankInfo, &value);
  pair<int, MemcachedClient*> cli = pool_.GetClient(rankInfo->photoid); 
  bool res = cli.second->Set(key, value, b ? 0x01 : 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

bool PhotoMemcClient::SetPhotoRankInfoSeq(const PhotoRankInfoSeq& rankInfoSeq)
{
  bool res = true;

  for(int i=0;i<rankInfoSeq.size();i++)
    {
      if(!SetPhotoRankInfo(rankInfoSeq[i]))
      {
//        MCE_INFO("PhotoMemcClient::SetPhotoRankInfo FAILED!! userid: "<<rankInfoSeq[i]->photoid<<"  line: "<<__LINE__);
        res = false;
      }

    }
  return res;
}


/*----------------------------------------------------------------------*/

static char * kEmptyPhotoSeq = "EMPTY_PHOTO_SEQ";

static void GetPhotoSeqKey(int uid, char * key, size_t len) {
  snprintf(key, len, "PHOTO_SEQ#%d", uid);
  key[len - 1] = 0;
}

static int  GetUserId4Seq(char * key, size_t len) {
  if (len < 5 || strncmp("PHOTO_SEQ#", key, 5) != 0) {
    return -1;
  }

  int  id = -1;
  try {
    id = boost::lexical_cast<int >(string(key + 5, len - 5));
  } catch(boost::bad_lexical_cast &) {
    return -1;
  }
  return id;
}

static void PhotoSeqPbToIce(const PbPhotoSeq& pb, ContentItemSeq& seq) {

  for (int i = 0; i < pb.contentitem_size(); i++) {
      ContentItem item;
      item.photoid = pb.contentitem(i).photoid();
      item.ownerid = pb.contentitem(i).ownerid();
      seq.push_back(item);
    }
}


ContentItemSeq PhotoMemcClient::GetPhotoSeq(int  uid) {
  char key[32];
  GetPhotoSeqKey(uid, key, 32);
  vector<string> keys;
  keys.push_back(key);
  pair<int, MemcachedClient*> cli = pool_.GetClient(uid); 
  if(!cli.second){
    return ContentItemSeq();
  }
  map<string, string> res;
  map<string, int> flags;
  cli.second->Get(keys, res, flags);
  pool_.ReleaseClient(cli.first, cli.second);
  if(res.empty()){
    return ContentItemSeq();
  } 
  if(res[key]==kEmptyPhotoSeq){
    return ContentItemSeq();
  }
  int flag = flags[key];
  PbPhotoSeq pb;
  if (flag & 0x01) {
    string decompressed;
    CompressWrap::instance().Decompress(res[key], &decompressed);
    pb.ParseFromArray(decompressed.data(),decompressed.size());
  } else {
    pb.ParseFromArray(res[key].c_str(), res[key].size());
  }
  ContentItemSeq photoSeq;
  PhotoSeqPbToIce(pb, photoSeq);
  return photoSeq;
}


bool PhotoMemcClient::SerializePhotoSeq(const ContentItemSeq& photoSeq, string * serialized) {
  if (photoSeq.empty()) {
    return false;
  }
  PbPhotoSeq pb;
  // TODO : 哪些字段是optional的? 可以优化
  for (int i = 0; i < photoSeq.size(); i++) {
    //pb.set_photoid(i,photoSeq[i]);
    PbContentItem * item = pb.add_contentitem();
    item->set_photoid(photoSeq.at(i).photoid);
    item->set_ownerid(photoSeq.at(i).ownerid);
  }

  pb.SerializeToString(serialized);

  std::string compressed;
  bool b = CompressWrap::instance().Compress(*serialized, &compressed);
  if (b) {
    serialized->swap(compressed);
  }
  return b;
}

bool PhotoMemcClient::SetEmptyPhotoSeq(int  id) {
  char key[32];
  GetPhotoSeqKey(id, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(id); 
  bool res = cli.second->Set(key, kEmptyPhotoSeq, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

bool PhotoMemcClient::SetPhotoSeq(int uid,const ContentItemSeq& photoSeq) {
  char key[32];
  GetPhotoSeqKey(uid, key, 32);
  string value;
  bool b = SerializePhotoSeq(photoSeq, &value);
  pair<int, MemcachedClient*> cli = pool_.GetClient(uid); 
  bool res = cli.second->Set(key, value, b ? 0x01 : 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

bool PhotoMemcClient::SetPhotoSeqList(const UserPhotoSeqList& list)
{
  bool res = true;

    for(int i=0;i<list.size();i++)
      {
        if(!SetPhotoSeq(list[i]->userid,list[i]->itemList))
        {
//          MCE_INFO("PhotoMemcClient::SetPhotoSeq FAILED!! userid: "<<list[i]->userid<<"  line: "<<__LINE__);
          res = false;
        }

      }
    return res;

}

}

}

