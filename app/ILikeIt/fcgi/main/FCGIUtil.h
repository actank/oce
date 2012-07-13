#include <ILikeIt.h>
#include "ILikeItLogicAdapter.h"
#include <vector>
namespace xce{
namespace fcgiutil{
using namespace xce::ilikeit;
using namespace std;
class FCGIUtil {
	public:
		static string UserDetailSeq2Str(const UserDetailSeq& likeList)
		{   
			ostringstream json;
			json<<"[";
			for( UserDetailSeq::const_iterator it = likeList.begin(); it != likeList.end(); ++it )
			{   
				if( it != likeList.begin() )
					json<<",";
				json<<"{id:"<<(*it)->uid<<",name:\"" <<(*it)->name<<"\",headUrl:\""<<(*it)->img<<"\"}";
			}   
			json<<"]";
			return json.str();
		}   

		static string LikeInfo2JSON(const LikeInfoPtr& lip)
		{   
			ostringstream json;
			json<<"likeCount:"<<lip->totalCount<<",ownerLike:"<<(lip->selfCount>0?"true":"false");
			json<<",likeList:"<<UserDetailSeq2Str(lip->detailList)<<"}";
			return json.str();
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
		}   


		static string ParseCookie(const string &http_cookie)
		{   
			int pos = http_cookie.find("societyguester")+15;
			return http_cookie.substr(pos,33);
		}   

}; 
};
};
