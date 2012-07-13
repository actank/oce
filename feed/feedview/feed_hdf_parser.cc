#include "feed/feedview/feed_hdf_parser.h"

using namespace xce;

FeedHdfParser * FeedXmlSchema::CreateHdfParser() const {
  FeedHdfParser * parser = new FeedHdfParser();

  map<string, LoopNodeItems> * m = &(parser->loop_nodes_);
  m->insert(make_pair("f.reply", LoopNodeItems()));
  m->insert(make_pair("merge_feed.fid", LoopNodeItems()));

  for(set<std::string>::const_iterator i = loop_keys_.begin(); i != loop_keys_.end(); ++i) {
    //printf("index node %s added\n", i->c_str());
    m->insert(make_pair(*i, LoopNodeItems()));
  }

  parser->headurl_keys_ = headurl_keys_;

  return parser;
}

