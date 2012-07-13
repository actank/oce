#include "FeedPassage.h"


//#include "WTransitI.h"

#include "MessageType.h"
#include "TalkDeliverAdapter.h"
#include "TalkUtil/src/XmppTools.h"
#include "TalkUtil/src/pugixml.hpp"
#include "QueryRunner.h"
#include "OnlineCenterAdapter.h"
#include "IceLogger.h"
//#include "NotifyRplAdapter.h"
#include <boost/lexical_cast.hpp>
#include <util/cpp/String.h>
//#include "FeedDBAdapter.h"
#include "FeedContentAdapter.h"
#include "TransitAider.h"
#include "MsgFilterAdapter.h"
#include "FeedNewsAdapter.h"
#include "WTransitI.h"
#include "NotifyDispatcherAdapter.h"
#include "UrlCode.h"

using namespace talk::wtransit;
using namespace pugi;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace ::xce::feed;
using namespace ::com::xiaonei::xce;
//using namespace mop::hi::svc::dispatch;
using namespace talk::online::adapter;
using namespace com::xiaonei::antispam::wap::generation;
using namespace boost;

////////////////////////// FeedPipe ////////////////////////////////////////////////////////


void FeedPipe::handle(){
	TimeStat ts;
	//_msgs.clear();
	FeedToMessagConverter converter;
	FeedWithTargetSeq objs;
	WTransitManagerI::instance().SwapFeedSeq(objs);
	if(objs.empty()){
		return;
	}
	for (size_t x = 0; x < objs.size(); ++x) {
	  MessageSeq msgs;
		//FeedWithTargetPtr feed = FeedWithTargetPtr::dynamicCast(objs.at(x));
		FeedWithTargetPtr feed = objs.at(x);
		MyUtil::IntSeq target;
		target.swap(feed->target);

		converter.setData(feed);

		MCE_DEBUG(" FeedPipe::handle --> _feed->target.size = "<< target.size());
		JidSeqMap jmap;
		try {
			jmap = OnlineCenterAdapter::instance().getUsersJids(target);

		} catch (Ice::Exception& e) {
			MCE_WARN("FeedPipe::handle-->OnlineCenterAdapter::getUsersJids-->" << target.size() << " " <<e);
		} catch (std::exception& e) {
			MCE_WARN("FeedPipe::handle --> online center err: " << target.size() << " "<<e.what());
		}
		if (jmap.empty()) {
			MCE_DEBUG("FeedPipe::handle --> no friend online  ");
			continue;
		}
		MCE_DEBUG("FeedPipe::handle --> jmap.size = "<<jmap.size());


		for (size_t i = 0; i < target.size(); ++i) {
			JidSeqMap::const_iterator it = jmap.find(target.at(i));
			if (it == jmap.end()) {
				MCE_DEBUG("FeedPipe::handle --> can not find jmap _feed->target(i)="<<target.at(i));
				continue;
			}
			for (size_t j = 0; j < it->second.size(); ++j) {
				MessagePtr msg;
				if ( (!it->second.at(j)->endpoint.empty()) ){
					if( it->second.at(j)->endpoint[0] == 'P' ){//新鲜事 to phone
						msg = converter.getToPhoneMessage(it->second.at(j));
					} else if( it->second.at(j)->endpoint[0] == 'T' ){// 新鲜事 to talk
						msg = converter.getToTalkMessage(it->second.at(j), false);
					} else if( it->second.at(j)->endpoint[0] == 'W' ){// 新鲜事 to web pager
            //if(it->second.at(j)->userId!=feed->actor && (it->second.at(j)->userId%10==5||it->second.at(j)->userId==255329305||it->second.at(j)->userId==241610785||it->second.at(j)->userId==238489851||it->second.at(j)->userId==128487631||it->second.at(j)->userId==232626782)){ 
						if(it->second.at(j)->userId!=feed->actor){
							MessagePtr tmp_msg = new Message;
							tmp_msg->from = new Jid();
							tmp_msg->from->userId = feed->actor;
							tmp_msg->to = it->second.at(j);
							tmp_msg->type = FEED_STATUS;
							ostringstream os;
							os << "<message type=\"common\" from=\"xfeed@talk.renren.com\"><body>{feed_id:'" << feed->feed << "'}</body></message>";
							tmp_msg->msg = os.str();
							msgs.push_back(tmp_msg);
						}
          }
					if(msg){
							msgs.push_back(msg);
					}
				}
			}
		}
	  if (!msgs.empty()) {
		  try{
        TalkDeliverAdapter::instance().deliver(msgs);
      }catch(Ice::Exception& e){
        MCE_WARN("FeedPipe::handle-->TalkDeliverAdapter::deliver" << "size:" << msgs.size() << " " << e);
      }
	  }
  }
	MONITOR("SignForTimeuse::WTransit::FeedPipe::handle-->" << ts.getTime() << "@WTransitManagerI::deliverFeed-->time" << ts.getTime() << "|" << "count" << objs.size());
	FunStatManager::instance().Stat("FeedPipe::handle", ts.getTime(), (ts.getTime() > 17.0));
}


/////////////////////////////// FeedWithTarget ////////////////////////////////////////////

// FeedWithTarget::FeedWithTarget(const FeedSeedPtr& feed, const MyUtil::IntSeq& targetSeq){
// 	this->feed = _feed->feed;
// 	source = _feed->source;
// 	smallType = _feed->type & 0x00FFFFFF;
// 	bigType = int((_feed->type & 0x00FFFFFF) / 100);
// 	actor = _feed->actor;
// 	feedxml = _feed->xml;
// 	time = _feed->time;
// 	replyCount = 0;
// 	this->target.swap(const_cast< MyUtil::IntSeq&>(targetSeq));
// 	this->target.push_back(actor);
// }


FeedWithTarget::FeedWithTarget(const FeedSeedPtr& feed, const MyUtil::IntSeq& targetSeq){
	this->feed = feed->feed;
	source = feed->source;
	smallType = feed->type & 0xFFFF;
	bigType = int((feed->type & 0xFFFF) / 100);
	actor = feed->actor;
	feedxml = feed->xml;
	time = feed->time;
	replyCount = 0;
	this->target.swap(const_cast< MyUtil::IntSeq&>(targetSeq));
	//this->target.push_back(actor);
}


FeedWithTarget::FeedWithTarget(const FeedContentPtr& content){
	this->feed = content->data->feed;
	source = content->data->source;
	smallType = content->data->type & 0xFFFF;
	bigType = int((content->data->type & 0xFFFF) / 100);
	actor = content->data->actor;
	feedxml = content->data->xml;
	time = content->data->time;
	if(content->reply){
		replyCount = content->reply->count;
	} else {
		replyCount = 0;
	}
	
	// _jfrom = 0;
	// _toPhoneMsg.load("");
	// _toTalkMasg.load("");
	// toWebPagerMsg.load("");
}





/////////////////////////////// FeedToMessagConverter ////////////////////////////////////////////


void FeedToMessagConverter::setData(const FeedContentPtr& content){
	MCE_DEBUG("build FeedToMessagConverter using FeedContent");
	_feed = new FeedWithTarget(content);
	_jfrom = new Jid();
	_jfrom->userId = _feed->smallType;
	_jfrom->endpoint = "feed.talk.xiaonei.com";
	_jfrom->index = -1;
	_toPhoneMsg.clear();
//	_toTalkMsg.clear();
//	_toPhoneMsg.load("");
	_toTalkMasg.load("");
	_toTalkMasgReady = false;
	MCE_DEBUG("FeedToMessagConverter::setData --> feedid="<<_feed->feed);
	MCE_DEBUG("FeedToMessagConverter::setData --> feed xml :"<<_feed->feedxml.c_str());
}
void FeedToMessagConverter::setData(const FeedWithTargetPtr& feed){
	this->_feed = feed;
	_jfrom = new Jid();
	_jfrom->userId = _feed->smallType;
	_jfrom->endpoint = "feed.talk.xiaonei.com";
	_jfrom->index = -1;
	_toPhoneMsg.clear();
//	_toTalkMsg.clear();
	// _toPhoneMsg.load("");
	_toTalkMasg.load("");
	_toTalkMasgReady = false;
	MCE_DEBUG("FeedToMessagConverter::setData --> feed xml :"<<_feed->feedxml.c_str());
}



string FeedToMessagConverter::_getType(int stype){
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.TypeName." << stype;
	string ret = WTransitManagerI::instance().GetServerProperties(key.str());
	return ret;
	//return service.getCommunicator()->getProperties()->getPropertyWithDefault( key.str(), "");
}


string FeedToMessagConverter::_getSType(int type){
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.STypeName." << type;
	string ret = WTransitManagerI::instance().GetServerProperties(key.str());
	return ret;
	//return service.getCommunicator()->getProperties()->getPropertyWithDefault( key.str(), "");
}


string FeedToMessagConverter::_getTemplate(int stype){
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.Template." << stype;
	string ret = WTransitManagerI::instance().GetServerProperties(key.str());
	//string ret = service.getCommunicator()->getProperties()->getPropertyWithDefault(key.str(), "");
	MCE_DEBUG("FeedToMessagConverter::getTemplate -> " << key.str() <<" template: "<<ret);
	return ret;
			
}

// JidPtr FeedWithTarget::_get_Jfrom(){
// 	if( !_jfrom ){
// 		_jfrom = new Jid();
// 		_jfrom->userId = smallType;
// 		_jfrom->endpoint = "feed.talk.xiaonei.com";
// 		_jfrom->index = -1;
// 	}
// 	return _jfrom;
// }

MessagePtr FeedToMessagConverter::getToPhoneMessage( const JidPtr& to ){
	MCE_DEBUG("FeedToMessagConverter::getToPhoneMessage");
	if( !_jfrom ){
		MCE_DEBUG("FeedToMessagConverter::getToPhoneMessage --> FeedToMessagConverter::setData should be called first");
		return 0;
	}
	//---for phone---
	ostringstream os;
	if ( _feed->smallType == 102 ||
		 _feed->smallType == 103 ||
		 _feed->smallType == 502 ||
		 _feed->smallType == 601 ||
		 _feed->smallType == 701 ||
		 _feed->smallType == 1101 ||
		 _feed->smallType == 104) {
		if(!_toPhoneMsg.empty()){
			MessagePtr msg = new Message();
			msg->from = _jfrom;//_get_Jfrom();
			msg->to = to;
			msg->type = PHONE_NOTIFY;
			msg->msg = _toPhoneMsg;
			return msg;
		}
		xml_document xml;
		if (!xml.load(_feed->feedxml.c_str())) {
			return false;
		}
		os << "<message>" << "<xfeed>" << "<feedid>" << _feed->feed
		   << "</feedid>" << "<type>" << _feed->smallType << "</type>"
		   << "<time>"
		   << xml.child("f").child("time").first_child().value()
		   << "</time>" << "<from>" << "<id>"
		   << xml.child("f").child("from").child("id").first_child().value() << "</id>"
		   << "<name>"
		   << xml.child("f").child("from").child("name").first_child().value()
		   << "</name>" << "</from>";
		
		if ( _feed->smallType == 102 ||
			 _feed->smallType == 103 ||
			 _feed->smallType == 104) {
			//os << "<sid>"<<xml.child("f").child("sID").first_child().value()<<"</sid>";
			if (_feed->smallType == 102) {
				string
					data =
					xml.child("f").child("share").child("blog").child("url").first_child().value();
				os << "<id>" << UrlParser::getAid(data) << "</id>"
				   << "<title>" << xml.child("f").child("share").child("blog").child(
					"title").first_child().value() << "</title>"
				   << "<friend>" << "<id>"
				   << xml.child("f").child("share").child("blog").child("owner").child("id").first_child().value()
				   << "</id>" << "<name>"
				   << xml.child("f").child("share").child("blog").child("owner").child("name").first_child().value()
				   << "</name>" << "</friend>";
			} else if (_feed->smallType == 103) {
				xml_node node;
				node = xml.child("f").child("share").child("album");
				os << "<id>" <<node.child("id").first_child().value()<< "</id>"
				   << "<picid>" << node.child("photo").child("id").first_child().value()<< "</picid>"
				   << "<albumname>" << node.child("title").first_child().value() << "</albumname>"
				   << "<friend>"
				   << "<id>"<< node.child("owner").child("id").first_child().value()<< "</id>"
				   << "<name>"<< node.child("owner").child("name").first_child().value()<< "</name>"
				   << "</friend>";
			} else if (_feed->smallType == 104) {
				xml_node node;
				node = xml.child("f").child("share").child("album");
				string data = node.child("url").first_child().value();
				os << "<id>" << UrlParser::getId(data, "album-") << "</id>"
				   << "<albumname>" << node.child("title").first_child().value() << "</albumname>"
				   << "<friend>" << "<id>"
				   << node.child("owner").child("id").first_child().value()
				   << "</id>" << "<name>"
				   << node.child("owner").child("name").first_child().value()
				   << "</name>" << "</friend>";
			}
			
		} else if (_feed->smallType == 502 ) {
			xml_node node;
			node = xml.child("f").child("status");
			os << "<id>"
			   << node.child("id").first_child().value()
			   << "</id>" << "<content>"
			   <<node.child("title").first_child().value() << "</content>";
			xml_node lbs_node = xml.child("f").child("lbs");
			if(lbs_node){
			  os << "<place>";
			  os << " <lbs_id>" << lbs_node.child("id").first_child().value() << "</lbs_id>";
			  /*string places = lbs_node.child("pID").first_child().value();
			  if(places.empty()){
			    places = "0";
			  }*/
			  os << "<places_id>0</places_id>";
			  os << "<name>" << lbs_node.child("content").first_child().value() << "</name>";
			  os << "<longtitude>" << lbs_node.child("longitude").first_child().value() << "</longtitude>";
			  os << "<latitude>" << lbs_node.child("latitude").first_child().value() <<"</latitude>";
			  os << "<url>http://map.renren.com/place/0</url>";
			  os << "</place>";
			}
		}
		else if(_feed->smallType == 1101){
      xml_node signin_node = xml.child("f").child("signin");
      if(signin_node){
        os << "<id>"
           << signin_node.child("id").first_child().value()
           << "</id>" << "<content>"
           <<signin_node.child("comment").first_child().value() << "</content>";
        os << "<place>";
        os << " <lbs_id>0</lbs_id>";
        os << "<places_id>" << signin_node.child("pID").first_child().value() << "</places_id>";
        os << "<name>" << signin_node.child("pName").first_child().value() << "</name>";
        os << "<longtitude>" << signin_node.child("longitude").first_child().value() << "</longtitude>";
        os << "<latitude>" << signin_node.child("latitudes").first_child().value() <<"</latitude>";
        os << "<url>http://map.renren.com/place/" << signin_node.child("pID").first_child().value() << "</url>";
        os << "</place>";
      }
		}
		else if (_feed->smallType == 601) {
			xml_node node;
			node = xml.child("f").child("blog");
			os << "<id>" << node.child("id").first_child().value() << "</id>" << "<title>"
			   << node.child("title").first_child().value()
			   << "</title>";
		} else if (_feed->smallType == 701) {
			xml_node node;
			node = xml.child("f").child("album");
			os << "<id>" << node.child("id").first_child().value() << "</id>"
			   << "<albumname>"
			   << node.child("title").first_child().value()
			   << "</albumname>" << "<picid>"
			   << node.child("photo").child("id").first_child().value() << "</picid>" << "<count>"
			   << node.child("count").first_child().value()
			   << "</count>";
		}
		os << "</xfeed>" << "</message>";
	} else {
		MCE_DEBUG("FeedToMessagConverter::getToPhoneMessage --> do not push stype="<<_feed->smallType<<" to phone");
		return 0;
	}
	xml_document phoneMessage;
	if (!phoneMessage.load(os.str().c_str())) {
		MCE_WARN("FeedToMessagConverter::getToPhoneMessage --> loade xml error: xml="<<os.str());
		MCE_WARN("FeedToMessagConverter::_toPhone --> load xml error:  xml=\n"<<os.str());
		return 0;
	}

	if(_feed->smallType == 1101){
    xml_node xfeed_node = phoneMessage.child("message").child("xfeed");
    if(xfeed_node){
      xfeed_node.set_name("signin");
    }
  }

	_toPhoneMsg = phoneMessage.xml();
	MessagePtr msg = new Message();
	msg->from = _jfrom;//_get_Jfrom();
	msg->to = to;
	msg->type = PHONE_NOTIFY;
	msg->msg = _toPhoneMsg;
	return msg;
}


MessagePtr FeedToMessagConverter::getToWebPagerMessage(  const JidPtr& to ){
	if( _feed->smallType != 502 ){
		return 0;
	}
	MessagePtr msg = getToTalkMessage(to);
	if(!msg){
		return 0;
	}
	msg->type = FEED_STATUS;
	return msg;
}

string FeedToMessagConverter::_buildFNode(){
	MCE_DEBUG("call FeedToMessagConverter::_buildFNode actor = "<<_feed->actor<<"smallType="<<_feed->smallType);
	if( !( _feed->smallType == 601 ||
		   _feed->smallType == 502 ||
		   _feed->smallType == 701 ||
		   _feed->smallType == 709 ||
		   _feed->smallType == 702 ||
		   (_feed->smallType>=101 && _feed->smallType<=115) ||
		   _feed->smallType == 110 ||
		   _feed->smallType == 801 ||
		   _feed->smallType == 802 ||
		   _feed->smallType == 507 ||
		   _feed->smallType == 602 ||
		   _feed->smallType == 704
			)
		){
		return _feed->feedxml;
	}

	xml_document xml;
	if (!xml.load(_feed->feedxml.c_str())) {
		MCE_DEBUG("FeedToMessagConverter::_buildFNode --> load xml error:  xml=\n"<<_feed->feedxml.c_str());
		return "<f>xml error from WTransitI</f>";
	}
	ostringstream os;
	xml_document fnode;
	xml_node node;
	xml_node tmpNode;
	string photoId;
	string nodeName ;
	pugi::xml_node_iterator iter;
	pugi::xml_node_iterator tmp;
	string shareId;
	string ownerId;
	string link;
  int pos = 0;
	size_t album_pos;	
	string web_title;
	string re_title;

  node = xml.child("f").append_child();
  node.set_name("count");
  os<<_feed->replyCount;
  node.append_child(node_pcdata).set_value(os.str().c_str());
  os.str("");

  switch (_feed->smallType) {
	case 601: 
		//<f><blog><id>  添加到  <f><blogID>
		node = xml.child("f").append_child();
		node.set_name("blogID");
		node.append_child(node_pcdata).set_value(
			xml.child("f").child("blog").child("id").first_child().value().c_str()
			);
		//添加<f><type>
		node = xml.child("f").append_child();
		node.set_name("type");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
	case 502:
		//<f><status><id>  添加到 <f><dID>
		node = xml.child("f").append_child();
		node.set_name("dID");
		node.append_child(node_pcdata).set_value(xml.child("f").child("status").child("id").first_child().value().c_str());
		//添加  <f><stype>
		node = xml.child("f").append_child();
		node.set_name("stype");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
//		os<<xml.child("f").child("status").child("id").first_child().value()<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
	case 701:
  {
		album_pos = (xml.xml()).find("<album>");
		if(album_pos == string::npos){
			MCE_WARN("FeedToMessagConverter::_buildFNode --> 701 no album node! xml:" << xml.xml());
			return "<f>xml error from WTransitI</f>";
		}
    pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
    pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
    if (type_attribute==NULL && tpl_attribute!=NULL) {
      pugi::xml_node f_album_title = xml.child("f").child("album").append_child();     //f-album-title
      f_album_title.set_name("title");
      f_album_title.append_child(node_pcdata).set_value(xml.child("f").child("album").child("name").first_child().value().c_str());
      xml.child("f").child("album").child("count").first_child().set_value("1");       //f-album-count

      pugi::xml_node f_album_photo = xml.child("f").child("album").append_child();     //f-album-photo
      f_album_photo.set_name("photo");
      pugi::xml_node f_album_photo_image = f_album_photo.append_child();       //f-album-photo-image
      f_album_photo_image.set_name("image");
      f_album_photo_image.append_child(node_pcdata).set_value(xml.child("f").child("photo").child("hUrl").first_child().value().c_str());
      pugi::xml_node f_album_photo_id = f_album_photo.append_child();          //f-album-photo-id
      f_album_photo_id.set_name("id");
      f_album_photo_id.append_child(node_pcdata).set_value(xml.child("f").child("photo").child("id").first_child().value().c_str());
      pugi::xml_node f_album_photo_bigimage = f_album_photo.append_child();      //f-album-photo-bigImage
      f_album_photo_bigimage.set_name("bigImage");
      f_album_photo_bigimage.append_child(node_pcdata).set_value(xml.child("f").child("photo").child("lUrl").first_child().value().c_str());
      pugi::xml_node f_album_photo_mainimage = f_album_photo.append_child();      //f-album-photo-mainImage
      f_album_photo_mainimage.set_name("mainImage");
      f_album_photo_mainimage.append_child(node_pcdata).set_value(xml.child("f").child("photo").child("mUrl").first_child().value().c_str());
      pugi::xml_node f_album_photo_digest = f_album_photo.append_child();        //f-album-photo-digest
      f_album_photo_digest.set_name("digest");
      f_album_photo_digest.append_child(node_pcdata).set_value(xml.child("f").child("photo").child("title").first_child().value().c_str());
      pugi::xml_node f_album_photo_imdigest = f_album_photo.append_child();       //f-album-photo-imDigest
      f_album_photo_imdigest.set_name("imDigest");
      f_album_photo_imdigest.append_child(node_pcdata).set_value(xml.child("f").child("photo").child("title").first_child().value().c_str());
      pugi::xml_node f_album_photo_url = f_album_photo.append_child();            //f-album-photo-url
      f_album_photo_url.set_name("url");
    	os<<PHOTO_SERVER<<"/getphoto.do?id="<<_feed->source
	  	   <<"&owner="<<_feed->actor;
    	f_album_photo_url.append_child(node_pcdata).set_value(os.str().c_str());
    } else {
  	  //添加 <f><album><photo><url>
    	node = xml.child("f").child("album").child("photo").append_child();
    	node.set_name("url");
    	os<<PHOTO_SERVER<<"/getphoto.do?id="<<_feed->source
	  	   <<"&owner="<<_feed->actor;
    	node.append_child(node_pcdata).set_value(os.str().c_str());
    }
   	os.str("");

		//<f><album><id>添加到<f><pId>
		photoId = xml.child("f").child("album").child("photo").child("id").first_child().value();
		node = xml.child("f").append_child();
		node.set_name("pId");
		node.append_child(node_pcdata).set_value(photoId.c_str());
		//添加  <f><type>
//		node = xml.child("f").append_child();
//		node.set_name("type");
//		os<<_feed->bigType;
//		node.append_child(node_pcdata).set_value(os.str().c_str());
//		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<photoId<<"_"<<_feed->smallType<<"_"<<_feed->actor;
//		os<<xml.child("f").child("album").child("id").first_child().value()<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");

		return xml.xml();
  }
	case 702:
		album_pos = (xml.xml()).find("<album>");
		if(album_pos == string::npos){
			MCE_WARN("FeedToMessagConverter::_buildFNode --> 702 no album node! xml:" << xml.xml());
			return "<f>xml error from WTransitI</f>";
		}
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
	
	case 709:
  {
      pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
      pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
      if (type_attribute==NULL && tpl_attribute!=NULL) {
        pugi::xml_node f_album_title = xml.child("f").child("album").append_child();
        f_album_title.set_name("title");
        f_album_title.append_child(node_pcdata).set_value(xml.child("f").child("album").child("name").first_child().value().c_str());
        xml.child("f").child("album").child("count").first_child().set_value(xml.child("f").child("upload").child("count").first_child().value().c_str());
      } 


      if (type_attribute==NULL && tpl_attribute!=NULL)
        node = xml.child("f");
      else
  	  	node = xml.child("f").child("album");
	  	if(!node){
	  		return xml.xml();
	  	}
	  	iter = node.begin();
	  	for(int k=0; k<4 && iter!=node.end();iter++){
		  	nodeName = iter->name();
		  	if( nodeName.compare("photo") == 0 ){
          if (type_attribute==NULL && tpl_attribute!=NULL) {
            pugi::xml_node f_album_photo = xml.child("f").child("album").append_child();     //f-album-photo
            f_album_photo.set_name("photo");
            pugi::xml_node f_album_photo_image = f_album_photo.append_child();          //f-album-photo-image
            f_album_photo_image.set_name("image");
            f_album_photo_image.append_child(node_pcdata).set_value((iter->child("hUrl")).first_child().value().c_str());
            pugi::xml_node f_album_photo_id = f_album_photo.append_child();           //f-album-photo-id
            f_album_photo_id.set_name("id");
            f_album_photo_id.append_child(node_pcdata).set_value((iter->child("id")).first_child().value().c_str());
            pugi::xml_node f_album_photo_bigimage = f_album_photo.append_child();      //f-album-photo-bigImage
            f_album_photo_bigimage.set_name("bigImage");
            f_album_photo_bigimage.append_child(node_pcdata).set_value((iter->child("lUrl")).first_child().value().c_str());
            pugi::xml_node f_album_photo_mainimage = f_album_photo.append_child();     //f-album-photo-mainImage
            f_album_photo_mainimage.set_name("mainImage");
            f_album_photo_mainimage.append_child(node_pcdata).set_value((iter->child("mUrl")).first_child().value().c_str());
            pugi::xml_node f_album_photo_desc = f_album_photo.append_child();
            f_album_photo_desc.set_name("desc");                                          //f-album-photo-desc
            f_album_photo_desc.append_child(node_pcdata).set_value((iter->child("title")).first_child().value().c_str());
            pugi::xml_node f_album_photo_fulldesc = f_album_photo.append_child();            //f-album-photo-fulldesc
            f_album_photo_fulldesc.set_name("fulldesc");
            f_album_photo_fulldesc.append_child(node_pcdata).set_value((iter->child("title")).first_child().value().c_str());
     //       MCE_INFO("709 ----->  image:"<< (iter->child("hUrl")).first_child().value().c_str());
            pugi::xml_node f_album_photo_url = f_album_photo.append_child();               //f-album-photo-url
            f_album_photo_url.set_name("url");
            f_album_photo_url.append_child(node_pcdata).set_value((iter->child("url")).first_child().value().c_str());
    //        MCE_INFO("709 ----->  url:"<< (iter->child("url")).first_child().value().c_str());
          }
			  	k++;
		  	}
	  	}
      if (type_attribute==NULL && tpl_attribute!=NULL)
        iter = node.begin();
  		while(iter!=node.end()){
	  		nodeName = iter->name();
		  	if( nodeName.compare("photo") == 0 ){
			  	tmp = iter;
			  	++iter;
			  	node.remove_child(*tmp);
	  		}else{
		  		++iter;
	  		}
		  }
	  	//添加<f><biz>
  		node = xml.child("f").append_child();
  		node.set_name("biz");
  		os<<xml.child("f").child("album").child("photo").child("id").first_child().value()
  		  <<"_"<<_feed->smallType<<"_"<<_feed->actor;
  		node.append_child(node_pcdata).set_value(os.str().c_str());
//      MCE_INFO("return xml:" << xml.xml());
		return xml.xml();
  }
	case 103:
  {
    pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
    pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
    if (type_attribute==NULL && tpl_attribute!=NULL) {
			MCE_DEBUG("FeedToMessagConverter::_buildFNode -->   103 share photo");
//      pugi::xml_node f_comment = xml.child("f").child("comment");
//      f_comment.set_value(xml.child("f").child("share").child("summary").first_child().value().c_str());
  		xml.child("f").child("share").remove_child("comment");
      pugi::xml_node f_share_album = xml.child("f").child("share").append_child();  //f-share-album
      f_share_album.set_name("album");
      pugi::xml_node f_share_album_id = f_share_album.append_child();  //f-share-album-id
      f_share_album_id.set_name("id");
      f_share_album_id.append_child(node_pcdata).set_value(xml.child("f").child("share").child("meta").child("albumid").first_child().value().c_str());
      pugi::xml_node f_share_album_title = f_share_album.append_child();  //f-share-album-title
      f_share_album_title.set_name("title");
      f_share_album_title.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("meta").child("albumname").first_child().value().c_str());
      os << "http://photo.renren.com/photo/" << xml.child("f").child("share").child("resourceUserId").first_child().value().c_str()
        << "/album-" << xml.child("f").child("share").child("meta").child("albumid").first_child().value().c_str();
      pugi::xml_node f_share_album_url = f_share_album.append_child();    //f-share-album-url
      f_share_album_url.set_name("url");
      f_share_album_url.append_child(pugi::node_pcdata).set_value(os.str().c_str());
      os.str("");
      pugi::xml_node f_share_album_owner = f_share_album.append_child();   //f-share-album-owner
      f_share_album_owner.set_name("owner");
      pugi::xml_node f_share_album_owner_id = f_share_album_owner.append_child();   //f-share-album-owner-id
      f_share_album_owner_id.set_name("id");
      f_share_album_owner_id.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("resourceUserId").first_child().value().c_str());
      pugi::xml_node f_share_album_owner_name = f_share_album_owner.append_child();   //f-share-album-owner-name
      f_share_album_owner_name.set_name("name");
      f_share_album_owner_name.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("resourceUserName").first_child().value().c_str());
      pugi::xml_node f_share_album_photo = f_share_album.append_child();     //f-share-album-photo
      f_share_album_photo.set_name("photo");
      pugi::xml_node f_share_album_photo_id = f_share_album_photo.append_child();   //f-share-album-photo-id
      f_share_album_photo_id.set_name("id");           
      f_share_album_photo_id.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("resourceId").first_child().value().c_str());
      pugi::xml_node f_share_album_photo_url = f_share_album_photo.append_child();  //f-share-album-photo-url
      f_share_album_photo_url.set_name("url");
      f_share_album_photo_url.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("url").first_child().value().c_str());
      pugi::xml_node f_share_album_photo_image = f_share_album_photo.append_child(); //f-share-album-photo-image
      f_share_album_photo_image.set_name("image");
      f_share_album_photo_image.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("thumbFullUrl").first_child().value().c_str());
    }
    node = xml.child("f").append_child();   
		node.set_name("stype");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
  }
	case 101:
	case 105:
	case 106:
	case 108:
	case 109:
	case 112:
	case 113:
	case 115:
		//添加  <f><stype>
		node = xml.child("f").append_child();
		node.set_name("stype");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
//		os<<xml.child("f").child("share").child("id").first_child().value()<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
	case 104:
  {
    pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
    pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
    if (type_attribute==NULL && tpl_attribute!=NULL) {
			MCE_DEBUG("FeedToMessagConverter::_buildFNode -->   104 share album");

  		xml.child("f").child("share").remove_child("comment");
      pugi::xml_node f_share_album = xml.child("f").child("share").append_child();  //f-share-album
      f_share_album.set_name("album");
      pugi::xml_node f_share_album_id = f_share_album.append_child();  //f-share-album-id
      f_share_album_id.set_name("id");
      f_share_album_id.append_child(node_pcdata).set_value(xml.child("f").child("share").child("resourceId").first_child().value().c_str());
      pugi::xml_node f_share_album_title = f_share_album.append_child();  //f-share-album-title
      f_share_album_title.set_name("title");
      f_share_album_title.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("title").first_child().value().c_str());
      pugi::xml_node f_share_album_url = f_share_album.append_child();    //f-share-album-url
      f_share_album_url.set_name("url");
      f_share_album_url.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("url").first_child().value().c_str());
      pugi::xml_node f_share_album_count = f_share_album.append_child();  //f-share-album-count
      f_share_album_count.set_name("count");
      f_share_album_count.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("meta").child("count").first_child().value().c_str());
      pugi::xml_node f_share_album_owner = f_share_album.append_child();   //f-share-album-owner
      f_share_album_owner.set_name("owner");
      pugi::xml_node f_share_album_owner_id = f_share_album_owner.append_child();    //f-share-album-owner-id
      f_share_album_owner_id.set_name("id");
      f_share_album_owner_id.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("resourceUserId").first_child().value().c_str());
      pugi::xml_node f_share_album_owner_name = f_share_album_owner.append_child();    //f-share-album-owner-name
      f_share_album_owner_name.set_name("name");
      f_share_album_owner_name.append_child(pugi::node_pcdata).set_value(xml.child("f").child("share").child("resourceUserName").first_child().value().c_str());

      link = xml.child("f").child("share").child("thumbFullUrl").first_child().value();
      string originURL;
      pos=link.find("musicparkbbs.renren");
      if(pos!=string::npos){
        pos = link.find("origURL", pos);
        if(pos!=string::npos){
          originURL = link.substr(pos+strlen("origURL="));
        }
      }
      pugi::xml_node f_share_album_image = f_share_album.append_child();    //f-share-album-image
      f_share_album_image.set_name("image");
      if(!originURL.empty()){
        originURL = talk::http::UriDecode(originURL);
  		  f_share_album_image.append_child(node_pcdata).set_value(originURL.c_str());
      } else
  		  f_share_album_image.append_child(node_pcdata).set_value(link.c_str());
    } else {
  		link = xml.child("f").child("share").child("album").child("image").first_child().value();
      string originURL;
      pos=link.find("musicparkbbs.renren");
      if(pos!=string::npos){
        pos = link.find("origURL", pos);
        if(pos!=string::npos){
          originURL = link.substr(pos+strlen("origURL="));
        }
      }
      if(!originURL.empty()){
        originURL = talk::http::UriDecode(originURL);
  		  xml.child("f").child("share").child("album").remove_child("image");
        node = xml.child("f").child("share").child("album").append_child();
        node.set_name("image");
  		  node.append_child(node_pcdata).set_value(originURL.c_str());
      }
    } 
    node = xml.child("f").append_child();
		node.set_name("stype");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		
    return xml.xml();
  }
	case 102:
  {
    pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
    pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
    if (type_attribute==NULL && tpl_attribute!=NULL) {
			MCE_DEBUG("FeedToMessagConverter::_buildFNode -->   102 share blog");

  		xml.child("f").child("share").remove_child("comment");
      pugi::xml_node f_share_blog = xml.child("f").child("share").append_child();   //f-share-blog
      f_share_blog.set_name("blog");
      pugi::xml_node f_share_blog_owner = f_share_blog.append_child();          //f-share-blog-owner
      f_share_blog_owner.set_name("owner");
      pugi::xml_node f_share_blog_owner_id = f_share_blog_owner.append_child();    //f-share-blog-owner-id
      f_share_blog_owner_id.set_name("id");
		  f_share_blog_owner_id.append_child(node_pcdata).set_value(xml.child("f").child("share").child("resourceUserId").first_child().value().c_str());
      pugi::xml_node f_share_blog_owner_name = f_share_blog_owner.append_child();   //f-share-blog-owner-name
      f_share_blog_owner_name.set_name("name");
		  f_share_blog_owner_name.append_child(node_pcdata).set_value(xml.child("f").child("share").child("resourceUserName").first_child().value().c_str());
      pugi::xml_node f_share_blog_id = f_share_blog.append_child();      //f-share-blog-id
      f_share_blog_id.set_name("id");
      f_share_blog_id.append_child(node_pcdata).set_value(xml.child("f").child("share").child("resourceId").first_child().value().c_str());
      pugi::xml_node f_share_blog_title = f_share_blog.append_child();     //f-share-blog-title
      f_share_blog_title.set_name("title");
      f_share_blog_title.append_child(node_pcdata).set_value(xml.child("f").child("share").child("title").first_child().value().c_str());
      pugi::xml_node f_share_blog_digest = f_share_blog.append_child();    //f-share-blog-digest
      f_share_blog_digest.set_name("digest");
      f_share_blog_digest.append_child(node_pcdata).set_value(xml.child("f").child("share").child("summary").first_child().value().c_str());
      link = xml.child("f").child("share").child("url").first_child().value();    
		  if(link.find("GetEntry.do?") != string::npos) { 
        os << link << "&shareID=" << xml.child("f").child("share").child("sourceId").first_child().value().c_str() 
          << "&shareOwner=" << xml.child("f").child("share").child("sourceUserId").first_child().value().c_str();
        pugi::xml_node f_share_blog_url = f_share_blog.append_child();    //f-share-blog-url
        f_share_blog_url.set_name("url");
        f_share_blog_url.append_child(node_pcdata).set_value(os.str().c_str());
  		  os.str("");
      }
    } else {
  		link = node.child("share").child("blog").child("url").first_child().value();
  		if(link.find("GetEntry.do?") != string::npos){
        os << link << "&shareID=" << _feed->source << "&shareOwner=" << _feed->actor;
  		  node.child("share").child("blog").remove_child("url");
  		  node = xml.child("f").child("share").child("blog").append_child();
  		  node.set_name("url");
  		  node.append_child(node_pcdata).set_value(os.str().c_str());
  		  os.str("");
      }
    }
 
    node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
//		os<<xml.child("f").child("share").child("id").first_child().value()<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
    return xml.xml();
  }
  case 107:
  {
    pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
    pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
    if (type_attribute==NULL && tpl_attribute!=NULL) {
			MCE_DEBUG("FeedToMessagConverter::_buildFNode -->   107 share link");

  		xml.child("f").child("share").remove_child("comment");
      pugi::xml_node f_share_shared_link = xml.child("f").child("share").append_child();
      f_share_shared_link.set_name("shared_link");
      pugi::xml_node f_share_shared_link_title = f_share_shared_link.append_child();
      f_share_shared_link_title.set_name("title");
      f_share_shared_link_title.append_child(node_pcdata).set_value(xml.child("f").child("share").child("title").first_child().value().c_str());
      pugi::xml_node f_share_shared_link_image = f_share_shared_link.append_child();
      f_share_shared_link_image.set_name("image");
      f_share_shared_link_image.append_child(node_pcdata).set_value(xml.child("f").child("share").child("thumbFullUrl").first_child().value().c_str());
      pugi::xml_node f_share_shared_link_body = f_share_shared_link.append_child();
      f_share_shared_link_body.set_name("body");
      f_share_shared_link_body.append_child(node_pcdata).set_value(xml.child("f").child("share").child("body").first_child().value().c_str());

  		shareId = xml.child("f").child("share").child("id").first_child().value();
  		ownerId = xml.child("f").child("from").child("id").first_child().value();
  		link = string("http://share.renren.com/share/GetShare.do?") + "id=" + shareId + "&owner=" + ownerId;
      pugi::xml_node f_share_shared_link_url =  f_share_shared_link.append_child();
  		f_share_shared_link_url.set_name("url");
  		f_share_shared_link_url.append_child(node_pcdata).set_value(link.c_str());
      pugi::xml_node f_share_shared_link_eurl =  f_share_shared_link.append_child();
  		f_share_shared_link_eurl.set_name("eurl");
  		f_share_shared_link_eurl.append_child(node_pcdata).set_value(link.c_str());
    } else {
  		node = xml.child("f");
  		shareId = node.child("share").child("id").first_child().value();
  		ownerId = node.child("from").child("id").first_child().value();
  		link = string("http://share.renren.com/share/GetShare.do?") + "id=" + shareId + "&owner=" + ownerId;
  		
  		node.child("share").child("shared_link").remove_child("url");
  		tmpNode = node.child("share").child("shared_link").append_child();
  		tmpNode.set_name("url");
  		tmpNode.append_child(node_pcdata).set_value(link.c_str());
  		
  		node.child("share").child("shared_link").remove_child("eurl");
  		tmpNode = node.child("share").child("shared_link").append_child();
  		tmpNode.set_name("eurl");
  		tmpNode.append_child(node_pcdata).set_value(link.c_str());
    } 
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");

		return xml.xml();
  }
	case 111:
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");

		node = xml.child("f");
		shareId = node.child("share").child("id").first_child().value();
		ownerId = node.child("from").child("id").first_child().value();
		link = string("http://share.renren.com/share/GetShare.do?") + "id=" + shareId + "&owner=" + ownerId;
		
		node.child("share").child("music").remove_child("url");
		tmpNode = node.child("share").child("music").append_child();
		tmpNode.set_name("url");
		tmpNode.append_child(node_pcdata).set_value(link.c_str());
		
		node.child("share").child("music").remove_child("eurl");
		tmpNode = node.child("share").child("music").append_child();
		tmpNode.set_name("eurl");
		tmpNode.append_child(node_pcdata).set_value(link.c_str());
		return xml.xml();
	case 110:
  {
    pugi::xml_attribute type_attribute = xml.child("f").attribute("type");
    pugi::xml_attribute tpl_attribute = xml.child("f").attribute("tpl");
    if (type_attribute==NULL && tpl_attribute!=NULL) {
			MCE_DEBUG("FeedToMessagConverter::_buildFNode -->   110 share video");

  		xml.child("f").child("share").remove_child("comment");
      pugi::xml_node f_share_video = xml.child("f").child("share").append_child();    //f-share-video
      f_share_video.set_name("video");
      pugi::xml_node f_share_video_title = f_share_video.append_child();   //f-share-video-title
      f_share_video_title.set_name("title");
		  f_share_video_title.append_child(node_pcdata).set_value(xml.child("f").child("share").child("title").first_child().value().c_str());
      pugi::xml_node f_share_video_image = f_share_video.append_child();   //f-share-video-image
      f_share_video_image.set_name("image");
		  f_share_video_image.append_child(node_pcdata).set_value(xml.child("f").child("share").child("thumbFullUrl").first_child().value().c_str());
      pugi::xml_node f_share_video_url = f_share_video.append_child();   //f-share-video-url
      f_share_video_url.set_name("url");
		  f_share_video_url.append_child(node_pcdata).set_value(xml.child("f").child("share").child("meta").child("flashUrl").first_child().value().c_str());
      pugi::xml_node f_share_video_scale = f_share_video.append_child();    //f-share-video-scale
      f_share_video_scale.set_name("scale");
		  f_share_video_scale.append_child(node_pcdata).set_value(xml.child("f").child("share").child("meta").child("scale").first_child().value().c_str());
      pugi::xml_node f_share_video_fullurl = f_share_video.append_child();    //f-share-video-fullurl
      f_share_video_fullurl.set_name("fullUrl");
		  f_share_video_fullurl.append_child(node_pcdata).set_value(xml.child("f").child("share").child("url").first_child().value().c_str());
    } 
	  //添加<f><share><eurl></eurl></share></f>
	  //share.renren.com/share/GetShare.do?id=${share_id}&amp;owner=${from_id}
	  os<<"http://share.renren.com/share/GetShare.do?id="<<xml.child("f").child("share").child("id").first_child().value()
	    <<"&amp;owner="<<xml.child("f").child("from").child("id").first_child().value();
	  node = xml.child("f").child("share").append_child();
	  node.set_name("eurl");
	  node.append_child(node_pcdata).set_value(
	  os.str().c_str()
	  //xml.child("f").child("share").child("video").child("url").first_child().value().c_str()
	  );
    os.str("");
		//添加  <f><stype>
		node = xml.child("f").append_child();
		node.set_name("stype");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
//		os<<xml.child("f").child("share").child("id").first_child().value()<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
  }
	case 801: case 802:
		//添加  <f><stype>
		node = xml.child("f").append_child();
		node.set_name("stype");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><biz>  
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
//		os<<xml.child("f").child("share").child("sID").first_child().value()<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
	case 507: case 602: case 704:
		//添加<f><biz>  
		node = xml.child("f").append_child();
		node.set_name("biz");
		os<<_feed->source<<"_"<<_feed->smallType<<"_"<<_feed->actor;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><time>
		node = xml.child("f").append_child();
		node.set_name("time");
		os<<_feed->time;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		//添加<f><type>
		node = xml.child("f").append_child();
		node.set_name("type");
		os<<_feed->smallType;
		node.append_child(node_pcdata).set_value(os.str().c_str());
		os.str("");
		return xml.xml();
	default:
		return xml.xml();
	}
	// if (!fnode.load(os.str().c_str())) {
	// 	MCE_DEBUG("FeedWithTarget::_buildFNode --> load old xml error:  xml=\n"<<os.str());
	// 	return "<f>xml error from WTransitI</f>";
	// }
	return "";
}


MessagePtr FeedToMessagConverter::getToTalkMessage(const JidPtr& to, bool offline){
	MCE_DEBUG("call FeedToMessagConverter::buildTalkMessage actor="<<_feed->actor );
	// if(_toTalkMasg.first_child()){
	// 	MCE_DEBUG("FeedToMessagConverter::buildTalkMessage --> xml parsed");
	// 	string str = jidToString(to);
	// 	_toTalkMasg.child("message").append_attribute("to") = str.c_str();
	// 	MessagePtr msg = new Message();
	// 	msg->from = _jfrom;
	// 	msg->to = to;
	// 	msg->type = SYSTEM_MESSAGE;
	// 	msg->msg = _toTalkMasg.xml();
	// 	return msg;
	// }
	if( !_jfrom ){
		MCE_DEBUG("FeedToMessagConverter::getToTalkMessage --> FeedToMessagConverter::setData should be called first");
		return 0;
	}
	if(_toTalkMasgReady){
		string str = jidToString(to);
		_toTalkMasg.child("message").append_attribute("to") = str.c_str();
		MessagePtr msg = new Message();
		msg->from = _jfrom;
		msg->to = to;
		msg->type = SYSTEM_MESSAGE;
		msg->msg = _toTalkMasg.xml();
		return msg;
	}
	//Ice::Long newId = 6000000000000000000 + ((Ice::Long) _feed->time)
	//	* 100000 + ((_feed->feed & 0xffffffff) % 100000);
	Ice::Long newId = WTransitManagerI::baseFeedId + _feed->feed;
	MCE_DEBUG("FeedToMessagConverter::getToTalkMessage --> feedid="<<_feed->feed<<" actor="<<_feed->actor);
	
	string tmp = "0";
	if(offline){
		tmp = "1";
	}
	string fnodestr;
	fnodestr = _buildFNode();
	if("<f>xml error from WTransitI</f>" == fnodestr){
		return 0;
	}
	ostringstream os;
	os << "<message type='common' from='" << _jfrom->endpoint << "@feed.talk.xiaonei.com'>"
	   << "<xfeed xmlns='http://talk.renren.com/xfeed' offline='" << tmp << "'>"
	   << "<id>" << newId << "</id>"
	   //<< "<feedid>" << _feed->feed << "</feedid>"
	   << "<actor>" << _feed->actor << "</actor>"
	   << "<time>" << _feed->time << "</time>"
	   << "<type>" << _feed->bigType << "</type>"
	   <<"<typen/>"
	   << "<stype>" << _feed->smallType << "</stype>"
	   <<"<stypen/>"
	   << "<resource>" << _feed->source << "</resource>"
	   //<< _buildFNode()
		 << fnodestr
	   << _getTemplate(_feed->smallType)
	   << "</xfeed></message>";
	
	//MCE_INFO("FeedToMessagConverter::getToTalkMessage -> actor "<<_feed->actor<<" msg:" << os.str());
	if (!_toTalkMasg.load(os.str().c_str())) {
		MCE_WARN("FeedToMessagConverter::getToTalkMessage -> actor="<<_feed->actor<<" wrong: " << os.str());
		return 0;
	}
	_toTalkMasgReady = true;
	string str = jidToString(to);
	_toTalkMasg.child("message").append_attribute("to") = str.c_str();
	MessagePtr msg = new Message();
	msg->from = _jfrom;
	msg->to = to;
	msg->type = SYSTEM_MESSAGE;
	msg->msg = _toTalkMasg.xml();
	return msg;
}

