#ifndef XCE_FEED_HDF_PARSER_H__
#define XCE_FEED_HDF_PARSER_H__

#include <stdio.h>
#include <string.h>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>

#include <expat.h>
#include <boost/lexical_cast.hpp>
#include "feed/feedview/headurl.h"

#include "QueryRunner.h"

using namespace std;

namespace xce {

class FeedHdfParser;

class FeedXmlSchema {
public:
  int Init(const string & xml) {
    XML_Parser parser = XML_ParserCreate(NULL);
    if (!parser) {
      return -1;
    }
    // string data("<key name=\"f\">" + xml + "</key>\r\n");
    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, SchemaStartTag, SchemaEndTag);
    // XML_SetElementHandler(parser, NULL, SchemaEndTag);
    if (!XML_Parse(parser, xml.c_str(), xml.size(), 1)) {
      MCE_WARN("Parse error at line " << XML_GetCurrentLineNumber(parser) 
          << XML_ErrorString(XML_GetErrorCode(parser)));
      XML_ParserFree(parser);
      return -2;
    }

    XML_ParserFree(parser);
    return 0;
  }

  FeedHdfParser * CreateHdfParser() const;

  void InitLoopKeyIndex(map<string, int> * index_map) const {
    for(set<std::string>::const_iterator i = loop_keys_.begin(); i != loop_keys_.end(); ++i) {
      index_map->insert(make_pair(*i, 0));
    }
  }

private:
  std::vector<std::string> tag_path_;
  set<std::string> loop_keys_;
  set<std::string> headurl_keys_;

  string GetPath(const string & key_name) const {
    string full_tag;
    for (size_t i = 0; i < tag_path_.size(); ++i) {
      full_tag += tag_path_[i];
      full_tag += '.';
    }
    full_tag += key_name;
    return full_tag;
  }

  static void SchemaStartTag(void *data, const char *el, const char **attr) {
    FeedXmlSchema * obj = (FeedXmlSchema *)data;
    bool is_loop = false;
    bool is_headurl = false;
    string key_name;
    for (size_t i = 0; attr[i]; i += 2) {
      if (strcmp("is_loop", attr[i]) == 0 
          && strcmp("1", attr[i + 1]) == 0) {
        is_loop = true;
      } else if (strcmp("name", attr[i]) == 0) {
        key_name = attr[i + 1];
      } else if (strcmp("type", attr[i]) == 0
          && strcmp("headurl", attr[i + 1]) == 0) {
        is_headurl = true;
      }
    }

    if (is_loop) {
      obj->loop_keys_.insert(obj->GetPath(key_name));
    }
    if (is_headurl) {
      obj->headurl_keys_.insert(obj->GetPath(key_name));
    }

    obj->tag_path_.push_back(key_name);
  }

  static void SchemaEndTag(void *data, const char *el) {
    FeedXmlSchema * obj = (FeedXmlSchema *)data;
    obj->tag_path_.pop_back();
  }
};

class FeedSchemaCache {
 public:
  static FeedSchemaCache & Instance() {
    static FeedSchemaCache cache;
    return cache;
  }
  const FeedXmlSchema * FindFeedSchema(unsigned int stype, unsigned int version) {
#if 0
    FeedXmlSchema * feed_schema = new FeedXmlSchema();
    string schema;
    {
      std::ifstream fin("schema.xml", std::ios::binary);
      if (!fin) {
        fprintf(stderr, "open cs file error\n");
        return NULL;
      }
      printf("open cs file ok\n");
      fin.unsetf(std::ios::skipws);
      std::copy(std::istream_iterator<char>(fin), std::istream_iterator<char>() , std::back_inserter(schema));
    }
    feed_schema->Init(schema);
    return feed_schema;
#else
    string key = boost::lexical_cast<string>(stype) + '_' + boost::lexical_cast<string>(version);
 
    IceUtil::RWRecMutex::RLock lock(schemas_mutex_);
    map<string, FeedXmlSchema*>::iterator it = schemas_.find(key);
    if (it != schemas_.end()) {
      return it->second;
    }
    return NULL;
#endif
  }

  bool Reload(int is_test) {
    MCE_INFO("Load feed schemas. is_test=" << is_test)
#if 1
    com::xiaonei::xce::Statement sql;

    sql << "select stype, version, keys_xml from data_config ";
    if (is_test) {
      sql << "where status > 0;";
    } else {
      sql << "where status > 1;";
    }

    mysqlpp::StoreQueryResult result;
    try {
      result = com::xiaonei::xce::QueryRunner("feed_admin_db", com::xiaonei::xce::CDbRServer).store(sql);
    } catch(std::exception & e) {
      MCE_WARN("load db data_config error : " << e.what());
      return false;
    }

    if (!result) {
      MCE_WARN("load db data_config return NULL");
      return false;
    }

    map<string, FeedXmlSchema*> schemas;
    for (size_t i = 0; i < result.num_rows(); ++i) {
      const mysqlpp::Row & row = result[i];
      string key = boost::lexical_cast<string>((int)row["stype"]) + '_' + boost::lexical_cast<string>((int)row["version"]);
      string schema = (std::string)row["keys_xml"];
      FeedXmlSchema * feed_schema = new FeedXmlSchema();
      if (feed_schema->Init(schema) != 0) {
        MCE_WARN(key << " keys_xml error.");
        delete feed_schema;
        continue;
      }
      MCE_INFO(key << " keys_xml load ok.");
      schemas.insert(make_pair(key, feed_schema));
    }
    MCE_INFO("keys_xml loaded count : " << schemas.size());

    {
      IceUtil::RWRecMutex::WLock lock(schemas_mutex_);
      schemas_.swap(schemas);
    }
#endif
    return true;
  }
 private:
  map<string, FeedXmlSchema*> schemas_;
  IceUtil::RWRecMutex schemas_mutex_;
};

typedef vector<map<string, string> > LoopNodeItems;
class FeedHdfParser {
 private:
  vector<string> tag_path_;
  set<string> headurl_keys_;
  map<string, LoopNodeItems> loop_nodes_;
  map<string, string> hdf_map_;
  bool tpl_error_;
  bool text_start_;
  // friend class FeedSchemaCache;
  friend class FeedXmlSchema;
  FeedHdfParser() : tpl_error_(false) {}
 public:
  map<string, string> & hdf_map() {
    return hdf_map_;
  }
  map<string, LoopNodeItems> & loop_nodes() {
    return loop_nodes_;
  }

  const string & Find(const char *key) const {
    map<string, string>::const_iterator it = hdf_map_.find(key);
    if (hdf_map_.end() != it) {
      return it->second;
    }
    static const std::string empty;
    return empty;
  }

  int Parse(const string & feed_xml) {
    std::string packed_xml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><feeds>");
    packed_xml += feed_xml;
    packed_xml += "</feeds>";

    XML_Parser parser = XML_ParserCreate(NULL);
    if (!parser) {
      MCE_WARN("Couldn't allocate memory for parser");
      return -1;
    }
    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, FeedHdfParser::OnTagStart, FeedHdfParser::OnTagEnd);
    XML_SetCharacterDataHandler(parser, FeedHdfParser::OnText);

    if (!XML_Parse(parser, packed_xml.c_str(), packed_xml.size(), 1)) {
      MCE_WARN("Parse error at line " << XML_GetCurrentLineNumber(parser) 
          << XML_ErrorString(XML_GetErrorCode(parser)));
      XML_ParserFree(parser);
      return -1;
    }

    XML_ParserFree(parser);
    return 0;
  }

  const static size_t kStartDepth = 1; // 从xml的第二级节点开始取路径, 忽略根节点(feeds)
  string GetXmlPath() {
    string xml_path;
    for (size_t i = kStartDepth; i < tag_path_.size(); ++i) {
      if (isdigit(tag_path_[i][0])) {
        continue;
      }
      if (i > kStartDepth) {
        xml_path += '.';
      }
      xml_path += tag_path_[i];
    }
    return xml_path;
  }


  static string GetLoopChildPath(const string & full_path) {
    size_t pos = full_path.find_first_of('.');
    while(pos != string::npos && pos < full_path.size() - 1) {
      if (isdigit(full_path[pos + 1])) {
        pos = full_path.find_first_of('.', pos + 1);
        if (pos == string::npos) {
          return string();
        }
        return full_path.substr(pos + 1);
      }
      pos = full_path.find_first_of('.', pos + 1);
    }
    return string();
  } 

  static string GetLoopParentPath(const string & full_path) {
    size_t pos = full_path.find_first_of('.');
    while(pos != string::npos && pos < full_path.size() - 1) {
      if (isdigit(full_path[pos + 1])) {
        return full_path.substr(0, pos);
      }
      pos = full_path.find_first_of('.', pos + 1);
    }
    return string();
  } 

  string GetHdfPath() {
    string hdf_path;
    for (size_t i = kStartDepth; i < tag_path_.size(); ++i) {
      if (i > kStartDepth) {
        hdf_path += '.';
      }
      hdf_path += tag_path_[i];
    }
    return hdf_path;
  }

  static void OnTagStart(void *data, const char *el, const char **attr) {
    FeedHdfParser * obj = (FeedHdfParser *)data;
    string xml_path = obj->GetXmlPath();
    obj->text_start_ = true;
    if (xml_path.empty()) {
      xml_path = el;

      for (int i = 0; attr[i]; i += 2) {
        if (0 == strcmp(attr[i], "tpl")) {
          if (0 != strcmp(attr[i + 1], "cs")) {
            obj->tpl_error_ = true;
          }
        }
      }
    } else {
      xml_path += '.';
      xml_path += el;
    }
    map<string, LoopNodeItems>::iterator it = obj->loop_nodes_.find(xml_path);

    obj->tag_path_.push_back(el);
    if (it != obj->loop_nodes_.end()) {
      it->second.push_back(map<string, string>());
      obj->tag_path_.push_back(boost::lexical_cast<string>(it->second.size()));
    }
  }

  static void OnTagEnd(void *data, const char *el) {
    FeedHdfParser * obj = (FeedHdfParser *)data;
    string xml_path = obj->GetXmlPath();
    map<string, LoopNodeItems>::iterator it = obj->loop_nodes_.find(xml_path);
    if (it != obj->loop_nodes_.end()) {
      obj->tag_path_.pop_back();
    }

    obj->tag_path_.pop_back();
    if (obj->tag_path_.empty()) {
      // obj->ProcessHeadUrl();
    }
  }

  enum MergePolicy { MP_APPEND, MP_REPLACE, MP_IGNORE };

  static MergePolicy GetMergePolicy(const string & hdf_path) {
    const char * p = hdf_path.c_str();
    const char * q = p;
    while(*q) {
      ++p;
      if (*q == '.' && isdigit(*p)) {
        return MP_APPEND;
      }
      q = p;
    }
    return MP_IGNORE;
    // return MP_REPLACE;
  }

  static void OnText(void *data, const char *txt, int txtlen) {
    FeedHdfParser * obj = (FeedHdfParser *)data;
    if (obj->tpl_error_) {
      return;
    }

    size_t pos = string(txt, txtlen).find_first_not_of(" \t\r\n");
    if (pos == string::npos) {
      return;
    }

    string hdf_path = obj->GetHdfPath();
    string body(txt, txtlen);
    MergePolicy policy = GetMergePolicy(hdf_path);

    string xml_path = obj->GetXmlPath();
    if (obj->headurl_keys_.find(xml_path) != obj->headurl_keys_.end()) {
      body = HeadUrl::FromPart(body);
    }

    if (xml_path != hdf_path) {
      // 循环节点，放在 loop_nodes_ 中
      string child_path = GetLoopChildPath(hdf_path);
      string parent_path = GetLoopParentPath(hdf_path);
      map<string, LoopNodeItems>::iterator it = obj->loop_nodes_.find(parent_path);
      if (it != obj->loop_nodes_.end()) {
        map<string, string> & node = it->second.back();
        node[child_path] += body;
        MCE_DEBUG("循环数据节点 : " << parent_path << "-"<< child_path);
      } else {
        MCE_WARN("循环数据节点错误 : " << parent_path);
      }
    } else {
      // 非循环节点，放在hdf_map_中
      switch(policy) {
      case MP_APPEND:
        obj->hdf_map_[hdf_path] += body;
        MCE_DEBUG("key " << hdf_path << " = " << obj->hdf_map_[hdf_path]);
        break;
      case MP_REPLACE:
        obj->hdf_map_[hdf_path] = body;
        break;
      default:
        if (obj->hdf_map_.find(hdf_path) == obj->hdf_map_.end()) {
          obj->hdf_map_[hdf_path] = body;
          MCE_DEBUG("key " << hdf_path << " = " << body);
          obj->text_start_ = false;
        } else {
          if (obj->text_start_ == false) {
            obj->hdf_map_[hdf_path] += body;
          }
        }
      }
    }
  }
};

}
#endif // XCE_FEED_HDF_PARSER_H__


