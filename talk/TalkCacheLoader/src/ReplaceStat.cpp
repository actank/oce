#include "ReplaceStat.h"
#include "DictTree.h"
#include <sstream>
#include "QueryRunner.h"
#include "ServiceI.h"
#include "XmppTools.h"
#include <util/cpp/String.h>

using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::cache::loader;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;


void ReplaceStat::loadFaceTree(){
	MCE_INFO("ReplaceStat::loadFace --> load face tree");
	vector<string> ubb2src;
        DictTree dictTree;

	Statement sql;
	sql<<"select ubb,src,alt from doing_ubb_cut";
	mysqlpp::StoreQueryResult res = QueryRunner("biz_friend_doing", CDbRServer).store(sql);
	if (!res) {
		return;
	}
	mysqlpp::Row row;
	for (size_t i = 0; i<res.num_rows(); ++i) {
		row = res.at(i);
		ubb2src.push_back(string("<img alt=\"")+row["alt"].data()+"\" src=\""+HEAD_URL+row["src"].data()+"\"/>");
		dictTree.addPhrase((unsigned char*)row["ubb"].c_str(),1,i);
	}
	
	IceUtil::RWRecMutex::WLock lock(_mutex);
	ubb2src.swap(_ubb2src);
	_dictTree = dictTree;
}

void ReplaceStat::handle(){
	loadFaceTree();
}

void ReplaceStat::_replaceLink(
	long id, int type,
	string& content, const string& link,
	unsigned int maxLength)
{
	IceUtil::RWRecMutex::RLock lock(_mutex);
	size_t pos;
	ostringstream str;
	string showLink;
	switch( type ){
	case TYPE_NORMAL:
		break;
	case TYPE_LINK:
		if( link.length()>maxLength ){
			showLink.assign(link,0,maxLength);
			showLink.append("...");
		}
		else{
			showLink.assign(link);
		}
		pos = content.find("[link]");
		if(pos == string::npos){
			break;
		}
		str<<"<a target='_blank' href='"<<link<<"'>"<<showLink<<"</a>";
		content.replace(pos,sizeof("[link]")-1,str.str());
		MCE_INFO("replace result = "<<content);
		break;
	case TYPE_AUDIO:
		pos = content.find("[audio]");
		if(pos == string::npos){
			break;
		}
		str<<"<a href='"<<link<<"' onclick='playStatusAudio("<<id<<",\""<<link<<"\",this);return false;' class='statusaudiobtn'><img src='"<<HEAD_URL<<"/imgpro/icons/audiobtn.gif'/></a>";
		content.replace(pos,sizeof("[audio]")-1,str.str());
		MCE_INFO("replace result = "<<content);
		break;
	case TYPE_VIDEO:
		pos = content.find("[video]");
		if(pos == string::npos){
			break;
		}
		str<<"<a href='"
		   <<link
		   <<"' onclick='playStatusAudio("
		   <<id
		   <<",\""
		   <<link
		   <<"\",this);return false;' class='statusaudiobtn'><img src='"
		   <<HEAD_URL
		   <<"/imgpro/icons/audiobtn.gif'/></a>";
		content.replace(pos,sizeof("[video]")-1,str.str());
		MCE_INFO("replace result = "<<content);
		break;
	}
}






void ReplaceStat::_replaceFace(string& content)
{
	IceUtil::RWRecMutex::RLock lock(_mutex);
	string tmp;
	DictNodePtr node;
	int pos;
	int textBeg = 0;
	int textEnd = 0;
	for( unsigned int index=0; index<content.length();){
		pos = index;
 		int id;
		string replace;
		textEnd = index;
		node = _dictTree.getHeadNode();
		while( node!=0 && pos<=content.length()){
			if( node->getStat()==1 ){
				id = node->getTid();
				replace = _ubb2src[id];
				index = pos;
				break;
			} else {
				node = node->getChild(content[pos]);
				pos++;
			}
		}
		if(!replace.empty()){
			string txt = content.substr(textBeg, textEnd-textBeg);
			MyUtil::StrUtil::string_replace(txt, "&", "&amp;");
			string encodedTxt = xmlEncode(txt);
			tmp += encodedTxt;
			tmp += replace;
			textBeg = index;
		}else{
			index++;
		}
	}
	string txt = content.substr(textBeg);
	if(!txt.empty()){
		MyUtil::StrUtil::string_replace(txt, "&", "&amp;");
		string encodedTxt = xmlEncode(txt);
		tmp += encodedTxt;
	}
	content = tmp;
}



void ReplaceStat::replace(
	long id, int type, string& content, const string& link, unsigned int maxLength)
{
	_replaceLink(id,type,content,link,maxLength);
	_replaceFace(content);
}
