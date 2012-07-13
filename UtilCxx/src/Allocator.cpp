#include "Allocator.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::platform::storage;


GroupPtr Domain::getGroup(const string& groupName) {
		
	map<string, GroupPtr>::iterator it = allGroups.find(groupName);
	if ( it != allGroups.end() ) {
		return (*it).second;
	} else {
		// MCE_WARN("[Domain::getGroup] Can't find group Name : " << groupName);
		MCE_DEBUG("[Domain::getGroup] Can't find group Name : " << groupName);
		return NULL;
	}
}

void Domain::loadConfig() {

	map<string, GroupPtr> tempAllGroups;
	Statement sql;
	ostringstream tmp;
	tmp << "select * from " << TABLE_STORAGE_NODE << " where " << COLUMN_STATUS 
		<< ">" << NODE_STATUS_DISABLE;
	sql << tmp.str();
	try {
		QueryRunner(_dbDesc, CDbRServer).query(sql, GroupResultHandler(tempAllGroups));
	} catch (Ice::Exception& e ) {
		MCE_WARN("[Domain::loadConfig] get groups from db Ice::Exception : " << e.what());
	} catch (...) {
		MCE_WARN("[Domain::loadConfig] get groups from db exception");
	}
					
	allGroups = tempAllGroups;
	MCE_INFO("[Domain::loadConfig] all available nodes : " << tempAllGroups.size());
}

// -------------------- Allocator -----------------------------------
string Allocator::locate(const string& pattern) {

	// we think the url of this image is like
	// "GROUP/photo/2007xxxx/xxxx.jpg,
	// so if it does not start with GROUP, just return null.
	GroupPtr group = NULL;
	string fileName = "";
	size_t pos = pattern.find(FILE_SEPARATOR);
	if (pos == string::npos) {
		group = domain->getGroup(pattern);
		fileName = pattern;
	} else {
		group = domain->getGroup(pattern.substr(0, pos));
		fileName = pattern.substr(pos+1, pattern.size()-pos-1);
	}
	if ( !group ) {
		// MCE_WARN("[Allocator::locate] no group for " << pattern);
		MCE_DEBUG("[Allocator::locate] no group for " << pattern);
		return "";
	}
	if (fileName == "") {
		MCE_WARN("storage fileName null [" + pattern + "]");
		return "";
	}

	// check whether we do something wrong that upload the picture to
	// head storage, if so, recorrect it
	if ((group->getName().find("hd") == 0) 
		|| (group->getName().find("kxhd") == 0)) {
		// :(, we have to add to subdirectory to recorrect it
		return "http://" + group->getReadableNode()->getDomain()
				+ "/photos/" + group->getName() + "/" + fileName;
	} else if ( (group->getName().find("fmn") == 0)
		|| (group->getName().find("kxfmn") == 0 )) {
		return "http://" + group->getReadableNode()->getDomain()
		+ "/" + pattern ;
	}else {
		return "http://" + group->getReadableNode()->getDomain()
				+ "/" + fileName;
	}	
}

string Allocator::locate(const string& pattern, int type) {

	// we think the url of this image is like
	// "GROUP/photo/2007xxxx/xxxx.jpg,
	// so if it does not start with GROUP, just return null.
	GroupPtr group = NULL;
	string fileName = "";
	string groupName = "";
	size_t pos = pattern.find(FILE_SEPARATOR);
	if (pos == string::npos) {
		groupName = pattern;
		group = domain->getGroup(groupName);
		fileName = pattern;
	} else {
		groupName = pattern.substr(0, pos);
		group = domain->getGroup(groupName);
		fileName = pattern.substr(pos+1, pattern.size()-pos-1);
	}
	
	if (type == NODE_TYPE_NORMAL) {

		if (fileName == "") {
			MCE_WARN("storage fileName null [" + pattern + "]");
			return "";
		}
		if ( group ) {
			// check whether we do something wrong that upload the picture to
			// head storage, if so, recorrect it
			if (groupName.find("hd") == 0) {
				// :(, we have to add to subdirectory to recorrect it
				return "http://" + group->getReadableNode()->getDomain()
					+ "/photos/" + groupName + "/" + fileName;
			}else if(groupName.find("fmn") == 0){
				return "http://" + group->getReadableNode()->getDomain()
					+ "/photos/" + pattern ;
			} 
			else {
				return "http://" + group->getReadableNode()->getDomain()
					+ "/" + fileName;
			}
		} else {
			// MCE_WARN("[Allocator::locate] no group for " << pattern);
			MCE_DEBUG("[Allocator::locate] no group for " << pattern);
			if ( groupName.find("hdn") == 0 ) {
				return "http://" + groupName + ".xnimg.cn" + "/photos/" + groupName + "/" + fileName;
			} else if ( groupName.find("fmn") == 0 ) {
				return "http://" + groupName + ".xnimg.cn" + "/photos/" + groupName + "/" + fileName;
			} else if ( groupName.find("hd") == 0 ) {
				return "http://" + groupName + ".xiaonei.com" + "/photos/" + groupName + "/" + fileName;
			} else {
				// return URL_HEAD + "/photos/0/0/men_head.gif";
				// return "http://" + groupName + ".xiaonei.com" + "/" + fileName;
				return "";
			}
		}
	} else if (type == NODE_TYPE_HEAD) {
		if (group) {
			return "http://" + group->getReadableNode()->getDomain()
				+ "/photos/" + pattern;
		} else {
			string url = "";
			if (pattern.find("head.jpg") != string::npos)
				url = URL_HEAD + "/photos/0/0/men_head.gif";
			else if (pattern.find("main.jpg") != string::npos)
				url = URL_HEAD + "/photos/0/0/men_main.gif";
			else if (pattern.find("tiny.jpg") != string::npos)
				url = URL_HEAD + "/photos/0/0/men_tiny.gif";
			else
				url = URL_HEAD + "/photos/" + pattern;	
			return url;		
		}
	}
	return "";
}

GroupPtr Allocator::locateGroup(const string& pattern) {
	size_t pos = pattern.find(FILE_SEPARATOR);
	if (pos == string::npos) {
		return domain->getGroup(pattern);
	} else {
		return domain->getGroup(pattern.substr(0, pos));
	}
}

void Allocator::run() {

	while ( true ) {
		sleep(LOAD_INTERVAL);
		loadConfig();
	}
}


//For feature Photo Memory, we add this interface. It is actually copy from function locate() and add tiny changes to
//make it output correct photo url.

string Allocator::getPhotoUrl(const string& pattern) {
  // we think the url of this image is like
  // "GROUP/photo/2007xxxx/xxxx.jpg,
  // so if it does not start with GROUP, just return null.
  GroupPtr group = NULL;
  string fileName = "";
  size_t pos = pattern.find(FILE_SEPARATOR);
  if(!domain)
  {
    MCE_INFO("[Allocator::locate] domain null for " << pattern);
     return "";
  }

  if (pos == string::npos) {
    group = domain->getGroup(pattern);
    fileName = pattern;
  } else {
    group = domain->getGroup(pattern.substr(0, pos));


    fileName = pattern.substr(pos+1, pattern.size()-pos-1);
  }

  if ( !group ) {
    // MCE_WARN("[Allocator::locate] no group for " << pattern);
    MCE_INFO("[Allocator::locate] no group for " << pattern);
    return "";
  }


  if (fileName == "") {
    MCE_INFO("storage fileName null [" + pattern + "]");
    return "";
  }

  if(group->getName().empty())
      {
        group->setName(pattern.substr(0, pos));
      }
  // check whether we do something wrong that upload the picture to
  // head storage, if so, recorrect it
  if ((group->getName().find("hd") == 0)
    || (group->getName().find("kxhd") == 0)) {
    // :(, we have to add to subdirectory to recorrect it
    return "http://" + group->getReadableNode()->getDomain()
        + "/photos/" + group->getName() + "/" + fileName;
  } else if ( (group->getName().find("fmn") == 0)
    || (group->getName().find("kxfmn") == 0 )) {
    return "http://" + group->getReadableNode()->getDomain()
    + "/" + pattern ;
  }else {
    return "http://" + group->getReadableNode()->getDomain()
        + "/" + fileName;
  }
}





// ------------------- HeadUrlUtil ----------------------------------
string HeadUrlUtil::getHeadFullUrl(const string& pattern) {

	// 针对合并后的开心
	bool isKaixin = false;
	string pattern1 = pattern;
	if ( pattern1.find("kx/") == 0 ) {
		pattern1 = pattern1.substr(3, pattern1.length()-3);
		isKaixin = true;		
	}

	try {
		MCE_DEBUG("[HeadUrlUtil::getHeadFullUrl] pattern1 : " << pattern1);
		if (pattern1 == "") {
			return "";
		}
		string url = Allocator::instance().locate(pattern1, NODE_TYPE_NORMAL);
		if (url == "")	{				
			if (pattern1.find("head.jpg") != string::npos)
				url = URL_HEAD + "/photos/0/0/men_head.gif";
			else if (pattern1.find("main.jpg") != string::npos)
				url = URL_HEAD + "/photos/0/0/men_main.gif";
			else if (pattern1.find("tiny.jpg") != string::npos)
				url = URL_HEAD + "/photos/0/0/men_tiny.gif";
			else
				url = URL_HEAD + "/photos/" + pattern1;
				// url = URL_HEAD + "/photos/0/0/men_tiny.gif";
			}

			// kaixin
			if ( isKaixin ) {
				size_t pos = url.find("xnimg.cn");
				if ( pos != string::npos ) {
					string tmp = url.substr(0, pos) + "rrimg.com" + url.substr(pos+8, url.length()-pos-8);
					url = tmp;
				}
			}

			return url;
	} catch (Ice::Exception& e) {
		MCE_WARN("[HeadUrlUtil::getHeadFullUrl] get url Ice::Exception : " << e.what());
		return "";
	} catch (std::exception& e) {
		MCE_WARN("[HeadUrlUtil::getHeadFullUrl] get url std::exception : " << e.what());
		return "";
	} catch (...) {
		MCE_WARN("[HeadUrlUtil::getHeadFullUrl] get url exception ");
		return "";
	}
}

	
/**
 * 从外站得到用户头像的完整URL地址，避免图片防盗链
 * @param pattern
 * @return
 */
string HeadUrlUtil::getHeadFullUrlFromOuterSite(const string& pattern) {

	// 针对合并后的开心
	bool isKaixin = false;
	string pattern1 = pattern;
	if ( pattern1.find("kx/") == 0 ) {
		pattern1 = pattern1.substr(3, pattern1.length()-3);
		isKaixin = true;		
	}

	try {
		string url = getHeadFullUrl(pattern1);
		if (url == "") {
			url = URL_TINY_HEAD + "/photos/" + pattern1;
		} else {
			if (url.find("xnimg.cn") != string::npos){
				;
			}else{
				size_t pos = url.find("xiaonei.com");
				if ( pos != string::npos ) {
					string part2 = url.substr(pos, url.size()-pos);
					url = "http://tiny.head." + part2;
				} else {
					url = "http://tiny.head." + url;
				}
			}
		}

		// kaixin
		if ( isKaixin ) {
			size_t pos = url.find("xnimg.cn");
			if ( pos != string::npos ) {
				string tmp = url.substr(0, pos) + "rrimg.com" + url.substr(pos+8, url.length()-pos-8);
				url = tmp;
			}
		}

		return url;
	} catch (Ice::Exception& e) {
		MCE_WARN("[HeadUrlUtil::getHeadFullUrlFromOuterSite] Ice::Exception : " << e.what());
		return "";	       	
	} catch (std::exception& e) {
		MCE_WARN("[HeadUrlUtil::getHeadFullUrlFromOuterSite] std::exception : " << e.what());
		return "";
	} catch (...) {
		MCE_WARN("[HeadUrlUtil::getHeadFullUrlFromOuterSite] Unknown Exception ");
		return "";
	}
}


// ================= ResultHandler部分 ================================
// ----------------- BatchLikeshipHandler ------------------------------
bool GroupResultHandler::handle(mysqlpp::Row& row) const {

	try {
		NodePtr node = new Node;
		node->setId((int) row[COLUMN_NODE_ID.c_str()]);
		node->setName((string) row[COLUMN_NODE_NAME.c_str()]);
		node->setDomain((string) row[COLUMN_DOMAIN.c_str()]);	
		node->setGroup((string) row[COLUMN_GROUP_NAME.c_str()]);
		node->setPath((string) row[COLUMN_PATH.c_str()]);
		node->setSpaceUsed((double) row[COLUMN_SPACE_USED.c_str()]);
		node->setCpuLoad((double) row[COLUMN_CPU_LOAD.c_str()]);
		node->setStatus((int) row[COLUMN_STATUS.c_str()]);
		node->setType((int) row[COLUMN_TYPE.c_str()]);
		node->setTime(time(NULL));

		string groupName = node->getGroup();
		if ( _result.find(groupName) != _result.end() ) {
			_result[groupName]->addNode(node);
		} else {
			GroupPtr group = new Group;
			group->addNode(node);
			_result.insert(make_pair<string, GroupPtr>(groupName, group));
		}
	} catch (...) {
		MCE_WARN("[GroupResultHandler::handle] add node exception ");
	}
		
	return true; 
}
