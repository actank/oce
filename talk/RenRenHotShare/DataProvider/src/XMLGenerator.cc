#include "XMLGenerator.h"
#include "Date.h"
#include <sstream>
#include <expat.h> 
#include "LogWrapper.h"

using namespace std;
using namespace MyUtil;
using namespace xce::dataprovider;

string XMLGenerator::OutputXMLVideo(const HotShareInfo &hotshare_info, const size_t version) {
  ostringstream ss;
  ss << "<hotshare v=\"" <<version << "\">\n"
    << "<localid>" << hotshare_info.id << "</localid>\n"
    << "<type>1</type>\n"
    << "<hotvideo>\n"
    << "<username>" << XMLEncode( XMLEncode( CutUtf8( DeleteInvalidChar(hotshare_info.user_name) ,string_len) ) ) <<"</username>\n"
    << "<creationdate>" << hotshare_info.creation_date << "</creationdate>\n"
    << "<title>" << XMLEncode( XMLEncode( CutUtf8(DeleteInvalidChar(hotshare_info.title), string_len ) )) << "</title>\n"
    << "<headurl>" << XMLEncode( XMLEncode(hotshare_info.head_url) ) << "</headurl>\n"
    << "<thumburl>" << XMLEncode( XMLEncode(hotshare_info.thumb_url) ) << "</thumburl>\n"
    << "<shareid>" << hotshare_info.share_id <<"</shareid>\n"
    << "<ownerid>" << hotshare_info.owner_id <<"</ownerid>\n"
    << "<userid>" << hotshare_info.user_id <<"</userid>\n"
    << "<videourl>" << XMLEncode( XMLEncode(hotshare_info.url) ) <<"</videourl>\n"
    << "</hotvideo>\n"
    << "</hotshare>";

  std::string ret = "";
  if ( ValidCheck( ss.str()) )
   ret = ss.str();
  return ret;
}

string XMLGenerator::OutputXMLAlbum(const HotShareInfo &hotshare_info, const Ice::Long album_id, const size_t version) {
  ostringstream ss;
  ss << "<hotshare v=\"" <<version << "\">\n"
    << "<localid>" << hotshare_info.id << "</localid>\n"
    << "<type>2</type>\n"
    << "<hotalbum>\n"
    << "<username>" << XMLEncode( XMLEncode( CutUtf8( DeleteInvalidChar(hotshare_info.user_name) ,string_len) ) ) <<"</username>\n"
    << "<creationdate>" << hotshare_info.creation_date << "</creationdate>\n"
    << "<title>" << XMLEncode( XMLEncode( CutUtf8(DeleteInvalidChar(hotshare_info.title), string_len ) )) << "</title>\n"
    << "<headurl>" << XMLEncode( XMLEncode(hotshare_info.head_url) ) << "</headurl>\n"
    << "<albumcoversmall>" << XMLEncode( XMLEncode(hotshare_info.thumb_url) ) << "</albumcoversmall>\n"
    << "<albumcoverlarge>" << XMLEncode( XMLEncode(hotshare_info.thumb_url1) ) << "</albumcoverlarge>\n"
    << "<shareid>" << hotshare_info.share_id <<"</shareid>\n"
    << "<ownerid>" << hotshare_info.owner_id <<"</ownerid>\n"
    << "<userid>" << hotshare_info.user_id <<"</userid>\n"
    << "<albumurl>" << XMLEncode( XMLEncode(hotshare_info.url) ) <<"</albumurl>\n"
    << "<albumid>" << album_id <<"</albumid>\n"
    << "<comment>" << XMLEncode( XMLEncode( CutUtf8(DeleteInvalidChar(hotshare_info.summary), summary_len ) )) <<"</comment>\n"
    << "</hotalbum>\n"
    << "</hotshare>";

  std::string ret = "";
  if ( ValidCheck( ss.str()) )
   ret = ss.str();
  return ret;
}

string XMLGenerator::OutputXMLPhoto(const PhotoInfo &photo_info, const size_t version) {
  ostringstream ss;
  ss << "<photo>\n"
    << "<pid>" << photo_info.photoId << "</pid>\n"
    << "<comment>" << XMLEncode( XMLEncode(DeleteInvalidChar(photo_info.atTitle)) ) << "</comment>\n"
    << "<tiny>" << XMLEncode( XMLEncode(photo_info.headUrl) ) << "</tiny>\n" 
    << "<large>" << XMLEncode( XMLEncode(photo_info.largeUrl) ) << "</large>\n"
    << "<link>" << XMLEncode( XMLEncode(photo_info.linkUrl) ) << "</link>\n"
    << "</photo>";
  return ss.str();
}

string XMLGenerator::XMLEncode(const string &source) {
  ostringstream ss;
  for (size_t i=0; i<source.length(); ++i) {
    switch ( source[i]) {
      case '&':
         ss << "&amp;";
        break;
      case '<':
        ss << "&lt;";
        break;
      case '>':
        ss << "&gt;";
        break;
      case '\'':
        ss << "&apos;";
        break;
      case '"':
        ss << "&quot;";
        break;
      default:
        ss <<  source[i];
    }
  }
  return ss.str();
}


string XMLGenerator::OutputXMLHotShare(const vector<string> &hotshare_xml_vector , const vector<Ice::Long> &low_id_vector, const vector<Ice::Long> &high_id_vector) {
  ostringstream xml;
  xml << "<message pushtime=\"" << GetCurrentTime() << "\">\n";
  if (low_id_vector.size()==high_id_vector.size() && low_id_vector.size()>0) {
    xml <<  "<range>\n";

    for (size_t i=0; i<low_id_vector.size(); ++i)
      xml << "<r begin=\"" << low_id_vector[i] << "\"  end=\"" << high_id_vector[i]  <<"\" />\n";
    
    xml << "</range>\n";
                                              
    xml << "<data>\n";
    if (hotshare_xml_vector.size() > 0) {
      for (size_t j=0; j<hotshare_xml_vector.size(); j++)
        xml << hotshare_xml_vector[j];
    }
    xml << "</data>\n";
  }         
  xml <<  "</message>";

  return xml.str();
}


string XMLGenerator::OutputXMLPhotoList(const vector<string> photo_list_vector) {
  ostringstream ss;
  ss << "<photolist>\n";
  for (vector<string>::const_iterator photo_list_vector_iter=photo_list_vector.begin(); 
      photo_list_vector_iter!=photo_list_vector.end(); photo_list_vector_iter++) {
    ss << (*photo_list_vector_iter) <<"\n";
  }
  ss  <<"</photolist>";

  std::string ret = "";
  if ( ValidCheck( ss.str()) )
   ret = ss.str();
  return ret;
}

const string XMLGenerator::GetCurrentTime() {
  struct tm *ptm;
  long ts;
  int y,m,d,h,n,s;
        
  ts = time(NULL);
  ptm = localtime(&ts);
        
  y = ptm-> tm_year+1900;     //年
  m = ptm-> tm_mon+1;             //月
  d = ptm-> tm_mday;               //日
  h = ptm-> tm_hour;               //时
  n = ptm-> tm_min;                 //分
  s = ptm-> tm_sec;                 //秒

  ostringstream ss;
  ss<<y<<"-"<<m<<"-"<<d<<" "<<h<<":"<<n<<":"<<s;

  return ss.str();
}

bool XMLGenerator::ValidCheck(const string & xml) { 
  XML_Parser parser = XML_ParserCreate(NULL);           
  if (!parser) {              
    MCE_WARN("XMLGenerator::ValidCheck   Message---->    Fail to create Parser..");
    return false;                         
  }
  if (!XML_Parse(parser, xml.c_str(), xml.size(), 1)) {
    MCE_INFO("XMLGenerator::ValidCheck  Message---->    xml error:"<<xml << " line:" << XML_GetCurrentLineNumber(parser)
        << "   errorstring:" <<XML_ErrorString(XML_GetErrorCode(parser)) );                          
    XML_ParserFree(parser);                               
    return false;
  }
  XML_ParserFree(parser);
  return true;
}

string XMLGenerator::DeleteInvalidChar(const string &src) {
  ostringstream ss; 
  for (size_t i=0; i<src.length(); ++i) {  
    if (src[i]!='\t' && src[i]!=' '  && src[i]!='\r' && src[i]!='\n' )
      ss << src[i];                  
  }
  return ss.str();
}

string XMLGenerator::CutUtf8(const string &src, size_t limitlen) {
  float len = 0;
  size_t i = 0;

  while ( i < src.length() ) {                      
    unsigned char current_char = src[i];    
    if (current_char>=0x00 && current_char<=0x7F)   
      len+=0.5, ++i;
    else if (current_char>=0xC0 && current_char<=0xDF) {
      current_char = src[i];
      if (current_char>=0x80 && current_char<=0xBF)   
        ++len;
      else
        break;
      i += 2;
    }
    else if (current_char>=0xE0 && current_char<=0xEF) {
      size_t j = 1 ;
      for ( ; j<3; ++j) {
        current_char = src[i+j];
        if (!(current_char>=0x80 && current_char<=0xBF))    
          break;
      }
      if (3 == j)
        ++len;
      else
        break;
      i += 3;
    }
    else if (current_char>=0xF0 && current_char<=0xF7) {
      size_t j = 1 ;
      for ( ; j<4; ++j) {
        current_char = src[i+j];
        if (!(current_char>=0x80 && current_char<=0xBF))            
          break;
      }
      if (4 == j)
        ++len;
      else
        break;
      i += 4;
    }
    else 
      break;

    if (limitlen <= (size_t)len)
      break;
  }
  string ret = src.substr(0,i);
  if (i < src.length())
    ret += "...";
  return ret;
}
