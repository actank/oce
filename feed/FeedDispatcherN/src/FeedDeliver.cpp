#include <boost/lexical_cast.hpp>

#include "FeedDeliver.h"

//#include "FeedItemAdapter.h"
//#include "FeedNewsAdapter.h"
#include "FeedNewsReplicaAdapter.h"
//#include "FeedMiniAdapter.h"
#include "FeedMiniReplicaAdapter.h"
#include "WTransitAdapter.h"
//#include "FeedDBAdapter.h"
//#include "SSDFeedDBAdapter.h"
//#include "FeedCacheReplicaAdapter.h"
#include "FeedCacheAdapter.h"
#include "FeedCacheLogicReplicaAdapter.h"
//#include "FeedCacheNAdapter.h"
#include "FeedInteractionNAdapter.h"
//#include "../../FeedMemcProxy/client/feed_interaction_client.h"
#include "PhotoDataGatheringAdapter.h"
#include "pugixml.hpp"
#include "FeedNaiveBayesReplicaAdapter.h"
#include "FeedSinkAdapter.h"
using namespace pugi;



//#include "WEventAdapter.h" //不用
using namespace xce::feed;
using namespace MyUtil;
using namespace std;
using namespace talk::adapter;


//---------------------------------------------------------------------------
FeedDeliver::FeedDeliver() {
}

FeedDeliver::~FeedDeliver() {

}

bool FeedDeliver::initialize(const Ice::PropertiesPtr& props) {
  return true;
}

void FeedDeliver::initLogger() {
  _logger = XceLoggerAdapter::instance().getLogger(1, 32768);
  _logger->initialize();
}


bool ParseForPage(long fid,const string & xml,string & statid, string & level){
	try {
		pugi::xml_document doc;
		if (!doc.load(xml.c_str())) {
			MCE_WARN("ParseForPage. load error. fid: " << fid);
			return false;
		}
		//pugi::xml_node stat_node = doc.child("f").child("stat");
		//pugi::xml_node id_node = stat_node.child("id");
		//pugi::xml_node level_node = stat_node.child("level");
    
		pugi::xml_node id_node = doc.child("f").child("stat").child("id");
		pugi::xml_node level_node = doc.child("f").child("stat").child("level");
		//statid = id_node.value();
		//level = level_node.value();
		statid = id_node.first_child().value();
		level = level_node.first_child().value();
		//pugi::xml_node id_node = doc.child("f").child("stat").child("id");
		MCE_INFO("ParseForPage. fid: " << fid << " statid:" << statid << " level:" << level  << " xmlsize:" << xml.size());
		return true;
	} catch (Ice::Exception& e) {
		MCE_WARN("ParseForPage. err" << e);
		return false;
	}
//	replyer = id_node.first_child().value();
}

bool ParseOrigin(long fid,const string & xml,string & origin_url,string & origin_title){
	try {
		pugi::xml_document doc;
		if (!doc.load(xml.c_str())) {
			MCE_WARN("ParseOrigin. load error. fid: " << fid);
			return false;
		}

		pugi::xml_node origin_url_node = doc.child("f").child("origin").child("url");
		origin_url = origin_url_node.first_child().value();

		pugi::xml_node origin_title_node = doc.child("f").child("origin").child("title");
		origin_title = origin_title_node.first_child().value();

		BOOST_FOREACH(string::value_type & v, origin_title){
			if(v == ' ') v = '_';
		}
		MCE_INFO("ParseOrigin. fid: " << fid << " origin_url:" << origin_url << " origin_title:" << origin_title);
		return true;
	} catch (Ice::Exception& e) {
		MCE_WARN("ParseOrigin. err" << e);
		return false;
	}
//	replyer = id_node.first_child().value();
}

void CallXceLogger(FeedSeedPtr & feed,set<int> & target,XceLoggerPtr & _logger){
	int stype = feed->type & 0x0000FFFF;
  vector<string> path;
  path.push_back("feed");
  path.push_back("dispatchto");

  bool parse_ok = false;
  string statid,level;
  if(stype / 100 == 20){
    //MCE_INFO("FeedDeliver::deliver. begin parse, stype" << stype);
  	parse_ok = ParseForPage(feed->feed,feed->xml,statid,level);
  }else{
    //MCE_INFO("FeedDeliver::deliver. no parse, stype" << stype);
  }

  ostringstream os;
	time_t seconds;
	seconds = time (NULL);
  os << "d " << seconds << " "<<feed->actor << " " << (feed->type & 0x0000FFFF) << " " << feed->feed;

  if(parse_ok && (!statid.empty() || !level.empty())){
  	os << " s" << statid << " l" << level;
  }
  size_t i = 0;
  for (set<int>::iterator sit = target.begin(); sit != target.end(); ++i, ++sit) {
    os << " " << *sit;
    if ((i % 2500 == 0 && i != 0) || i == (target.size() - 1)) { //只打印前2500个
      try {
        _logger->addLog(path, os.str());
        os.str("");
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDeliver::deliver --> log err, " << e);
      }
    }
  }
}

void CallXceLoggerV2(FeedSeedPtr & feed,set<int> & target,XceLoggerPtr & _logger){
	int stype = feed->type & 0x0000FFFF;
  vector<string> path;
  path.push_back("feed");
  path.push_back("dispatchtoV2");

  ostringstream os;
	time_t seconds;
	seconds = time (NULL);
  os << "dV2 time:" << seconds
  		<< " fid:" << feed->feed
  		<< " source:" << (feed->source)
  		<< " stype:" << (feed->type & 0x0000FFFF)
  		<< " actor:"<<feed->actor
  		<< " newsmerge:" << feed->newsMerge
  		<< " minimerge:" << feed->miniMerge
  		<< " targetsize:" << target.size()
  		;
  string origin_url,origin_title;
  string head = os.str();
  os << " targets:" << PrintHelper::Print(target,2500);
  ParseOrigin(feed->feed,feed->xml,origin_url,origin_title);
	if(!origin_url.empty())
		os << " originurl:" << origin_url;
	if(!origin_title.empty()){
		os << " origin_title:" << origin_title;
	}

  string statid,level;
  if(stype / 100 == 20){
  	ParseForPage(feed->feed,feed->xml,statid,level);
    if(!statid.empty())
    	os << " statid:" << statid;
    if(!level.empty())
    	os << " level:" << level;
  }


  try {
    _logger->addLog(path, os.str());
    MCE_INFO("CallXceLoggerV2. succ head:" <<head);
  } catch (Ice::Exception& e) {
    MCE_WARN("CallXceLoggerV2. fail head:" <<head << " log err, " << e);
  }

}
void FeedDeliver::deliver(FeedSeedPtr & feed, FeedConfigPtr & config,
    set<int> & target, map<int, int> & rank, DispatchCost & cost) {
  int stype = feed->type & 0x0000FFFF;
  if(feed->xml.empty()){
    MCE_INFO("FeedDeliver::deliver. xml empty. fid:" << feed->feed << " stype:" << stype);
  }

  //----to mini
  TimeStat t1;
    deliverToMini(feed, config);
    cost.mini_ = t1.getTime();
  if(target.empty()){
    return;
  }

  CallXceLogger(feed,target,_logger);
  CallXceLoggerV2(feed,target,_logger);
//  vector<string> path;
//  path.push_back("feed");
//  path.push_back("dispatchto");
//
//  bool parse_ok = false;
//  string statid,level;
//  if(stype / 100 == 20){
//    //MCE_INFO("FeedDeliver::deliver. begin parse, stype" << stype);
//  	parse_ok = ParseForPage(feed->feed,feed->xml,statid,level);
//  }else{
//    //MCE_INFO("FeedDeliver::deliver. no parse, stype" << stype);
//  }
//
//  ostringstream os;
//	time_t seconds;
//	seconds = time (NULL);
//  os << "d " << seconds << " "<<feed->actor << " " << (feed->type & 0x0000FFFF) << " " << feed->feed;
//
//  if(parse_ok && (!statid.empty() || !level.empty())){
//  	os << " s" << statid << " l" << level;
//  }
//  size_t i = 0;
//  for (set<int>::iterator sit = target.begin(); sit != target.end(); ++i, ++sit) {
//    os << " " << *sit;
//    if ((i % 2500 == 0 && i != 0) || i == (target.size() - 1)) { //只打印前2500个
//      try {
//        _logger->addLog(path, os.str());
//        os.str("");
//      } catch (Ice::Exception& e) {
//        MCE_WARN("FeedDeliver::deliver --> log err, " << e);
//      }
//    }
//  }
  cost.log_ = t1.getTime();


  /*和下一段代码功能相同
   const static unsigned int kBatchSize = 50000;
   vector<int> in, partIn;
   in.assign(target.begin(), target.end());
   for (vector<int>::iterator vit = in.begin(); vit < in.end(); vit += kBatchSize) {
   partIn.assign(vit, vit + kBatchSize < in.end() ? vit + kBatchSize : in.end());
   try {
   //talk::adapter::WTransitAdapter::instance().deliverFeed(feed,partIn);
   } catch (Ice::Exception& e) {
   MCE_WARN("FeedDeliver::deliver --> invoke WTransit err, " << e);
   }
   try {
   set<int> tmpset(in.begin(),in.end());
   deliverToNews(feed, config, tmpset, rank);
   } catch (Ice::Exception& e) {
   MCE_WARN("deleiver to FeedItem failed: feedid=" <<feed->feed<<" reason: " << e);
   }
   }
   */
//  try{
//    FeedInteractionNAdapter::instance().AddFeedInteraction(feed->feed, feed->actor, target.size(), feed->time);
//  } catch (Ice::Exception& e){
//    MCE_WARN("FeedDeliver::deliver --> call FeedInteractionN err, " << e);
//  } catch (...){
//    MCE_WARN("FeedDeliver::deliver --> call FeedInteractionN err");
//  }

  //FeedInteractionMemcClient::instance().AddInteraction(feed->feed,target.size());
  cost.interaction_ = t1.getTime();
  const static unsigned int maxSize = 50000;
  size_t targetPartCount = 0;
  set<int> targetPart;
  set<int>::iterator itr = target.begin();
  while (itr != target.end()) {
    targetPart.insert(*itr);
    ++targetPartCount;
    ++itr;
    if (targetPart.size() >= maxSize || targetPartCount == target.size()) {
      try {
        MyUtil::IntSeq targetVector;
        targetVector.assign(targetPart.begin(), targetPart.end());

        talk::adapter::WTransitAdapter::instance().deliverFeed(feed,targetVector); //发送给WTransit
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDeliver::deliver --> invoke WTransit err, " << e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << (feed->type & 0xFFFF) << ",actor = " << feed->actor);
      }
      cost.wtran_ += t1.getTime();
      //to news
      try {
        deliverToNews(feed, config, targetPart, rank);
      } catch (Ice::Exception& e) {
        MCE_WARN("deleiver to FeedItem failed :  " << e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << (feed->type & 0xFFFF) << ",actor = " << feed->actor);
      }
      cost.news_ += t1.getTime();
      //to photos
      int stype = feed->type & 0xFFFF;
      if (stype == 701 || stype == 709) {
        deliverToPhotos(feed, config, targetPart, rank);
      }
      cost.photo_ += t1.getTime();
      targetPart.clear();
    }

  }
}


void FeedDeliver::deliverToNews(FeedSeedPtr feed, FeedConfigPtr config,
    set<int> & target, map<int, int> & rank) {
	MCE_INFO("FeedDeliver::deliverToNews --> source = " << feed->source << ",actor = " << feed->actor << ",stype = " << (feed->type & 0xFFFF) << ",target size = " << target.size() );
  if (config->sendNewsConfig) {
    FeedItem item;
    item.feed = feed->feed;
    item.type = feed->type;
    item.time = feed->time;
    item.weight = feed->baseWeight;
    item.merge = feed->newsMerge;
    item.actor = feed->actor;

    int stype = item.type & 0xFFFF;
    MyUtil::Int2IntMap id2Weight;

    //int freq = 0;
    //try{
    //	freq = FeedNaiveBayesAdapter::instance().GetSendFrequency(feed->actor);
    //	freq = freq?freq <= 100: 100;
    //	MCE_INFO("FeedDeliver::deliverToNew. get freq. actor:" << feed->actor
    //			<< " fid:" << feed->feed
    //			<< " freq:" << freq);
    //}catch(Ice::Exception &e){
    //	MCE_WARN("FeedDeliver::deliverToNew. get freq error. actor:" << feed->actor
    //			<< " fid:" << feed->feed
    //			<< " error: " << e);
    //}
    for (set<int>::iterator itr = target.begin(); itr != target.end(); ++itr) {
      map<int, int>::iterator mit = rank.find(*itr);
      if (mit != rank.end()) {
        id2Weight[*itr] = item.weight
            | ((mit->second << 24) & 0xff000000);
      } else {
        id2Weight[*itr] = item.weight;
      }
      //if(freq){
      //	id2Weight[*itr] = id2Weight[*itr] | freq;
      //}
    }

    try {
      //FeedItemAdapter::instance().addFeed(item, config, id2Weight);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedDeliver::deliverToNews --> call FeedItemAdapter err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor );
    }
  
    try {
//      FeedNewsAdapter::instance().addFeed(item, config, id2Weight);//TODO
      FeedNewsReplicaAdapter::instance().addFeed(item, config, id2Weight);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedDeliver::deliverToNews --> call FeedNews err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
    }
     
    try {
      FeedSinkAdapter::instance().AddFeed(item, config, id2Weight);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedDeliver::deliverToNews --> call FeedSink err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
    }

    if (!(stype==8189 || stype==8190 || stype==401 || stype==8181 || stype == 1702 || stype==8011 || (stype >= 8001 && stype <= 8005))) {
      /*try {
        FeedDBAdapter::instance().put(item, id2Weight);
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDeliver::deliverToNews --> call FeedDBAdapter err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
      }
      try {
        SSDFeedDBAdapter::instance().put(item, id2Weight);
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDeliver::deliverToNews --> call SSDFeedDBAdapter err:"<<e<< ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
      }

      try {
        FeedCacheAdapter::instance().put(item, id2Weight);
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDeliver::deliverToNews --> call FeedCacheAdapter err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
      }*/
    } else {
      MCE_INFO("feed:"<<item.feed<<" stype:"<<stype  << ",source = " << feed->source << ",actor = " << feed->actor << " expr:"<<config->expr<<" is not save to db");
    }
    try {
      FeedCacheLogicReplicaAdapter::instance().put(item, id2Weight, true);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedDeliver::deliverToNews --> call FeedCacheLogicReplicaAdapter err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
    }
  } else {
    MCE_INFO("FeedDeliver::deliverToNews --> not deliver to FeedItem, feedid="<<feed->feed << ",actor = " << feed->actor);
  }
}

void FeedDeliver::deliverToMini(FeedSeedPtr feed, FeedConfigPtr config) {

	MCE_INFO("FeedDeliver::deliverToMini --> source = " << feed->source << ",actor = " << feed->actor << ",stype = " << (feed->type&0xFFFF) );
  if (config->sendMiniConfig) {
    FeedItem item;
    item.feed = feed->feed;
    item.type = feed->type;
    item.time = feed->time;
    item.weight = feed->baseWeight;
    item.merge = feed->miniMerge;
    item.actor = feed->actor;

    int stype = item.type & 0xFFFF;
    MyUtil::Int2IntMap id2Weight;
    id2Weight[feed->actor] = item.weight;
    try {
//      FeedMiniAdapter::instance().addFeed(item, config, id2Weight);
      FeedMiniReplicaAdapter::instance().addFeed(item, config, id2Weight);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedDeliver::deliverToMini --> call FeedMiniAdapter err:"<<e << ",feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
    }

  } else {
    int stype = feed->type & 0xFFFF;
    MCE_INFO("FeedDeliver::deliverToMini --> sendMiniConfig is false,not deliver to FeedMini ,feedid = " << feed->feed << ",source = " << feed->source << ",stype = " << stype << ",actor = " << feed->actor);
  }
}




void FeedDeliver::deliverToPhotos(FeedSeedPtr feed, FeedConfigPtr config,
    set<int> & target, map<int, int> & rank) {

  MCE_INFO("FeedDeliver::deliverToPhotos --> source = " << feed->source << ",actor = " << feed->actor << ",stype = " << (feed->type & 0xFFFF) << ",target size = " << target.size()<<" line:"<<__LINE__ );

    MyUtil::Int2IntMap id2Weight;
    for (set<int>::iterator itr = target.begin(); itr != target.end(); ++itr) {
      map<int, int>::iterator mit = rank.find(*itr);
      if (mit != rank.end()) {
        id2Weight[*itr] = feed->baseWeight | ((mit->second << 24) & 0xff000000);
      } else {
        id2Weight[*itr] = feed->baseWeight;
      }
    }
    MyUtil::LongSeq photoids;
    string xml = feed->xml;
    int ownerid = feed->actor;

    xml_document doc;
    doc.load(xml.c_str());

    xml_node f = doc.child("f");
    xml_node album = f.child("album");
    for (xml_node photo = album.child("photo"); !photo.empty(); photo= photo.next_sibling("photo")) {
      string str_id = photo.child("id").first_child().value();
      long id = atol(str_id.c_str());
      photoids.push_back(id);
      MCE_INFO("FeedDeliver::deliverToPhotos photoid: "<<id<< " line: "<<__LINE__);
    }
    try {
      PhotoDataGatheringAdapter::instance().GatherPhotoData(ownerid,photoids, id2Weight);
    } catch (Ice::Exception& e) {
       MCE_INFO("FeedDeliver::deliverToPhotos -- ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    }
}


