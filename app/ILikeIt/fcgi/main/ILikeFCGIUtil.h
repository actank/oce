#include <ILikeIt.h>
#include "ILikeItLogicAdapter.h"
#include <vector>
#include "xiaonei-commons-gid/util/c/gid.h"
namespace xce{
namespace ilikeit{
using namespace xce::ilikeit;
using namespace std;

const int TASK_LEVEL_POST = 1;

class ILikeFCGIUtil {
	public:
	static string UserDetailSeqStr(int userId, const UserDetail2Seq& likeList)
	{   
		ostringstream json;
		json<<"[";
		if(likeList.begin()==likeList.end()){
		json<<"]";
		return json.str();
		
		}
		UserDetail2Seq::const_iterator itEnd=(likeList.end()-1);
		int count = 8;
		if((*itEnd)->uid==userId){
		json<<"{id:"<<(*itEnd)->uid<<",name:\"" <<(*itEnd)->name<<"\",headUrl:\""<<(*itEnd)->img<<"\"}";
		--count;
		}else{
		++itEnd;
		}
		for( UserDetail2Seq::const_iterator it = likeList.begin(); it < itEnd && count>0; ++it )
		{   
		if((*it)->uid<0)
			continue;
		if( count!=8 )
			json<<",";
		json<<"{id:"<<(*it)->uid<<",name:\"" <<(*it)->name<<"\",headUrl:\""<<(*it)->img<<"\"}";
		--count;
		}   
		json<<"]";
		return json.str();
	}   

	static string LikeInfo2JSON(int userId, const LikeInfo2Ptr& lip)
	{   
		ostringstream json;
		json<<"likeCount:"<<lip->totalCount<<",ownerLike:"<<(lip->selfCount>0?"true":"false");
		json<<",friendCount:"<<lip->friendCount;
		json<<",likeList:"<<UserDetailSeqStr(userId, lip->detailList)<<"}";
		return json.str();
	}   

	static string GetParam(FCGX_Request& request){
		string  result;
		if(strcmp(FCGX_GetParam("REQUEST_METHOD", request.envp), "POST") == 0){
			string content_length = FCGX_GetParam("CONTENT_LENGTH", request.envp);
			if(content_length == ""){
				result = "";
			} else {
				long len = 0;
				len = boost::lexical_cast<long>(content_length);
				if (len <= 0 && len > 2048) {
					result = "";
				} else {
					char *buffer = new char[len + 1];
					FCGX_GetStr(buffer, len, request.in);
					buffer[len] = '\0';
					result = buffer;
					delete []buffer;
				}
			}
		}else{
			result = FCGX_GetParam("QUERY_STRING", request.envp);
		}
		return result;
	}

	static void ParseParam(const string& query_string, map<string, string>& mapParam) {
		vector<string> vecParam;
		size_t pos0 = 0;
		size_t pos1 = query_string.find('&', pos0);
		while ( pos1 != string::npos) {
		string strSeg = query_string.substr(pos0, pos1-pos0);
		vecParam.push_back(strSeg);
		pos0 = pos1+1;
		pos1 = query_string.find('&', pos0);
		}   
		if ( pos0 < query_string.length() ) { 
		string strSeg = query_string.substr(pos0, query_string.length()-pos0);
		vecParam.push_back(strSeg);
		}   
		for ( vector<string>::iterator it = vecParam.begin(); it != vecParam.end(); it++ ) { 
		size_t pos = (*it).find('=');
		if ( pos != string::npos ) { 
			string key = (*it).substr(0, pos);
			string value = (*it).substr(pos+1, (*it).length()-pos-1);
			if ( key != "" ) { 
			mapParam.insert(pair<string, string>(key, value));
			}   
		}   
		}   
		if(mapParam.find("gid")!=mapParam.end()){
			string val = mapParam["gid"];
			if(val.find("video")!=string::npos){
				long id = getGidNum(val);
				long oid = gid::G::ExtractX(id) - (1L<<43);
				ostringstream oss;
				oss<<"video_"<<oid;
				mapParam["gid"] = oss.str(); 
				MCE_INFO("extractX gid:"<< val<<" id:"<<gid::G::ExtractX(id)<<" oid:"<<mapParam["gid"]);
				mapParam["Gid"] = val;
			}else{
				mapParam["Gid"] = val;
			}
		}
	}   


	static string ParseCookie(const string &http_cookie)
	{   
		int pos = http_cookie.find("societyguester")+15;
		if(pos>=0&&pos+33<=http_cookie.size())
		    return http_cookie.substr(pos,33);
		else
		    return "";
	}   
	
	 	static	char dec2hexChar(short int n) {
		if ( 0 <= n && n <= 9 ) {
		return char( short('0') + n );
		} else if ( 10 <= n && n <= 15 ) {
		return char( short('A') + n - 10 );
		} else {
		return char(0);
		}
	}

	static	short hexChar2dec(char c) {
		if ( '0'<=c && c<='9' ) {
		return short(c-'0');
		} else if ( 'a'<=c && c<='f' ) {
		return ( short(c-'a') + 10 );
		} else if ( 'A'<=c && c<='F' ) {
		return ( short(c-'A') + 10 );
		} else {
		return -1;
		}
	}

	static string escapeURL(const string &URL)
	{
		string result = "";
		for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if (
			( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c=='/' || c=='.'
		   ) {
			result += c;
		} else {
			int j = (short int)c;
			if ( j < 0 ) {
			j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1*16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0);
		}
		}
		return result;
	}

	static string deescapeURL(const std::string &URL) {
		string result = "";
		for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if ( c != '%' ) {
			result += c;
		} else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
		}
		return result;
	}

	static long getGidNum(const string& gid){
		int pos=gid.find('_');
		long num=-1;
		try{
		num = boost::lexical_cast<long>(gid.substr(pos+1));
		}catch(...){
		}
		return num;
	}
	static string convert(const string& text){
		if(text.find('%'))
		{
		char buf[text.size()*2];
		size_t j=0;
		for(size_t i=0;i<text.size();++i){
			if(text[i]=='%')
			buf[j++]='%';
			buf[j++]=text[i];
		}
		buf[j]='\0';
		return string(buf);
		}else{
		return text;
		}
	} 
	static int typeSize;
}; 
};
};
