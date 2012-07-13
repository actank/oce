#ifndef _SOCIALGRAPHRCD_ICE_
#define _SOCIALGRAPHRCD_ICE_

#include <Util.ice>

module xce {
module socialgraph {

struct ContentRcd {
  string md5;
  long sid; //share id
  int uid;  //user id
  string uname; // user name
  string thumburl;
  string shareurl;
  string title;
  int lisc; //limit or score.for request it stands for limit, for response it stands for score!
  string desc; 
};

["java:type:java.util.ArrayList"] 
sequence<ContentRcd> ContentRcdSeq;

interface ContentRcdWorker {
  ContentRcdSeq GetVideoRcd(int host, ContentRcd rcd);
//  ContentRcdSeq GetBlogRcd();
};

};
};

#endif

