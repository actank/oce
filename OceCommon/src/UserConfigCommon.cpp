#include "UserConfigCommon.h"
#include "LogWrapper.h"
#include "boost/lexical_cast.hpp"
using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace boost;
//---------------------------------------------------------------------------
string UserConfigDataI::getProperty(const string& key)
{
    IceUtil::Mutex::Lock lock(*this);
    
    if (!strcasecmp(key.c_str(), CUCUSERID.c_str())) {
        return boost::lexical_cast<string>(userId);
    } else if (!strcasecmp(key.c_str(), CUCBASICCONFIG.c_str())) {
        return boost::lexical_cast<string>(basicConfig);
    } else if (!strcasecmp(key.c_str(), CUCPOKECONFIG.c_str())) {
        return boost::lexical_cast<string>(pokeConfig);
    } else if (!strcasecmp(key.c_str(), CUCREQUESTFRIENDCONFIG.c_str())) {
        return boost::lexical_cast<string>(requestFriendConfig);
    } else if (!strcasecmp(key.c_str(), CUCPHOTOCONFIG.c_str())){
    	return boost::lexical_cast<string>(photoConfig);
    } else if (!strcasecmp(key.c_str(), CUCMESSAGECONFIG.c_str())) {
	return boost::lexical_cast<string>(messageConfig);
    } else if (!strcasecmp(key.c_str(), CUCEMAILCONFIG.c_str())) {
        return boost::lexical_cast<string>(emailConfig);
    } else if (!strcasecmp(key.c_str(), CUCBROWSECONFIG.c_str())) {
        return boost::lexical_cast<string>(browseConfig);
    } else if (!strcasecmp(key.c_str(), CUCDISPLAYMENULIST.c_str())) {
        return displayMenuList;
    } else if (!strcasecmp(key.c_str(), CUCMOREMENULIST.c_str())) {
        return moreMenuList;
    } else if (!strcasecmp(key.c_str(), CUCWANTSEECSS.c_str())) {
        return boost::lexical_cast<string>(wantSeeCss);
    } else if (!strcasecmp(key.c_str(), CUCSENDFEEDCONFIG.c_str())) {
        return sendFeedConfig;
    } else if (!strcasecmp(key.c_str(), CUCRECVFEEDCONFIG.c_str())) {
		return recvFeedConfig;
	} else if (!strcasecmp(key.c_str(), CUCPROFILEPRIVACY.c_str())) {
		return profilePrivacy;
	} else if (!strcasecmp(key.c_str(), CUCSTATUSCONFIG.c_str())) {
		return  boost::lexical_cast<string>(statusConfig);
	}
	

	NoSuchPropertyException e;
    e.message = key;
    throw e;
}

void UserConfigDataI::setProperty(const string& key, const string& val)
{
    IceUtil::Mutex::Lock lock(*this);
    if (!strcasecmp(key.c_str(), CUCUSERID.c_str())) {
        userId = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCBASICCONFIG.c_str())) {
        basicConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCPOKECONFIG.c_str())) {
        pokeConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCREQUESTFRIENDCONFIG.c_str())) {
        requestFriendConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCPHOTOCONFIG.c_str())) {
        photoConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCMESSAGECONFIG.c_str())) {
        messageConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCEMAILCONFIG.c_str())) {
        emailConfig = boost::lexical_cast<Ice::Long>(val);
    } else if (!strcasecmp(key.c_str(), CUCBROWSECONFIG.c_str())) {
        browseConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCDISPLAYMENULIST.c_str())) {
        displayMenuList = val;
    } else if (!strcasecmp(key.c_str(), CUCMOREMENULIST.c_str())) {
        moreMenuList = val;
    } else if (!strcasecmp(key.c_str(), CUCWANTSEECSS.c_str())) {
        wantSeeCss = boost::lexical_cast<Ice::Short>(val);
    } else if (!strcasecmp(key.c_str(), CUCSENDFEEDCONFIG.c_str())) {
        sendFeedConfig = val;
    } else if (!strcasecmp(key.c_str(), CUCRECVFEEDCONFIG.c_str())) {
		recvFeedConfig = val;
	}else if (!strcasecmp(key.c_str(), CUCPROFILEPRIVACY.c_str())) {
		profilePrivacy = val;
	} else if (!strcasecmp(key.c_str(), CUCSTATUSCONFIG.c_str())) {
		statusConfig = boost::lexical_cast<Ice::Int>(val);

	}
}

Str2StrMap UserConfigDataI::getProperties()
{
    Str2StrMap result;
    result[CUCUSERID] = getProperty(CUCUSERID);
    result[CUCBASICCONFIG] = getProperty(CUCBASICCONFIG);
    result[CUCPOKECONFIG] = getProperty(CUCPOKECONFIG);
    result[CUCREQUESTFRIENDCONFIG] = getProperty(CUCREQUESTFRIENDCONFIG);
    result[CUCPHOTOCONFIG] = getProperty(CUCPHOTOCONFIG);
    result[CUCMESSAGECONFIG] = getProperty(CUCMESSAGECONFIG);
    result[CUCEMAILCONFIG] = getProperty(CUCEMAILCONFIG);
    result[CUCBROWSECONFIG] = getProperty(CUCBROWSECONFIG);
    result[CUCDISPLAYMENULIST] = getProperty(CUCDISPLAYMENULIST);
    result[CUCMOREMENULIST] = getProperty(CUCMOREMENULIST);
    result[CUCWANTSEECSS] = getProperty(CUCWANTSEECSS);
    result[CUCSENDFEEDCONFIG] = getProperty(CUCSENDFEEDCONFIG);
    result[CUCRECVFEEDCONFIG] = getProperty(CUCRECVFEEDCONFIG);
    result[CUCPROFILEPRIVACY] = getProperty(CUCPROFILEPRIVACY);
    result[CUCSTATUSCONFIG] = getProperty(CUCSTATUSCONFIG);
    

    return result;
}

void 
UserConfigDataI::setProperties(const Str2StrMap& properties)
{
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        setProperty(it->first, it->second);
    }
}

void UserConfigDataI::parse(Ice::ObjectPtr& obj)
{
	UserConfigDataPtr data = UserConfigDataPtr::dynamicCast(obj);
    userId = data->userId ;
    basicConfig = data->basicConfig ;
    pokeConfig = data->pokeConfig ;
    requestFriendConfig = data->requestFriendConfig ;
    photoConfig = data->photoConfig ;
    messageConfig = data->messageConfig ;
    emailConfig = data->emailConfig ;
    browseConfig = data->browseConfig ;
    displayMenuList = data->displayMenuList ;
    moreMenuList = data->moreMenuList ;
    wantSeeCss = data->wantSeeCss ;
    sendFeedConfig = data->sendFeedConfig ;
    recvFeedConfig = data->recvFeedConfig ;
    profilePrivacy = data->profilePrivacy ;
    statusConfig = data->statusConfig ;
    LOG_DEBUG("Oce", "DISPLAY:"<< displayMenuList);

}

void UserConfigDataI::parse(mysqlpp::Row& row )
{
//    UserConfigPtr uc = new UserConfig();
    userId = (int)(row[CUCUSERID.c_str()]);
    basicConfig = (int)(row[CUCBASICCONFIG.c_str()]);
    pokeConfig = (int)(row[CUCPOKECONFIG.c_str()]);
    requestFriendConfig = (int)(row[CUCREQUESTFRIENDCONFIG.c_str()]);
    photoConfig = (int)(row[CUCPHOTOCONFIG.c_str()]);
    messageConfig = (int)(row[CUCMESSAGECONFIG.c_str()]);
    emailConfig = (long)(row[CUCEMAILCONFIG.c_str()]);
    browseConfig = (int)(row[CUCBROWSECONFIG.c_str()]);
#ifndef NEWARCH
    displayMenuList = row[CUCDISPLAYMENULIST.c_str()].get_string();
    moreMenuList = row[CUCMOREMENULIST.c_str()].get_string();
#else
    displayMenuList = row[CUCDISPLAYMENULIST.c_str()].data();
    moreMenuList = row[CUCMOREMENULIST.c_str()].data();
#endif
    wantSeeCss = (short)(row[CUCWANTSEECSS.c_str()]);
#ifndef NEWARCH
    sendFeedConfig = row[CUCSENDFEEDCONFIG.c_str()].get_string();
    recvFeedConfig = row[CUCRECVFEEDCONFIG.c_str()].get_string();
    profilePrivacy = row[CUCPROFILEPRIVACY.c_str()].get_string();
#else
    sendFeedConfig = row[CUCSENDFEEDCONFIG.c_str()].data();
    recvFeedConfig = row[CUCRECVFEEDCONFIG.c_str()].data();
    profilePrivacy = row[CUCPROFILEPRIVACY.c_str()].data();
#endif
    statusConfig = (int)(row[CUCSTATUSCONFIG.c_str()]);
    LOG_DEBUG("Oce", "DISPLAY:"<< displayMenuList);

}

//---------------------------------------------------------------------------

