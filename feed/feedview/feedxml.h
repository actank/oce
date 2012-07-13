#ifndef XCE_FEED_XML_MAP_H__
#define XCE_FEED_XML_MAP_H__

#include <string>
#include <list>
#include <vector>
#include <map>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <expat.h>

#include "feed/base_feed/common.h"
#include "feed/base_feed/logging.h"

//#include <iostream>


namespace xce {

#define kTemplateVersion "TemplateVersion"

typedef std::map<std::string, std::string> StringMap;
typedef std::map<std::string, std::vector<std::string> > VectorMap;


struct FeedXml {
  StringMap body;
  std::string template_file;
  VectorMap ls;
  std::string template_type;
  std::string template_version;
  static const std::string tmp;
  static const std::vector<std::string> vtmp;

  const std::string & find(const char *key) const {
    StringMap::const_iterator it = body.find(key);
    if (body.end() != it)
      return it->second;

    return tmp;
  }

  std::vector<std::string> find_v(const char *key) const {
    VectorMap::const_iterator vit = ls.find(key);
    if (ls.end() != vit)
      return vit->second;

    std::vector<std::string> v;
    StringMap::const_iterator it = body.find(key);
    if (body.end() != it)
      v.push_back(it->second);

    return v;
  }
  const std::string & find(const char *key, int idx) const {
    if (idx == 0) {
      StringMap::const_iterator it = body.find(key);
      if (body.end() != it)
        return it->second;
    } else {
      VectorMap::const_iterator vit = ls.find(key);
      if (ls.end() != vit) {
        if (vit->second.size() > idx) {
          return vit->second[idx];
        } else {
          LOG(WARNING) << "feedxml ls[" << key << "][" << idx << "] empty.";
        }
      }
    }
    return tmp;
  }
};

namespace detail {

struct ParseContext {
  StringMap map;
  std::list<std::string> stack;
  std::string tmp_text;

  enum State {
    STATE_NONE,
    STATE_REPLY,
    STATE_MERGE,
  } state;

  FeedXml* data;

  ParseContext() : state(STATE_NONE), data(0) {}
};

// #define PARSE_LOG

static void StartTag(void* data, const char *name, const char **atts) {
  ParseContext* context = (ParseContext*)data;

  // 忽略添加的伪root节点
  if (strcmp(name, "feeds") == 0)
    return;

#ifdef PARSE_LOG
  std::cout << std::string(context->stack.size(), ' ') << "<" <<  name << std::endl;
#endif

  // <merge_feed><count>3</count><fid>2695065650</fid><fid>2695028012</fid><fid>2695016671</fid></merge_feed>

  if (0 == strcmp(name, "f")) {
    for (int i = 0; atts[i]; i += 2) {
      if (0 == strcmp(atts[i], "v")) {
        std::string version = atts[i+1];
        std::string::size_type pos = version.find('.');
        if (pos != std::string::npos) {
          version[pos] = '\0';
        }
        context->data->template_version = version.c_str();
        context->data->body.insert(std::make_pair(kTemplateVersion, version));
      } else if (0 == strcmp(atts[i], "tpl")) {
        std::string tpl = atts[i+1];
        context->data->template_type = tpl.c_str();
      }
    }
  }

  context->tmp_text.clear();
  context->stack.push_back(name);
}

static void EndTag(void* data, const char *name) {
   if (strcmp(name, "feeds") == 0)
     return;

  ParseContext* context = (ParseContext*)data;

  if (!context->tmp_text.empty()) {
    std::string key = boost::join(context->stack, "_"); 
    //std::cout << key<< " :" << context->tmp_text << std::endl;
    
    VectorMap::iterator it = context->data->ls.find(key);
    if (it != context->data->ls.end()) {
      it->second.push_back(context->tmp_text);
    } else {
      std::pair<StringMap::iterator, bool> ret = context->data->body.insert(std::make_pair(key, context->tmp_text));
      if ( ret.second == false ) { // body有重复
        std::vector<std::string> v;
        v.push_back(ret.first->second);
        v.push_back(context->tmp_text);
        context->data->ls.insert(std::make_pair(key, v));
        //context->data->body.erase(ret.first);
        
      } else
      {
        //Sleep(1);
      }
    }  
  }
  context->stack.pop_back();
  context->tmp_text.clear();
#ifdef PARSE_LOG
  std::cout << std::string(context->stack.size(), ' ') <<  name << "> " << context->tmp_text << std::endl;
#endif

}

static void HandleText(void* data, const XML_Char *s, int len) {
 
  ParseContext* context = (ParseContext*)data;

  std::string value(s, len);
  size_t pos = value.find_first_not_of(" \t\r\n");
  if (pos == std::string::npos)
    return;
  context->tmp_text += value;

#ifdef PARSE_LOG
    std::string key = boost::join(context->stack, "_");
    std::cout << std::string(context->stack.size(), ' ') << '#' << key << '#' << value << '#' << std::endl;
#endif
}

#ifdef PARSE_LOG
#undef PARSE_LOG
#endif

#if 0
static void HandleInstruction(void* data, const XML_Char *target, const XML_Char *xdata) {
  std::cout << "instruction:" << target  << " data:" <<  xdata << std::endl;
}

static void HandleComment(void *data, const XML_Char *xdata) {
  std::cout << "comment:" << xdata << std::endl;
}

static void HandleDecl(void *data, const XML_Char *name, XML_Content *model) {
  std::cout << "decl:" << name << model->name << std::endl;
}

static void HandleAttDecl( void *data,
                       const XML_Char  *elname,
                       const XML_Char  *attname,
                       const XML_Char  *att_type,
                       const XML_Char  *dflt,
                       int   isrequired) {
  std::cout << "decl:" << elname << " attname:" << att_type << " dflt:" << dflt << " isrequired:" << isrequired << std::endl;
}
#endif
} // xce::detail

inline int ParseCtemplateXml(const std::string& xml, FeedXml* data) {
  detail::ParseContext context;
  context.data = data;

  // XML_ParserReset(XML_Parser parser, const XML_Char *encoding);
  XML_Parser parser = XML_ParserCreate("UTF-8");
  XML_SetUserData(parser, &context);
  XML_SetElementHandler(parser, detail::StartTag, detail::EndTag);
  XML_SetCharacterDataHandler(parser, detail::HandleText);

  // XML_SetProcessingInstructionHandler(parser, detail::HandleInstruction);
  // XML_SetCommentHandler(parser, detail::HandleComment);

  // XML_SetElementDeclHandler(parser, detail::HandleDecl);
  // XML_SetAttlistDeclHandler(parser, detail::HandleAttDecl);

  XML_Status status = XML_Parse(parser, xml.c_str(),
    static_cast<int>(xml.size()), true);

  XML_Error ret = XML_ERROR_NONE;
  if (XML_STATUS_OK != status) {
    ret = XML_GetErrorCode(parser);
#if 0
    // expandfeed.cc 中已经输出了Log，这里不知道 fid, 但是知道 LineNumber
    LOG(INFO) << "xml parse error, line: " << XML_GetCurrentLineNumber(parser)
      << "error string:" << XML_ErrorString(ret);
#endif
  }

  XML_ParserFree(parser);
  return ret;
}

} // namespace
#endif
