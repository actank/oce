#include "UserCommon.h"
#include "LogWrapper.h"

using namespace mop::hi::svc::model;
using namespace MyUtil;

//---------------------------------------------------------------------------
string 
UserHandle::getProperty(const string& key)
{
    IceUtil::Mutex::Lock lock(*this);
    LOG_DEBUG("Oce", "-----call user::getProperty()----");
    if (!strcasecmp(key.c_str(), CUID.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->id);
    } else if (!strcasecmp(key.c_str(), CUPASSWORD.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->password);
    } else if (!strcasecmp(key.c_str(), CUEMAIL.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->email);
    } else if (!strcasecmp(key.c_str(), CUNAME.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->name);
    } else if (!strcasecmp(key.c_str(), CUFRIENDCOUNT.c_str())){
    	return boost::lexical_cast<string>(value<UserDataPtr>()->friendCount);
    } else if (!strcasecmp(key.c_str(), CUNEWMESSAGECOUNT.c_str())) {
		return boost::lexical_cast<string>(value<UserDataPtr>()->newMessageCount);
    } else if (!strcasecmp(key.c_str(), CUPOSTCOUNT.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->postCount);
    } else if (!strcasecmp(key.c_str(), CUSTATUS.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->status);
    } else if (!strcasecmp(key.c_str(), CULOGINCOUNT.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->loginCount);
    } else if (!strcasecmp(key.c_str(), CUHEADURL.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->headUrl);
    } 
    //else if (!strcasecmp(key.c_str(), CUMESSAGEINCOUNT.c_str())) {
    //    return boost::lexical_cast<string>(value<UserDataPtr>()->messageInCount);
    //} 
    else if (!strcasecmp(key.c_str(), CULEAVEWORDCOUNT.c_str())){
    	return boost::lexical_cast<string>(value<UserDataPtr>()->leavewordCount);
    } else if (!strcasecmp(key.c_str(), CUSELECTED.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->selected);
    } else if (!strcasecmp(key.c_str(), CUVERIFYCODE.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->verifyCode);
    } else if (!strcasecmp(key.c_str(), CUAUTH.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->auth);
    } else if (!strcasecmp(key.c_str(), CUNEWEMAIL.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->newEmail);
    } else if (!strcasecmp(key.c_str(), CUEMAILVERIFY.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->emailVerify);
    } else if (!strcasecmp(key.c_str(), CUTRIBECOUNT.c_str())){
    	return boost::lexical_cast<string>(value<UserDataPtr>()->tribeCount);
    } else if (!strcasecmp(key.c_str(), CUGENDER.c_str())) {
		return boost::lexical_cast<string>(value<UserDataPtr>()->gender);
    } else if (!strcasecmp(key.c_str(), CUUNIVERSITY.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->university);
    } else if (!strcasecmp(key.c_str(), CUEXP.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->exp);
    } else if (!strcasecmp(key.c_str(), CULARGEURL.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->largeUrl);
    } else if (!strcasecmp(key.c_str(), CUBLOGCONTROL.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->blogControl);
    } else if (!strcasecmp(key.c_str(), CUNOTIFY.c_str())){
    	return boost::lexical_cast<string>(value<UserDataPtr>()->notify);
    } else if (!strcasecmp(key.c_str(), CULEAVEWORDFLAG.c_str())) {
		return boost::lexical_cast<string>(value<UserDataPtr>()->leavewordFlag);
    } else if (!strcasecmp(key.c_str(), CUENTRYCOMMENTFLAG.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->entryCommentFlag);
    } else if (!strcasecmp(key.c_str(), CUSTAR.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->star);
    } else if (!strcasecmp(key.c_str(), CUPHOTOCONTROL.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->photoControl);
    } else if (!strcasecmp(key.c_str(), CUGUESTREQUESTCOUNT.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->guestRequestCount);
    } else if (!strcasecmp(key.c_str(), CUTRIBEREQUESTCOUNT.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->tribeRequestCount);
    } else if (!strcasecmp(key.c_str(), CUCOMMONRIGHTS.c_str())){
    	return boost::lexical_cast<string>(value<UserDataPtr>()->commonRights);
    } else if (!strcasecmp(key.c_str(), CUPASSWORDMD5.c_str())) {
		return boost::lexical_cast<string>(value<UserDataPtr>()->passwordMd5);
    } else if (!strcasecmp(key.c_str(), CUENTRY.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->entry);
    } else if (!strcasecmp(key.c_str(), CUVERIFYREQUESTCOUNT.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->verifyRequestCount);
    } else if (!strcasecmp(key.c_str(), CUSPACESIZE.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->spaceSize);
    } else if (!strcasecmp(key.c_str(), CUSIGNATURE.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->signature);
    } else if (!strcasecmp(key.c_str(), CUQMAIL.c_str())){
    	return boost::lexical_cast<string>(value<UserDataPtr>()->qmail);
    } else if (!strcasecmp(key.c_str(), CUSOURCE.c_str())) {
		return boost::lexical_cast<string>(value<UserDataPtr>()->source);
    //} else if (!strcasecmp(key.c_str(), CUOPENCONFIG.c_str())) {
    //    return boost::lexical_cast<string>(value<UserDataPtr>()->openConfig);
    //} else if (!strcasecmp(key.c_str(), CUCHATCONFIG.c_str())) {
    //    return boost::lexical_cast<string>(value<UserDataPtr>()->chatConfig);
    } else if (!strcasecmp(key.c_str(), CUTEMPLATE.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->template1);
    } else if (!strcasecmp(key.c_str(), CUCLASSINVITEDCOUNT.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->classInvitedCount);
    } else if (!strcasecmp(key.c_str(), CUCLASSJOINCOUNT.c_str())) {
	    return boost::lexical_cast<string>(value<UserDataPtr>()->classJoinCount);
	} else if (!strcasecmp(key.c_str(), CUCLASSCREATECOUNT.c_str())) {
	    return boost::lexical_cast<string>(value<UserDataPtr>()->classCreateCount);
	} else if (!strcasecmp(key.c_str(), CUSTAGE.c_str())) {
        return boost::lexical_cast<string>(value<UserDataPtr>()->stage);
    } //else if (!strcasecmp(key.c_str(), CUCURNETWORKID.c_str())){
    	//return boost::lexical_cast<string>(value<UserDataPtr>()->curNetworkId);
   // } else if (!strcasecmp(key.c_str(), CUCURNETWORKNAME.c_str())) {
//		return boost::lexical_cast<string>(value<UserDataPtr>()->curNetworkName);
    //} 
	else if (!strcasecmp(key.c_str(), CUISOLDUSER.c_str())) {
                return boost::lexical_cast<string>(value<UserDataPtr>()->isOldUser);
    } 
     
    LOG_DEBUG("Oce", "--- finish User getProperty    kye:"<<key<<"-----");
    NoSuchPropertyException e;
    e.message = key;
    throw e;
}

void
UserHandle::setProperty(const string& key, const string& val)
{
    IceUtil::Mutex::Lock lock(*this);
        
    if (!strcasecmp(key.c_str(), CUID.c_str())) {
        value<UserDataPtr>()->id = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUPASSWORD.c_str())) {
        value<UserDataPtr>()->password = val;
    } else if (!strcasecmp(key.c_str(), CUEMAIL.c_str())) {
        value<UserDataPtr>()->email = val;
    } else if (!strcasecmp(key.c_str(), CUNAME.c_str())) {
        value<UserDataPtr>()->name = val;
    } else if (!strcasecmp(key.c_str(), CUFRIENDCOUNT.c_str())){
    	value<UserDataPtr>()->friendCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUNEWMESSAGECOUNT.c_str())) {
		value<UserDataPtr>()->newMessageCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUPOSTCOUNT.c_str())) {
        value<UserDataPtr>()->postCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUSTATUS.c_str())) {
        value<UserDataPtr>()->status = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CULOGINCOUNT.c_str())) {
        value<UserDataPtr>()->loginCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUHEADURL.c_str())) {
        value<UserDataPtr>()->headUrl = val;
    } 
    //else if (!strcasecmp(key.c_str(), CUMESSAGEINCOUNT.c_str())) {
    //    value<UserDataPtr>()->messageInCount = boost::lexical_cast<Ice::Int>(val);
    //} 
    else if (!strcasecmp(key.c_str(), CULEAVEWORDCOUNT.c_str())){
    	value<UserDataPtr>()->leavewordCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUSELECTED.c_str())) {
        value<UserDataPtr>()->selected = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUVERIFYCODE.c_str())) {
        value<UserDataPtr>()->verifyCode = val;
    } else if (!strcasecmp(key.c_str(), CUAUTH.c_str())) {
        value<UserDataPtr>()->auth = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUNEWEMAIL.c_str())) {
        value<UserDataPtr>()->newEmail = val;
    } else if (!strcasecmp(key.c_str(), CUEMAILVERIFY.c_str())) {
        value<UserDataPtr>()->emailVerify = val;
    } else if (!strcasecmp(key.c_str(), CUTRIBECOUNT.c_str())){
    	value<UserDataPtr>()->tribeCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUGENDER.c_str())) {
		value<UserDataPtr>()->gender = val;
    } else if (!strcasecmp(key.c_str(), CUUNIVERSITY.c_str())) {
        value<UserDataPtr>()->university = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUEXP.c_str())) {
        value<UserDataPtr>()->exp = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CULARGEURL.c_str())) {
        value<UserDataPtr>()->largeUrl = val;
    } else if (!strcasecmp(key.c_str(), CUBLOGCONTROL.c_str())) {
        value<UserDataPtr>()->blogControl = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUNOTIFY.c_str())){
    	value<UserDataPtr>()->notify = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CULEAVEWORDFLAG.c_str())) {
		value<UserDataPtr>()->leavewordFlag = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUENTRYCOMMENTFLAG.c_str())) {
        value<UserDataPtr>()->entryCommentFlag = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUSTAR.c_str())) {
        value<UserDataPtr>()->star = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUPHOTOCONTROL.c_str())) {
        value<UserDataPtr>()->photoControl = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUGUESTREQUESTCOUNT.c_str())) {
        value<UserDataPtr>()->guestRequestCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUTRIBEREQUESTCOUNT.c_str())) {
        value<UserDataPtr>()->tribeRequestCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCOMMONRIGHTS.c_str())){
    	value<UserDataPtr>()->commonRights = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUPASSWORDMD5.c_str())) {
		value<UserDataPtr>()->passwordMd5 = val;
    } else if (!strcasecmp(key.c_str(), CUENTRY.c_str())) {
        value<UserDataPtr>()->entry = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUVERIFYREQUESTCOUNT.c_str())) {
        value<UserDataPtr>()->verifyRequestCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUSPACESIZE.c_str())) {
        value<UserDataPtr>()->spaceSize = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUSIGNATURE.c_str())) {
        value<UserDataPtr>()->signature = val;
    } else if (!strcasecmp(key.c_str(), CUQMAIL.c_str())){
    	value<UserDataPtr>()->qmail = val;
    } else if (!strcasecmp(key.c_str(), CUSOURCE.c_str())) {
		value<UserDataPtr>()->source = boost::lexical_cast<Ice::Int>(val);
    //} else if (!strcasecmp(key.c_str(), CUOPENCONFIG.c_str())) {
    //    value<UserDataPtr>()->openConfig = boost::lexical_cast<Ice::Int>(val);
    //} else if (!strcasecmp(key.c_str(), CUCHATCONFIG.c_str())) {
    //    value<UserDataPtr>()->chatConfig = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUTEMPLATE.c_str())) {
        value<UserDataPtr>()->template1 = val;
    } else if (!strcasecmp(key.c_str(), CUCLASSINVITEDCOUNT.c_str())) {
        value<UserDataPtr>()->classInvitedCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUCLASSJOINCOUNT.c_str())) {
	    value<UserDataPtr>()->classJoinCount = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUCLASSCREATECOUNT.c_str())) {
	    value<UserDataPtr>()->classCreateCount = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CUSTAGE.c_str())) {
        value<UserDataPtr>()->stage = boost::lexical_cast<Ice::Int>(val);
    } //else if (!strcasecmp(key.c_str(), CUCURNETWORKID.c_str())){
    	//value<UserDataPtr>()->curNetworkId = boost::lexical_cast<Ice::Int>(val);
    //} else if (!strcasecmp(key.c_str(), CUCURNETWORKNAME.c_str())) {
//		value<UserDataPtr>()->curNetworkName = val;
    //} 
	else if (!strcasecmp(key.c_str(), CUISOLDUSER.c_str())) {
                value<UserDataPtr>()->isOldUser = boost::lexical_cast<Ice::Int>(val);
    }
}

Str2StrMap 
UserHandle::getProperties()
{
    Str2StrMap result;
    result[CUID] 				= getProperty(CUID);                
	result[CUPASSWORD] 			= getProperty(CUPASSWORD);          
	result[CUEMAIL] 			= getProperty(CUEMAIL);             
	result[CUNAME] 				= getProperty(CUNAME);              
	result[CUFRIENDCOUNT] 		= getProperty(CUFRIENDCOUNT);       
	result[CUNEWMESSAGECOUNT] 	= getProperty(CUNEWMESSAGECOUNT);   
	result[CUPOSTCOUNT] 		= getProperty(CUPOSTCOUNT);         
	result[CUSTATUS] 			= getProperty(CUSTATUS);            
	result[CULOGINCOUNT] 		= getProperty(CULOGINCOUNT);        
	result[CUHEADURL] 			= getProperty(CUHEADURL);           
	//result[CUMESSAGEINCOUNT] 	= getProperty(CUMESSAGEINCOUNT);    
	result[CULEAVEWORDCOUNT] 	= getProperty(CULEAVEWORDCOUNT);    
	//result[CUMESSAGEOUTCOUNT] 	= getProperty(CUMESSAGEOUTCOUNT);   
	result[CUSELECTED] 			= getProperty(CUSELECTED);          
	result[CUVERIFYCODE] 		= getProperty(CUVERIFYCODE);        
	result[CUAUTH] 				= getProperty(CUAUTH);              
	result[CUNEWEMAIL] 			= getProperty(CUNEWEMAIL);          
	result[CUEMAILVERIFY] 		= getProperty(CUEMAILVERIFY);       
	result[CUTRIBECOUNT] 		= getProperty(CUTRIBECOUNT);        
	result[CUGENDER] 			= getProperty(CUGENDER);            
	result[CUUNIVERSITY] 		= getProperty(CUUNIVERSITY);        
	result[CUEXP] 				= getProperty(CUEXP);               
	//result[CULASTIP] 			= getProperty(CULASTIP);            
	result[CULARGEURL] 			= getProperty(CULARGEURL);          
	result[CUBLOGCONTROL] 		= getProperty(CUBLOGCONTROL);       
	result[CUNOTIFY] 			= getProperty(CUNOTIFY);            
	result[CULEAVEWORDFLAG] 	= getProperty(CULEAVEWORDFLAG);     
	result[CUENTRYCOMMENTFLAG] 	= getProperty(CUENTRYCOMMENTFLAG);  
	result[CUSTAR] 				= getProperty(CUSTAR);              
	result[CUPHOTOCONTROL] 		= getProperty(CUPHOTOCONTROL);      
	result[CUGUESTREQUESTCOUNT] = getProperty(CUGUESTREQUESTCOUNT); 
	result[CUTRIBEREQUESTCOUNT] = getProperty(CUTRIBEREQUESTCOUNT); 
	result[CUCOMMONRIGHTS] 		= getProperty(CUCOMMONRIGHTS);      
	result[CUPASSWORDMD5] 		= getProperty(CUPASSWORDMD5);       
	result[CUENTRY] 			= getProperty(CUENTRY);             
	result[CUVERIFYREQUESTCOUNT]= getProperty(CUVERIFYREQUESTCOUNT);
	result[CUSPACESIZE] 		= getProperty(CUSPACESIZE);         
	//result[CUCOOKIEID] 			= getProperty(CUCOOKIEID);          
	result[CUSIGNATURE] 		= getProperty(CUSIGNATURE);         
	result[CUQMAIL] 			= getProperty(CUQMAIL);             
	result[CUSOURCE] 			= getProperty(CUSOURCE);            
	//result[CUOPENCONFIG] 		= getProperty(CUOPENCONFIG);        
	//result[CUCHATCONFIG] 		= getProperty(CUCHATCONFIG);        
	result[CUTEMPLATE] 			= getProperty(CUTEMPLATE);          
	result[CUCLASSCREATECOUNT] 	= getProperty(CUCLASSCREATECOUNT);  
	result[CUCLASSJOINCOUNT] 	= getProperty(CUCLASSJOINCOUNT);    
	result[CUCLASSINVITEDCOUNT] = getProperty(CUCLASSINVITEDCOUNT); 
	result[CUSTAGE] 			= getProperty(CUSTAGE);             
	//result[CUCURNETWORKID] 		= getProperty(CUCURNETWORKID);      
	//result[CUCURNETWORKNAME] 	= getProperty(CUCURNETWORKNAME);
	result[CUISOLDUSER] 	= getProperty(CUISOLDUSER);
    

    return result;
}

void 
UserHandle::setProperties(const Str2StrMap& properties)
{
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        setProperty(it->first, it->second);
    }
}

DataHandlePtr 
UserHandle::parse(mysqlpp::Row& row)
{    
    DataHandlePtr d = new UserHandle;
    UserDataPtr u = new UserData();
	u->id = (int)row[CUID.c_str()];
#ifndef NEWARCH
	u->password = row[CUPASSWORD.c_str()].get_string();
	u->email = row[ CUEMAIL.c_str()].get_string();
	u->name = row[ CUNAME.c_str()].get_string();
#else
	u->password = row[CUPASSWORD.c_str()].data();
	u->email = row[ CUEMAIL.c_str()].data();
	u->name = row[ CUNAME.c_str()].data();
#endif
	u->friendCount = (int)row[ CUFRIENDCOUNT.c_str()];
	u->newMessageCount = (int)row[ CUNEWMESSAGECOUNT.c_str()];
	u->postCount = (int)row[ CUPOSTCOUNT.c_str()];
	u->status = (int)row[ CUSTATUS.c_str()];
	u->loginCount = (int)row[ CULOGINCOUNT.c_str()];
#ifndef NEWARCH
	u->headUrl = row[ CUHEADURL.c_str()].get_string();
#else
	u->headUrl = row[ CUHEADURL.c_str()].data();
#endif
	u->leavewordCount = (int)row[ CULEAVEWORDCOUNT.c_str()];
	u->selected = (int)row[ CUSELECTED.c_str()];
#ifndef NEWARCH
	u->verifyCode = row[ CUVERIFYCODE.c_str()].get_string();
#else
	u->verifyCode = row[ CUVERIFYCODE.c_str()].data();
#endif
	u->auth = (int)row[ CUAUTH.c_str()];
#ifndef NEWARCH
	u->newEmail = row[ CUNEWEMAIL.c_str()].get_string();
	u->emailVerify = row[ CUEMAILVERIFY.c_str()].get_string();
#else
	u->newEmail = row[ CUNEWEMAIL.c_str()].data();
	u->emailVerify = row[ CUEMAILVERIFY.c_str()].data();
#endif
	u->tribeCount = (int)row[ CUTRIBECOUNT.c_str()];
#ifndef NEWARCH
	u->gender = row[ CUGENDER.c_str()].get_string();
#else
	u->gender = row[ CUGENDER.c_str()].data();
#endif
	u->university = (int)row[ CUUNIVERSITY.c_str()];
	u->exp = (int)row[ CUEXP.c_str()];
#ifndef NEWARCH
	u->largeUrl = row[ CULARGEURL.c_str()].get_string();
#else
	u->largeUrl = row[ CULARGEURL.c_str()].data();
#endif
	u->blogControl = (int)row[ CUBLOGCONTROL.c_str()];
	u->notify = (int)row[ CUNOTIFY.c_str()];
	u->leavewordFlag = (int)row[ CULEAVEWORDFLAG.c_str()];
	u->entryCommentFlag = (int)row[ CUENTRYCOMMENTFLAG.c_str()];
	u->star = (int)row[ CUSTAR.c_str()];
	u->photoControl = (int)row[ CUPHOTOCONTROL.c_str()];
	u->guestRequestCount= (int)row[ CUGUESTREQUESTCOUNT.c_str()];
	u->tribeRequestCount= (int)row[ CUTRIBEREQUESTCOUNT.c_str()];
	u->commonRights = (int)row[ CUCOMMONRIGHTS.c_str()];
#ifndef NEWARCH
	u->passwordMd5 = row[ CUPASSWORDMD5.c_str()].get_string();
#else
	u->passwordMd5 = row[ CUPASSWORDMD5.c_str()].data();
#endif
	if(row[ CUENTRY.c_str()].is_null()) u->entry = 0;
	else u->entry = (int)row[ CUENTRY.c_str()];
	u->verifyRequestCount= (int)row[ CUVERIFYREQUESTCOUNT.c_str()];
	u->spaceSize = (int)row[ CUSPACESIZE.c_str()];
#ifndef NEWARCH
	u->signature = row[ CUSIGNATURE.c_str()].get_string();
	u->qmail = row[ CUQMAIL.c_str()].get_string();
#else
	u->signature = row[ CUSIGNATURE.c_str()].data();
	u->qmail = row[ CUQMAIL.c_str()].data();
#endif
	if(row[ CUSOURCE.c_str()].is_null()) u->source = 0;
	else u->source = (int)row[ CUSOURCE.c_str()];
#ifndef NEWARCH
	u->template1 = row[ CUTEMPLATE.c_str()].get_string();
#else
	u->template1 = row[ CUTEMPLATE.c_str()].data();
#endif
	u->classInvitedCount= (int)row[ CUCLASSINVITEDCOUNT.c_str()];
	u->classJoinCount= (int)row[ CUCLASSJOINCOUNT.c_str()];
	u->classCreateCount= (int)row[ CUCLASSCREATECOUNT.c_str()];
	u->stage = (int)row[ CUSTAGE.c_str()];
	u->isOldUser= (int)row[ CUISOLDUSER.c_str()];
     
	
    d->value(u);  
    LOG_DEBUG("Oce", "---finish user:: parse()----");
    return d;
}



//---------------------------------------------------------------------------
string 
UserBasicHandle::getProperty(const string& key)
{
    IceUtil::Mutex::Lock lock(*this);

	if (!strcasecmp(key.c_str(), CUBID.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->id);


	} else if (!strcasecmp(key.c_str(), CUBUNIV.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->univ);


	} else if (!strcasecmp(key.c_str(), CUBSELECTED.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->selected);


	} else if (!strcasecmp(key.c_str(), CUBNAME.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->name);


	} else if (!strcasecmp(key.c_str(), CUBSTATUS.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->status);


	} else if (!strcasecmp(key.c_str(), CUBGENDER.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->gender);


	} else if (!strcasecmp(key.c_str(), CUBUNIVYEAR.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->univYear);


	} else if (!strcasecmp(key.c_str(), CUBDEPARTMENT.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->department);


	} else if (!strcasecmp(key.c_str(), CUBDORM.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->dorm);


	} else if (!strcasecmp(key.c_str(), CUBBIRTHDAY.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->birthDay);


	} else if (!strcasecmp(key.c_str(), CUBBIRTHMONTH.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->birthMonth);


	} else if (!strcasecmp(key.c_str(), CUBBIRTHYEAR.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->birthYear);


	} else if (!strcasecmp(key.c_str(), CUBHOMEPROVINCE.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return value<UserBasicDataPtr>()->homeProvince;


	} else if (!strcasecmp(key.c_str(), CUBHOMECITY.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->homeCity);


	} else if (!strcasecmp(key.c_str(), CUBCITYCODE.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->cityCode);


	} else if (!strcasecmp(key.c_str(), CUBHIGHSCHOOL.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);

		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->highSchool);

	} else if (!strcasecmp(key.c_str(), CUBHIGHSCHOOLCODE.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->highSchoolCode);


	} else if (!strcasecmp(key.c_str(), CUBASTROLOGY.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->astrology);


	} else if (!strcasecmp(key.c_str(), CUBHEADURL.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->headUrl);


	} else if (!strcasecmp(key.c_str(), CUBTINYURL.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->tinyUrl);


	} else if (!strcasecmp(key.c_str(), CUBBASICCONTROL.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->basicControl);


	} else if (!strcasecmp(key.c_str(), CUBSTAR.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->star);


	} else if (!strcasecmp(key.c_str(), CUBDOING.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->doing);


	} else if (!strcasecmp(key.c_str(), CUBDOINGTIME.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->doingTime);


	} else if (!strcasecmp(key.c_str(), CUBSTAGE.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->stage);


//	} else if (!strcasecmp(key.c_str(), CUBCURNETWORKID.c_str())) {
//	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
//		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->curNetworkId);


//	} else if (!strcasecmp(key.c_str(), CUBCURNETWORKNAME.c_str())) {
//	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
//		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->curNetworkName);


//	} else if (!strcasecmp(key.c_str(), CUBJOINYEAR.c_str())) {
//	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
//		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->joinYear);


	} else if (!strcasecmp(key.c_str(), CUBROOM.c_str())) {
	    LOG_DEBUG("Oce", "---UserBasic::getProperty   line:"<<__LINE__);
		return boost::lexical_cast<string>(value<UserBasicDataPtr>()->room);


	} 
    
    NoSuchPropertyException e;
    e.message = key;
    throw e;
}

void
UserBasicHandle::setProperty(const string& key, const string& val)
{
    IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CUBID.c_str())) {
		value<UserBasicDataPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBUNIV.c_str())) {
		value<UserBasicDataPtr>()->univ = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBSELECTED.c_str())) {
		value<UserBasicDataPtr>()->selected = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBNAME.c_str())) {
		value<UserBasicDataPtr>()->name = val;
	} else if (!strcasecmp(key.c_str(), CUBSTATUS.c_str())) {
		value<UserBasicDataPtr>()->status = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBGENDER.c_str())) {
		value<UserBasicDataPtr>()->gender = val;
	} else if (!strcasecmp(key.c_str(), CUBUNIVYEAR.c_str())) {
		value<UserBasicDataPtr>()->univYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBDEPARTMENT.c_str())) {
		value<UserBasicDataPtr>()->department = val;
	} else if (!strcasecmp(key.c_str(), CUBDORM.c_str())) {
		value<UserBasicDataPtr>()->dorm = val;
	} else if (!strcasecmp(key.c_str(), CUBBIRTHDAY.c_str())) {
		value<UserBasicDataPtr>()->birthDay = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBBIRTHMONTH.c_str())) {
		value<UserBasicDataPtr>()->birthMonth = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBBIRTHYEAR.c_str())) {
		value<UserBasicDataPtr>()->birthYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBHOMEPROVINCE.c_str())) {
		value<UserBasicDataPtr>()->homeProvince = val;
	} else if (!strcasecmp(key.c_str(), CUBHOMECITY.c_str())) {
		value<UserBasicDataPtr>()->homeCity = val;
	} else if (!strcasecmp(key.c_str(), CUBCITYCODE.c_str())) {
		value<UserBasicDataPtr>()->cityCode = val;
	} else if (!strcasecmp(key.c_str(), CUBHIGHSCHOOL.c_str())) {
		value<UserBasicDataPtr>()->highSchool = val;
	} else if (!strcasecmp(key.c_str(), CUBHIGHSCHOOLCODE.c_str())) {
		value<UserBasicDataPtr>()->highSchoolCode = val;
	} else if (!strcasecmp(key.c_str(), CUBASTROLOGY.c_str())) {
		value<UserBasicDataPtr>()->astrology = val;
	} else if (!strcasecmp(key.c_str(), CUBHEADURL.c_str())) {
		value<UserBasicDataPtr>()->headUrl = val;
	} else if (!strcasecmp(key.c_str(), CUBTINYURL.c_str())) {
		value<UserBasicDataPtr>()->tinyUrl = val;
	} else if (!strcasecmp(key.c_str(), CUBBASICCONTROL.c_str())) {
		value<UserBasicDataPtr>()->basicControl = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBSTAR.c_str())) {
		value<UserBasicDataPtr>()->star = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBDOING.c_str())) {
		value<UserBasicDataPtr>()->doing = val;
	} else if (!strcasecmp(key.c_str(), CUBDOINGTIME.c_str())) {
		value<UserBasicDataPtr>()->doingTime = val;
	} else if (!strcasecmp(key.c_str(), CUBSTAGE.c_str())) {
		value<UserBasicDataPtr>()->stage = boost::lexical_cast<Ice::Int>(val);
//	} else if (!strcasecmp(key.c_str(), CUBCURNETWORKID.c_str())) {
//		value<UserBasicDataPtr>()->curNetworkId = boost::lexical_cast<Ice::Int>(val);
//	} else if (!strcasecmp(key.c_str(), CUBCURNETWORKNAME.c_str())) {
//		value<UserBasicDataPtr>()->curNetworkName = val;
//	} else if (!strcasecmp(key.c_str(), CUBJOINYEAR.c_str())) {
//		value<UserBasicDataPtr>()->joinYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBROOM.c_str())) {
		value<UserBasicDataPtr>()->room = val;
	} 
}

Str2StrMap 
UserBasicHandle::getProperties()
{
    Str2StrMap result;
	result[CUBID] = getProperty(CUBID);
	result[CUBUNIV] = getProperty(CUBUNIV);
	result[CUBSELECTED] = getProperty(CUBSELECTED);
	result[CUBNAME] = getProperty(CUBNAME);
	result[CUBSTATUS] = getProperty(CUBSTATUS);
	result[CUBGENDER] = getProperty(CUBGENDER);
	result[CUBUNIVYEAR] = getProperty(CUBUNIVYEAR);
	result[CUBDEPARTMENT] = getProperty(CUBDEPARTMENT);
	result[CUBDORM] = getProperty(CUBDORM);
	result[CUBBIRTHDAY] = getProperty(CUBBIRTHDAY);
	result[CUBBIRTHMONTH] = getProperty(CUBBIRTHMONTH);
	result[CUBBIRTHYEAR] = getProperty(CUBBIRTHYEAR);
	result[CUBHOMEPROVINCE] = getProperty(CUBHOMEPROVINCE);
	result[CUBHOMECITY] = getProperty(CUBHOMECITY);
	result[CUBCITYCODE] = getProperty(CUBCITYCODE);
	result[CUBHIGHSCHOOL] = getProperty(CUBHIGHSCHOOL);
	result[CUBHIGHSCHOOLCODE] = getProperty(CUBHIGHSCHOOLCODE);
	result[CUBASTROLOGY] = getProperty(CUBASTROLOGY);
	result[CUBHEADURL] = getProperty(CUBHEADURL);
	result[CUBTINYURL] = getProperty(CUBTINYURL);
	result[CUBBASICCONTROL] = getProperty(CUBBASICCONTROL);
	result[CUBSTAR] = getProperty(CUBSTAR);
	result[CUBDOING] = getProperty(CUBDOING);
	result[CUBDOINGTIME] = getProperty(CUBDOINGTIME);
	result[CUBSTAGE] = getProperty(CUBSTAGE);
//	result[CUBCURNETWORKID] = getProperty(CUBCURNETWORKID);
//	result[CUBCURNETWORKNAME] = getProperty(CUBCURNETWORKNAME);
//	result[CUBJOINYEAR] = getProperty(CUBJOINYEAR);
	result[CUBROOM] = getProperty(CUBROOM);    

    return result;
}

void 
UserBasicHandle::setProperties(const Str2StrMap& properties)
{
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        setProperty(it->first, it->second);
    }
}



DataHandlePtr 
UserBasicHandle::parse(mysqlpp::Row& row)
{
    DataHandlePtr d = new UserBasicHandle;
    UserBasicDataPtr u = new UserBasicData();
	u->id = (int)row[ CUBID.c_str()];
	u->univ = (int)row[ CUBUNIV.c_str()];
	u->selected = (int)row[ CUBSELECTED.c_str()];
#ifndef NEWARCH
	u->name = row[ CUBNAME.c_str()].get_string();
#else
	u->name = row[ CUBNAME.c_str()].data();
#endif
	u->status = (int)row[ CUBSTATUS.c_str()];
#ifndef NEWARCH
	u->gender = row[ CUBGENDER.c_str()].get_string();
#else
	u->gender = row[ CUBGENDER.c_str()].data();
#endif
	if(row[ CUBUNIVYEAR.c_str()].is_null()) u->univYear = 0;
	else u->univYear = (int)row[ CUBUNIVYEAR.c_str()];
#ifndef NEWARCH
	u->department = row[ CUBDEPARTMENT.c_str()].get_string();
	u->dorm = row[ CUBDORM.c_str()].get_string();
#else
	u->department = row[ CUBDEPARTMENT.c_str()].data();
	u->dorm = row[ CUBDORM.c_str()].data();
#endif
	
	if(row[ CUBBIRTHDAY.c_str()].is_null()) u->birthDay = 0;
	else u->birthDay = (int)row[ CUBBIRTHDAY.c_str()];
	
	if(row[ CUBBIRTHMONTH.c_str()].is_null()) u->birthMonth = 0;
	else u->birthMonth = (int)row[ CUBBIRTHMONTH.c_str()];
	
	if(row[ CUBBIRTHYEAR.c_str()].is_null()) u->birthYear = 0;
	else u->birthYear = (int)row[ CUBBIRTHYEAR.c_str()];
	
#ifndef NEWARCH
	u->homeProvince = row[ CUBHOMEPROVINCE.c_str()].get_string();
	u->homeCity = row[ CUBHOMECITY.c_str()].get_string();
	u->cityCode = row[ CUBCITYCODE.c_str()].get_string();
	u->highSchool = row[ CUBHIGHSCHOOL.c_str()].get_string();
	u->highSchoolCode = row[ CUBHIGHSCHOOLCODE.c_str()].get_string();
	u->astrology = row[ CUBASTROLOGY.c_str()].get_string();
	u->headUrl = row[ CUBHEADURL.c_str()].get_string();
	u->tinyUrl = row[ CUBTINYURL.c_str()].get_string();
#else
	u->homeProvince = row[ CUBHOMEPROVINCE.c_str()].data();
	u->homeCity = row[ CUBHOMECITY.c_str()].data();
	u->cityCode = row[ CUBCITYCODE.c_str()].data();
	u->highSchool = row[ CUBHIGHSCHOOL.c_str()].data();
	u->highSchoolCode = row[ CUBHIGHSCHOOLCODE.c_str()].data();
	u->astrology = row[ CUBASTROLOGY.c_str()].data();
	u->headUrl = row[ CUBHEADURL.c_str()].data();
	u->tinyUrl = row[ CUBTINYURL.c_str()].data();
#endif
	u->basicControl = (int)row[ CUBBASICCONTROL.c_str()];
	u->star = (int)row[ CUBSTAR.c_str()];
#ifndef NEWARCH
	u->doing = row[ CUBDOING.c_str()].get_string();
	u->doingTime = row[ CUBDOINGTIME.c_str()].get_string();
#else
	u->doing = row[ CUBDOING.c_str()].data();
	u->doingTime = row[ CUBDOINGTIME.c_str()].data();
#endif
	u->stage = (int)row[ CUBSTAGE.c_str()];
#ifndef NEWARCH
	u->room = row[ CUBROOM.c_str()].get_string();
#else
	u->room = row[ CUBROOM.c_str()].data();
#endif
    d->value(u);
    return d;
}

//---------------------------------------------------------------------------
string 
UserDescHandle::getProperty(const string& key)
{
    IceUtil::Mutex::Lock lock(*this);
    
    if(!strcasecmp(key.c_str(), CUDID.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->id);
	} else if(!strcasecmp(key.c_str(), CUDREGISTERTIME.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->registerTime);
	} else if(!strcasecmp(key.c_str(), CUDLASTLOGINTIME.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->lastLoginTime);
	} else if(!strcasecmp(key.c_str(), CUDLASTLOGOUTTIME.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->lastLogoutTime);
	} else if(!strcasecmp(key.c_str(), CUDMSN.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->msn);
	} else if(!strcasecmp(key.c_str(), CUDHOMEPAGE.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->homepage);
	} else if(!strcasecmp(key.c_str(), CUDMOBILEPHONE.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->mobilePhone);
	} else if(!strcasecmp(key.c_str(), CUDFIXPHONE.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->fixPhone);
	} else if(!strcasecmp(key.c_str(), CUDCONTACTCONTROL.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->contactControl);
	} else if(!strcasecmp(key.c_str(), CUDQQ.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->qq);
	} else if(!strcasecmp(key.c_str(), CUDMAINURL.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->mainUrl);
	} else if(!strcasecmp(key.c_str(), CUDDESCR.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->descr);
	} else if(!strcasecmp(key.c_str(), CUDDESCBAN.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->descBan);
	} else if(!strcasecmp(key.c_str(), CUDACTIVATETIME.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->activateTime);
	} else if(!strcasecmp(key.c_str(), CUDFAVFLAG.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->favFlag);
	} else if(!strcasecmp(key.c_str(), CUDFAV.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->fav);
	} else if(!strcasecmp(key.c_str(), CUDLASTEMAILNOTIFY.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->lastEmailNotify);
	//} else if(!strcasecmp(key.c_str(), CUDLASTEMAILNOTIFY1.c_str())) {
//		return boost::lexical_cast<string>(value<UserDescDataPtr>()->lastEmailNotify1);
	} else if(!strcasecmp(key.c_str(), CUDTEMPLATE.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->template1);
	} else if(!strcasecmp(key.c_str(), CUDLASTMODIFYTIME.c_str())) {
		return boost::lexical_cast<string>(value<UserDescDataPtr>()->lastModifyTime);
	} 
    NoSuchPropertyException e;
    e.message = key;
    throw e;
}

void
UserDescHandle::setProperty(const string& key, const string& val)
{
    IceUtil::Mutex::Lock lock(*this);
    if(!strcasecmp(key.c_str(), CUDID.c_str())) {
		value<UserDescDataPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if(!strcasecmp(key.c_str(), CUDREGISTERTIME.c_str())) {
		value<UserDescDataPtr>()->registerTime = val;
	} else if(!strcasecmp(key.c_str(), CUDLASTLOGINTIME.c_str())) {
		value<UserDescDataPtr>()->lastLoginTime = val;
	} else if(!strcasecmp(key.c_str(), CUDLASTLOGOUTTIME.c_str())) {
		value<UserDescDataPtr>()->lastLogoutTime = val;
	} else if(!strcasecmp(key.c_str(), CUDMSN.c_str())) {
		value<UserDescDataPtr>()->msn = val;
	} else if(!strcasecmp(key.c_str(), CUDHOMEPAGE.c_str())) {
		value<UserDescDataPtr>()->homepage = val;
	} else if(!strcasecmp(key.c_str(), CUDMOBILEPHONE.c_str())) {
		value<UserDescDataPtr>()->mobilePhone = val;
	} else if(!strcasecmp(key.c_str(), CUDFIXPHONE.c_str())) {
		value<UserDescDataPtr>()->fixPhone = val;
	} else if(!strcasecmp(key.c_str(), CUDCONTACTCONTROL.c_str())) {
		value<UserDescDataPtr>()->contactControl = boost::lexical_cast<Ice::Int>(val);
	} else if(!strcasecmp(key.c_str(), CUDQQ.c_str())) {
		value<UserDescDataPtr>()->qq = boost::lexical_cast<Ice::Int>(val);
	} else if(!strcasecmp(key.c_str(), CUDMAINURL.c_str())) {
		value<UserDescDataPtr>()->mainUrl = val;
	} else if(!strcasecmp(key.c_str(), CUDDESCR.c_str())) {
		value<UserDescDataPtr>()->descr = val;
	} else if(!strcasecmp(key.c_str(), CUDDESCBAN.c_str())) {
		value<UserDescDataPtr>()->descBan = boost::lexical_cast<Ice::Int>(val);
	} else if(!strcasecmp(key.c_str(), CUDACTIVATETIME.c_str())) {
		value<UserDescDataPtr>()->activateTime = val;
	} else if(!strcasecmp(key.c_str(), CUDFAVFLAG.c_str())) {
		value<UserDescDataPtr>()->favFlag = boost::lexical_cast<Ice::Int>(val);
	} else if(!strcasecmp(key.c_str(), CUDFAV.c_str())) {
		value<UserDescDataPtr>()->fav = val;
	} else if(!strcasecmp(key.c_str(), CUDLASTEMAILNOTIFY.c_str())) {
		value<UserDescDataPtr>()->lastEmailNotify = val;
//	} else if(!strcasecmp(key.c_str(), CUDLASTEMAILNOTIFY1.c_str())) {
//		value<UserDescDataPtr>()->lastEmailNotify1 = val;
	} else if(!strcasecmp(key.c_str(), CUDTEMPLATE.c_str())) {
		value<UserDescDataPtr>()->template1 = boost::lexical_cast<Ice::Int>(val);
	} else if(!strcasecmp(key.c_str(), CUDLASTMODIFYTIME.c_str())) {
		value<UserDescDataPtr>()->lastModifyTime = val;
	} 
}

Str2StrMap 
UserDescHandle::getProperties()
{
    Str2StrMap result;
	result[CUDID]					= getProperty(CUDID);              
	result[CUDREGISTERTIME]			= getProperty(CUDREGISTERTIME);    
	result[CUDLASTLOGINTIME]		= getProperty(CUDLASTLOGINTIME);   
	result[CUDLASTLOGOUTTIME]		= getProperty(CUDLASTLOGOUTTIME);  
	result[CUDMSN]					= getProperty(CUDMSN);             
	result[CUDHOMEPAGE]				= getProperty(CUDHOMEPAGE);        
	result[CUDMOBILEPHONE]			= getProperty(CUDMOBILEPHONE);     
	result[CUDFIXPHONE]				= getProperty(CUDFIXPHONE);        
	result[CUDCONTACTCONTROL]		= getProperty(CUDCONTACTCONTROL);  
	result[CUDQQ]					= getProperty(CUDQQ);              
	result[CUDMAINURL]				= getProperty(CUDMAINURL);         
	result[CUDDESCR]				= getProperty(CUDDESCR);           
	result[CUDDESCBAN]				= getProperty(CUDDESCBAN);         
	result[CUDACTIVATETIME]			= getProperty(CUDACTIVATETIME);    
	result[CUDFAVFLAG]				= getProperty(CUDFAVFLAG);         
	result[CUDFAV]					= getProperty(CUDFAV);             
	result[CUDLASTEMAILNOTIFY]		= getProperty(CUDLASTEMAILNOTIFY); 
	//result[CUDLASTEMAILNOTIFY1]		= getProperty(CUDLASTEMAILNOTIFY1);
	result[CUDTEMPLATE]				= getProperty(CUDTEMPLATE);        
	result[CUDLASTMODIFYTIME]		= getProperty(CUDLASTMODIFYTIME);  

    return result;
}

void 
UserDescHandle::setProperties(const Str2StrMap& properties)
{
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        setProperty(it->first, it->second);
    }
}


DataHandlePtr 
UserDescHandle::parse(mysqlpp::Row& row)
{
    DataHandlePtr d = new UserDescHandle;
    UserDescDataPtr ud = new UserDescData();
    ud->id 					= (int)row[ CUDID.c_str()];              		
#ifndef NEWARCH
	ud->registerTime 		= row[ CUDREGISTERTIME.c_str()].get_string();    		
	ud->lastLoginTime 		= row[ CUDLASTLOGINTIME.c_str()].get_string();   		
	ud->lastLogoutTime 		= row[ CUDLASTLOGOUTTIME.c_str()].get_string();  		
	ud->msn 				= row[ CUDMSN.c_str()].get_string();             		
	ud->homepage 			= row[ CUDHOMEPAGE.c_str()].get_string();        		
	ud->mobilePhone 		= row[ CUDMOBILEPHONE.c_str()].get_string();     		
	ud->fixPhone 			= row[ CUDFIXPHONE.c_str()].get_string();        		
#else
	ud->registerTime 		= row[ CUDREGISTERTIME.c_str()].data();    		
	ud->lastLoginTime 		= row[ CUDLASTLOGINTIME.c_str()].data();   		
	ud->lastLogoutTime 		= row[ CUDLASTLOGOUTTIME.c_str()].data();  		
	ud->msn 				= row[ CUDMSN.c_str()].data();             		
	ud->homepage 			= row[ CUDHOMEPAGE.c_str()].data();        		
	ud->mobilePhone 		= row[ CUDMOBILEPHONE.c_str()].data();     		
	ud->fixPhone 			= row[ CUDFIXPHONE.c_str()].data();        		
#endif
	ud->contactControl 		= (int)row[ CUDCONTACTCONTROL.c_str()];  		
	if(row[ CUDQQ.c_str()].is_null()) ud->qq = 0;
	else ud->qq 			= (int)row[ CUDQQ.c_str()];              		
#ifndef NEWARCH
	ud->mainUrl 			= row[ CUDMAINURL.c_str()].get_string();         		
	ud->descr 				= row[ CUDDESCR.c_str()].get_string();           		
#else
	ud->mainUrl 			= row[ CUDMAINURL.c_str()].data();         		
	ud->descr 				= row[ CUDDESCR.c_str()].data();           		
#endif
	ud->descBan 			= (int)row[ CUDDESCBAN.c_str()];         		
#ifndef NEWARCH
	ud->activateTime 		= row[ CUDACTIVATETIME.c_str()].get_string();    		
#else
	ud->activateTime 		= row[ CUDACTIVATETIME.c_str()].data();    		
#endif
	if(row[ CUDFAVFLAG.c_str()].is_null()) ud->favFlag = 3;
	else ud->favFlag 		= (int)row[ CUDFAVFLAG.c_str()];         		
#ifndef NEWARCH
	ud->fav 				= row[ CUDFAV.c_str()].get_string();             		
	ud->lastEmailNotify 	= row[ CUDLASTEMAILNOTIFY.c_str()].get_string(); 		
#else
	ud->fav 				= row[ CUDFAV.c_str()].data();             		
	ud->lastEmailNotify 	= row[ CUDLASTEMAILNOTIFY.c_str()].data(); 		
#endif
	ud->template1 			= (int)row[ CUDTEMPLATE.c_str()];        		
#ifndef NEWARCH
	ud->lastModifyTime 		= row[ CUDLASTMODIFYTIME.c_str()].get_string();  	
#else
	ud->lastModifyTime 		= row[ CUDLASTMODIFYTIME.c_str()].data();  	
#endif

    d->value(ud);
    return d;
}

//---------------------------------------------------------------------------
string 
ExtraInfoHandle::getProperty(const string& key)
{
    IceUtil::Mutex::Lock lock(*this);
    
    if (!strcasecmp(key.c_str(), CEIUSERID.c_str())) {
        return boost::lexical_cast<string>(value<ExtraInfoPtr>()->userId);
    } else if (!strcasecmp(key.c_str(), CEIELEMENTARYSCHOOLID.c_str())) {
        return boost::lexical_cast<string>(value<ExtraInfoPtr>()->elementarySchoolId);
    } else if (!strcasecmp(key.c_str(), CEIELEMENTARYSCHOOLNAME.c_str())) {
        return value<ExtraInfoPtr>()->elementarySchoolName;
    } else if (!strcasecmp(key.c_str(), CEIELEMENTARYSCHOOLYEAR.c_str())) {
        return boost::lexical_cast<string>(value<ExtraInfoPtr>()->elementarySchoolYear);
    } else if (!strcasecmp(key.c_str(), CEIJUNIORHIGHSCHOOLID.c_str())){
    	return boost::lexical_cast<string>(value<ExtraInfoPtr>()->juniorHighSchoolId);
    } else if (!strcasecmp(key.c_str(), CEIJUNIORHIGHSCHOOLNAME.c_str())) {
	return value<ExtraInfoPtr>()->juniorHighSchoolName;
    } else if (!strcasecmp(key.c_str(), CEIJUNIORHIGHSCHOOLYEAR.c_str())) {
        return boost::lexical_cast<string>(value<ExtraInfoPtr>()->juniorHighSchoolYear);
    }
    NoSuchPropertyException e;
    e.message = key;
    throw e;
}

void
ExtraInfoHandle::setProperty(const string& key, const string& val)
{
    IceUtil::Mutex::Lock lock(*this);
    if (!strcasecmp(key.c_str(), CEIUSERID.c_str())) {
        value<ExtraInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CEIELEMENTARYSCHOOLID.c_str())) {
        value<ExtraInfoPtr>()->elementarySchoolId = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CEIELEMENTARYSCHOOLYEAR.c_str())) {
        value<ExtraInfoPtr>()->elementarySchoolYear = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CEIELEMENTARYSCHOOLNAME.c_str())) {
        value<ExtraInfoPtr>()->elementarySchoolName = val;
    } else if (!strcasecmp(key.c_str(), CEIJUNIORHIGHSCHOOLID.c_str())) {
        value<ExtraInfoPtr>()->juniorHighSchoolId = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CEIJUNIORHIGHSCHOOLYEAR.c_str())) {
        value<ExtraInfoPtr>()->juniorHighSchoolYear = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CEIJUNIORHIGHSCHOOLNAME.c_str())) {
        value<ExtraInfoPtr>()->juniorHighSchoolName = val;
    } 
}


Str2StrMap 
ExtraInfoHandle::getProperties()
{
    Str2StrMap result;
    result[CEIUSERID] = getProperty(CEIUSERID);
    result[CEIELEMENTARYSCHOOLID] = getProperty(CEIELEMENTARYSCHOOLID);
    result[CEIELEMENTARYSCHOOLNAME] = getProperty(CEIELEMENTARYSCHOOLNAME);
    result[CEIELEMENTARYSCHOOLYEAR] = getProperty(CEIELEMENTARYSCHOOLYEAR);
    result[CEIJUNIORHIGHSCHOOLID] = getProperty(CEIJUNIORHIGHSCHOOLID);
    result[CEIJUNIORHIGHSCHOOLNAME] = getProperty(CEIJUNIORHIGHSCHOOLNAME);
    result[CEIJUNIORHIGHSCHOOLYEAR] = getProperty(CEIJUNIORHIGHSCHOOLYEAR);
    

    return result;
}

void 
ExtraInfoHandle::setProperties(const Str2StrMap& properties)
{
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        setProperty(it->first, it->second);
    }
}


DataHandlePtr
ExtraInfoHandle::parse(mysqlpp::Row& row)
{
    DataHandlePtr d = new ExtraInfoHandle;
    ExtraInfoPtr ei = new ExtraInfo();
    ei->userId = (Ice::Int)(int)row[ CEIUSERID.c_str()];
    ei->elementarySchoolId = (Ice::Int)(int)row[ CEIELEMENTARYSCHOOLID.c_str()];
    ei->elementarySchoolYear = (Ice::Int)(int)row[ CEIELEMENTARYSCHOOLYEAR.c_str()];
#ifndef NEWARCH
    ei->elementarySchoolName = row[ CEIELEMENTARYSCHOOLNAME.c_str()].get_string();
#else
    ei->elementarySchoolName = row[ CEIELEMENTARYSCHOOLNAME.c_str()].data();
#endif
    ei->juniorHighSchoolId = (Ice::Int)(int)row[ CEIJUNIORHIGHSCHOOLID.c_str()];
    ei->juniorHighSchoolYear = (Ice::Int)(int)row[ CEIJUNIORHIGHSCHOOLYEAR.c_str()];
#ifndef NEWARCH
    ei->juniorHighSchoolName = row[ CEIJUNIORHIGHSCHOOLNAME.c_str()].get_string();
#else
    ei->juniorHighSchoolName = row[ CEIJUNIORHIGHSCHOOLNAME.c_str()].data();
#endif


    d->value(ei);
    return d;
}






//---------------------------------------------------------------------------
string 
ValueAddedDataHandle::getProperty(const string& key)
{
    IceUtil::Mutex::Lock lock(*this);
    
    if (!strcasecmp(key.c_str(), CVAUSERID.c_str())) {
        return boost::lexical_cast<string>(value<ValueAddedDataPtr>()->userId);
    } else if (!strcasecmp(key.c_str(), CVANICKNAME.c_str())) {
        return value<ValueAddedDataPtr>()->nickname;
    } 
    
    NoSuchPropertyException e;
    e.message = key;
    throw e;
}

void
ValueAddedDataHandle::setProperty(const string& key, const string& val)
{
    IceUtil::Mutex::Lock lock(*this);
    if (!strcasecmp(key.c_str(), CVAUSERID.c_str())) {
        value<ValueAddedDataPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
    } else if (!strcasecmp(key.c_str(), CVANICKNAME.c_str())) {
        value<ValueAddedDataPtr>()->nickname = val;
    } 
    
}


Str2StrMap 
ValueAddedDataHandle::getProperties()
{
    Str2StrMap result;
    result[CVAUSERID] = getProperty(CVAUSERID);
    result[CVANICKNAME] = getProperty(CVANICKNAME);

    return result;
}

void 
ValueAddedDataHandle::setProperties(const Str2StrMap& properties)
{
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        setProperty(it->first, it->second);
    }
}


DataHandlePtr
ValueAddedDataHandle::parse(mysqlpp::Row& row)
{
    DataHandlePtr d = new ValueAddedDataHandle;
    ValueAddedDataPtr ei = new ValueAddedData();
    ei->userId = (Ice::Int)row[ CVAUSERID.c_str()];
#ifndef NEWARCH
    ei->nickname = row[ CVANICKNAME.c_str()].get_string();
#else
    ei->nickname = row[ CVANICKNAME.c_str()].data();
#endif
    
    d->value(ei);
    return d;
}

