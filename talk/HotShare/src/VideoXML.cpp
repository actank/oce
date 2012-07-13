#include "VideoXML.h"
#include "Config.h"
#include "Date.h"
#include <sstream>
#include <expat.h> 
#include "LogWrapper.h"
#include <json/json.h>

using namespace std;
using namespace MyUtil;
using namespace xce::hotshare;

string VideoXML::OutputXML(const HotShareInfo &hsInfo, const size_t version)
{
  ostringstream ss;
  ss << "<hotshare v=\"" <<version << "\">\n"
    << "<dbid>" << hsInfo.id << "</dbid>\n"
    << "<localid>" << hsInfo.local_id << "</localid>\n"
    << "<type>" << hsInfo.type << "</type>\n"
    << "<hotvideo>\n"
    << "<username>" << XMLEncode( XMLEncode( CutUtf8( DeleteInvalidChar(hsInfo.user_name) ,TITLE_LEN) ) ) <<"</username>\n"
    << "<time>" << hsInfo.creation_date << "</time>\n"
    << "<title>" << XMLEncode( XMLEncode( CutUtf8(DeleteInvalidChar(hsInfo.title), TITLE_LEN ) )) << "</title>\n"
    << "<headurl>" << XMLEncode( XMLEncode(hsInfo.head_url) ) << "</headurl>\n"
    << "<thumburl>" << XMLEncode( XMLEncode(hsInfo.thumb_url) ) << "</thumburl>\n"
    << "<shareid>" << hsInfo.share_id <<"</shareid>\n"
    << "<ownerid>" << hsInfo.user_id <<"</ownerid>\n"
    << "<videourl>" << XMLEncode( XMLEncode(hsInfo.url) ) <<"</videourl>\n"
    << "</hotvideo>\n"
    << "</hotshare>";

  std::string ret = "";
  if ( ValidCheck( ss.str()) )
   ret = ss.str();
  
  return ret;
}



string VideoXML::XMLEncode(const string &source)
{
  ostringstream ss;
  for (size_t i=0; i<source.length(); ++i)
  {
    switch ( source[i])
    {
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


string VideoXML::OutputXML(const vector<string> &xmlVec , const vector<Ice::Long> &lowIDVec, const vector<Ice::Long> &highIDVec)
{
  ostringstream xml;
  xml << "<message pushtime=\"" << GetCurrentTime() << "\">\n";
  if (lowIDVec.size()==highIDVec.size() && lowIDVec.size()>0)
  {
    xml <<  "<range>\n";

    for (size_t i=0; i<lowIDVec.size(); ++i)
      xml << "<r begin=\"" << lowIDVec[i] << "\"  end=\"" << highIDVec[i]  <<"\" />\n";
    
    xml << "</range>\n";
                                              
    if (xmlVec.size() > 0)
    {
      xml << "<data>\n";

      for (size_t j=0; j<xmlVec.size(); j++)
        xml << xmlVec[j];

      xml << "</data>\n";
    }
  }         

  xml <<  "</message>";
                      

  return xml.str();

}

const string VideoXML::GetCurrentTime()
{
  struct tm *ptm;
  long ts;
  int         y,m,d,h,n,s;
        
  ts   =   time(NULL);
  ptm   =   localtime(&ts);
        
  y   =   ptm-> tm_year+1900;     //年
  m   =   ptm-> tm_mon+1;             //月
  d   =   ptm-> tm_mday;               //日
  h   =   ptm-> tm_hour;               //时
  n   =   ptm-> tm_min;                 //分
  s   =   ptm-> tm_sec;                 //秒

  ostringstream ss;
  ss<<y<<"-"<<m<<"-"<<d<<" "<<h<<":"<<n<<":"<<s;

  return ss.str();
}

int VideoXML::UTF8Length(const char *_s, int _len)
{
  
  const unsigned char *s = (const unsigned char *)_s;  
  const unsigned char *lim = s + _len;  
  int len = 0;  
  if(_len > 4)
    while(s < lim - 4) {
      unsigned short code = (unsigned char)*s++;
      if(code < 0x80)
        len++;
      else
        if(code < 0xC2)
          len++;
        else
          if(code < 0xE0) {
            unsigned short c = ((code - 0xC0) << 6) + s[0] - 0x80;
            if(s[0] >= 0x80 && s[0] < 0xc0 && c >= 0x80 && c < 0x800)
              len++;
            else
              len += 2;
            s += 1;
          }
          else
            if(code < 0xF0) {
              unsigned short c = ((code - 0xE0) << 12) + ((s[0] - 0x80) << 6) + s[1] - 0x80;
              if(s[0] >= 0x80 && s[0] < 0xc0 && s[1] >= 0x80 && s[1] < 0xc0 && c >= 0x800
                  && !(c >= 0xEE00 && c <= 0xEEFF))
                len++;
              else
                len += 3;
              s += 2;
            }
            else
              len++;
    }
  while(s < lim) {
    unsigned short code = (unsigned char)*s++;
    if(code < 0x80)
      len++;
    else
      if(code < 0xC2)
        len++;
      else
        if(code < 0xE0) {
          if(s > lim - 1) {
            len++;
            break;
          }

          unsigned short c = ((code - 0xC0) << 6) + s[0] - 0x80;
          if(s[0] >= 0x80 && s[0] < 0xc0 && c >= 0x80 && c < 0x800)
            len++;
          else
            len += 2;
          s += 1;
        }
        else
          if(code < 0xF0) {
            if(s > lim - 2) {
              len += 1 + (int)(lim - s);
              break;
            }

            unsigned short c = ((code - 0xE0) << 12) + ((s[0] - 0x80) << 6) + s[1] - 0x80;
            if(s[0] >= 0x80 && s[0] < 0xc0 && s[1] >= 0x80 && s[1] < 0xc0 && c >= 0x800
                && !(c >= 0xEE00 && c <= 0xEEFF))
              len++;
            else
              len += 3;
            s += 2;
          }
          else
            len++;
  }

  return len;

}


bool VideoXML::ValidCheck(const string & xml)
{ 
  XML_Parser parser = XML_ParserCreate(NULL);           
  if (!parser) 
  {              
    MCE_WARN("VideoXML::ValidCheck   Message---->    Fail to create Parser..");
    return false;                         
  }
                   
  if (!XML_Parse(parser, xml.c_str(), xml.size(), 1)) 
  {
    MCE_INFO("VideoXML::ValidCheck  Message---->    xml error:"<<xml << " line:" << XML_GetCurrentLineNumber(parser)
        << "   errorstring:" <<XML_ErrorString(XML_GetErrorCode(parser)) );                          
    XML_ParserFree(parser);                               
    return false;
  }

  XML_ParserFree(parser);
  return true;
}

string VideoXML::DeleteInvalidChar(const string &src)
{
  ostringstream ss; 
  for (size_t i=0; i<src.length(); ++i)
  {  
    if (src[i]!='\t' && src[i]!=' ' && src[i]!='\n')
      ss << src[i];                  
  }
   
  return ss.str();
}

string VideoXML::CutUtf8(const string &src, size_t limitlen)
{
  float len = 0;
  size_t i = 0;

  while ( i < src.length() )   
  {                      
    unsigned char current_char = src[i];    
    if (current_char>=0x00 && current_char<=0x7F)   
      len+=0.5, ++i;
    else if (current_char>=0xC0 && current_char<=0xDF)
    {
      current_char = src[i];
      if (current_char>=0x80 && current_char<=0xBF)   
        ++len;
      else
        break;
      i += 2;
    }
    else if (current_char>=0xE0 && current_char<=0xEF)
    {
      size_t j = 1 ;
      for ( ; j<3; ++j)
      {
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
    else if (current_char>=0xF0 && current_char<=0xF7)
    {
      size_t j = 1 ;
      for ( ; j<4; ++j)
      {
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
  return src.substr(0, i);
}
