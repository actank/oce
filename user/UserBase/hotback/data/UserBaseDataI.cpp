#include "UserBaseDataI.h"
#include "LogWrapper.h"
#include "boost/lexical_cast.hpp"

using namespace MyUtil;
using namespace boost;
using namespace xce::userbase;
//---------------------------------------------------------------------------
void UserPassportDataI::setProperty(const string& strKey, const string& iVal)
{
    if (!strcasecmp(strKey.c_str(), CUPID.c_str()))
	{
        uID = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUPSTATUS.c_str()))
	{
        status = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUPSAFESTATUS.c_str()))
	{
        safeStatus = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUPACCOUNT.c_str()))
	{
        account = iVal;
    } else if (!strcasecmp(strKey.c_str(), CUPDOMAIN.c_str()))
	{
        domain = iVal;
    } else if (!strcasecmp(strKey.c_str(), CUPPASSWORDMD5.c_str()))
	{
    	passwordmd5 = iVal; 
    }
}


void UserPassportDataI::setProperties(const Str2StrMap& properties)
{
    IceUtil::Mutex::Lock lock(m_mutex);
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it)
	{
        setProperty(it->first, it->second);
    }
}

void UserPassportDataI::parse(Ice::ObjectPtr& obj)
{
	UserPassportDataPtr data = UserPassportDataPtr::dynamicCast(obj);
    uID       = data->uID;
	status    = data->status;
	safeStatus= data->safeStatus;
	account   = data->account;
	domain    = data->domain;
	passwordmd5 = data->passwordmd5 ;

}


void UserPassportDataI::parse(mysqlpp::Row& row )
{
    uID       = (int)(row[CUPID.c_str()]);
    
	status       = (int)(row[CUPSTATUS.c_str()]);

	safeStatus       = (int)(row[CUPSAFESTATUS.c_str()]);
 
#ifndef NEWARCH 
	account     = row[CUPACCOUNT.c_str()].get_string();
   
	domain      = row[CUPDOMAIN.c_str()].get_string();
   
	passwordmd5 = row[CUPPASSWORDMD5._str()].get_string();
#else
	account     = row[CUPACCOUNT.c_str()].data();
   
	domain      = row[CUPDOMAIN.c_str()].data();
   
	passwordmd5 = row[CUPPASSWORDMD5.c_str()].data();
#endif

}

UserPassportDataIPtr UserPassportDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	UserPassportDataIPtr res = new UserPassportDataI();
	res->uID         = uID;
	res->status      = status;
	res->safeStatus  = safeStatus;
	res->account     = account;
	res->domain      = domain;
	res->passwordmd5 = passwordmd5;
	return res;
}

//--------------------------------------------------------------------------------------

Ice::ObjectPtr UserPassportFactoryI::create(Ice::Int id)
{
	MCE_INFO("UserPassportFactoryI::create " << id);
	
	UserPassportDataIPtr rs = new UserPassportDataI ;
	BatchPassportResHandlerI handler(rs);
	Statement sql;
	sql << "SELECT id, account, domain, password_md5, status,safe_status FROM user_passport WHERE id = '"<<id<<"'";
	size_t resultSize = QueryRunner(DB_USER_PASSPORT, CDbRServer ).query(sql,handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserPassportFactoryI::create NoSuchUserPassportException [" << id << "]");
		return NULL;
	}
	
	if (rs->uID != id) {
		MCE_WARN("UserPassportFactoryI::create UserPassportIdErrorException id=" << id << ", resultID="<<rs->uID);
		return NULL;
	}

	MCE_INFO( "UserPassportFactoryI::create " << id << " done.");
	return rs;
}
//-------------------------------BatchPassportResHandlerI-----------------------
bool BatchPassportResHandlerI::handle(mysqlpp::Row& row) const
{
	m_rs->parse(row);
	if(m_rs == 0)
	{
		MCE_WARN( "BatchPassportResHandlerI::parse ParseFailedException! ");
		return false;
	}
	return true;
}

//-------------------------------BatchEmailResHandlerI-----------------------
UserEmailResultHandlerI::UserEmailResultHandlerI(Ice::Int& rs) :
	m_rs(rs) {
}

bool UserEmailResultHandlerI::handle(mysqlpp::Row& row) const {
	m_rs = (int) row["id"];
	return true;
}

//---------------------------------------------------------------------------

void UserNameDataI::setProperty(const string& strKey, const string& iVal)
{
    if (!strcasecmp(strKey.c_str(), CUNID.c_str()))
	{
        uID = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUNNAME.c_str()))
	{
    	name = iVal; 
    } else if (!strcasecmp(strKey.c_str(), CUNNICKNAME.c_str()))
	{
    	nickname = iVal; 
    }
}


void UserNameDataI::setProperties(const Str2StrMap& properties)
{
    IceUtil::Mutex::Lock lock(m_mutex);
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it)
	{
        setProperty(it->first, it->second);
    }
}

void UserNameDataI::parse(Ice::ObjectPtr& obj)
{
	UserNameDataPtr data = UserNameDataPtr::dynamicCast(obj);
    uID  = data->uID  ;
	name = data->name ;
	nickname = data->nickname ;

}


void UserNameDataI::parse(mysqlpp::Row& row )
{
    uID  = (int)(row[CUNID.c_str()]);
    
#ifndef NEWARCH 
	name = row[CUNNAME.c_str()].get_string();
	nickname = row[CUNNICKNAME.c_str()].get_string();
#else
	name = row[CUNNAME.c_str()].data();
	nickname = row[CUNNICKNAME.c_str()].data();
#endif

}

UserNameDataIPtr UserNameDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	UserNameDataIPtr res = new UserNameDataI();
	res->uID = uID;
	res->name = name;
	res->nickname = nickname;
	return res;
}

//--------------------------------------------------------------------------------------

Ice::ObjectPtr UserNameFactoryI::create(Ice::Int id)
{
	MCE_INFO("UserNameFactoryI::create " << id);
	
	UserNameDataIPtr rs = new UserNameDataI ;
	BatchNameResHandlerI handler(rs);
	Statement sql;
	sql << "SELECT id, name, nickname FROM user_names WHERE id = '"<<id<<"'";
	size_t resultSize = QueryRunner(DB_USER_NAME, CDbRServer ).query(sql, handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserNameFactoryI::create NoSuchUserNameException [" << id << "]");
		return NULL;
	}
	
	if (rs->uID != id) {
		MCE_WARN("UserNameFactoryI::create UserNameIdErrorException id=" << id << ", resultID="<<rs->uID);
		return NULL;
	}

	MCE_INFO( "UserNameFactoryI::create " << id << " done.");
	return rs;
}



//-------------------------------BatchNameResHandlerI-----------------------
bool BatchNameResHandlerI::handle(mysqlpp::Row& row) const
{
	m_rs->parse(row);
	if(m_rs == 0)
	{
		MCE_WARN( "BatchNameResHandlerI::parse ParseFailedException! ");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------


void UserStateDataI::setProperty(const string& strKey, const string& iVal)
{
    if (!strcasecmp(strKey.c_str(), CUSID.c_str())){
        id = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUSLEVEL.c_str())){
        level = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUSSTAR.c_str())){
        star = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CUSSTATE.c_str())){
	state = boost::lexical_cast<Ice::Int>(iVal);
    }
}


void UserStateDataI::setProperties(const Str2StrMap& properties)
{
    IceUtil::Mutex::Lock lock(m_mutex);
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it)
	{
        setProperty(it->first, it->second);
    }
}

void UserStateDataI::parse(Ice::ObjectPtr& obj)
{
	UserStateDataPtr data = UserStateDataPtr::dynamicCast(obj);
    id    = data->id    ;
    level   = data->level   ;
    star = data->star ;
    state = data->state ;
}


void UserStateDataI::parse(mysqlpp::Row& row )
{
    id    = (int)(row[CUSID.c_str()]);
    
    level   = (int)(row[CUSLEVEL.c_str()]);

    star = (int)(row[CUSSTAR.c_str()]);

    state = (int)(row[CUSSTATE.c_str()]);
}

UserStateDataIPtr UserStateDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	UserStateDataIPtr res = new UserStateDataI();
	res->id = id;
	res->star= star; 
	res->state = state;
	res->level = level;
	return res;
}

//--------------------------------------------------------------------------------------

Ice::ObjectPtr UserStateFactoryI::create(Ice::Int id)
{
	MCE_INFO("UserStateFactoryI::create " << id);
	
	UserStateDataIPtr rs = new UserStateDataI ;
	BatchStateResHandlerI handler(rs);
	Statement sql;
	sql << "SELECT id, state, star, level FROM user_state WHERE id = '"<<id<<"'";
	size_t resultSize = QueryRunner(DB_USER_STATE, CDbRServer ).query(sql,handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserStateFactoryI::create NoSuchUserStateException [" << id << "]");
		return NULL;
	}
	if (rs->id != id) {
		MCE_WARN("UserStateFactoryI::create UserStateIdErrorException id=" << id << ", resultID="<<rs->id);
		return NULL;
	}
	
	MCE_INFO( "UserStateFactoryI::create " << id << " done.");
	return rs;
}
//-------------------------------BatchStateResHandlerI-----------------------
bool BatchStateResHandlerI::handle(mysqlpp::Row& row) const
{
	m_rs->parse(row);
	if(m_rs == 0)
	{
		MCE_WARN( "BatchStateResHandlerI::parse ParseFailedException! ");
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------


void UserRightDataI::setProperty(const string& strKey, const string& iVal)
{
    if (!strcasecmp(strKey.c_str(), CURID.c_str()))
	{
        uID = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CURAUTH.c_str()))
	{
        auth = boost::lexical_cast<Ice::Int>(iVal);
	} else if (!strcasecmp(strKey.c_str(), CURCOMMONRIGHTS.c_str()))
	{
        rights = boost::lexical_cast<Ice::Int>(iVal);
    } 
}


void UserRightDataI::setProperties(const Str2StrMap& properties)
{
    IceUtil::Mutex::Lock lock(m_mutex);
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it)
	{
        setProperty(it->first, it->second);
    }
}

void UserRightDataI::parse(Ice::ObjectPtr& obj)
{
	UserRightDataPtr data = UserRightDataPtr::dynamicCast(obj);
    uID    = data->uID    ;
    auth   = data->auth   ;
    rights = data->rights ;
}

void UserRightDataI::parse(mysqlpp::Row& row )
{
    uID    = (int)(row[CURID.c_str()]);
    
    auth   = (int)(row[CURAUTH.c_str()]);

    rights = (int)(row[CURCOMMONRIGHTS.c_str()]);
}

UserRightDataIPtr UserRightDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	UserRightDataIPtr res = new UserRightDataI();
	res->uID = uID;
	res->auth = auth; 
	res->rights = rights;
	return res;
}

//--------------------------------------------------------------------------------------

Ice::ObjectPtr UserRightFactoryI::create(Ice::Int id)
{
	MCE_INFO("UserRightFactoryI::create " << id);
	
	UserRightDataIPtr rs = new UserRightDataI ;
	BatchRightResHandlerI handler(rs);
	Statement sql;
	sql << "SELECT id, auth, common_rights FROM user_right WHERE id = '"<<id<<"'";
	size_t resultSize = QueryRunner(DB_USER_RIGHT, CDbRServer ).query(sql,handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserRightFactoryI::create NoSuchUserRightException [" << id << "]");
		return NULL;
	}
	if (rs->uID != id) {
		MCE_WARN("UserRightFactoryI::create UserRightIdErrorException id=" << id << ", resultID="<<rs->uID);
		return NULL;
	}

	MCE_INFO( "UserRightFactoryI::create " << id << " done.");
	return rs;
}
//-------------------------------BatchRightResHandlerI-----------------------
bool BatchRightResHandlerI::handle(mysqlpp::Row& row) const
{
	m_rs->parse(row);
	if(m_rs == 0)
	{
		MCE_WARN( "BatchRightResHandlerI::parse ParseFailedException! ");
		return false;
	}
	return true;
}



//---------------------------------------------------------------------------

int UserLoginCountDataI::getLogincount()
{
    IceUtil::Mutex::Lock lock(m_mutex);
	return logincount;
    
}

void UserLoginCountDataI::setProperty(const string& strKey, const string& iVal)
{
    if (!strcasecmp(strKey.c_str(), CULCID.c_str()))
	{
        uID = boost::lexical_cast<Ice::Int>(iVal);
    } else if (!strcasecmp(strKey.c_str(), CULCLOGINCOUNT.c_str()))
	{
        logincount = boost::lexical_cast<Ice::Int>(iVal);
	} 
}


void UserLoginCountDataI::setProperties(const Str2StrMap& properties)
{
    IceUtil::Mutex::Lock lock(m_mutex);
    Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it)
	{
        setProperty(it->first, it->second);
    }
}

void UserLoginCountDataI::parse(Ice::ObjectPtr& obj)
{
	UserLoginCountDataPtr data = UserLoginCountDataPtr::dynamicCast(obj);
    uID        = data->uID       ; 

    logincount = data->logincount; 
}


void UserLoginCountDataI::parse(mysqlpp::Row& row )
{
    uID        = (int)(row[CULCID.c_str()]);

    logincount = (int)(row[CULCLOGINCOUNT.c_str()]);
}

void UserLoginCountDataI::incLogincount()
{
    IceUtil::Mutex::Lock lock(m_mutex);
	++logincount;
}

UserLoginCountDataIPtr UserLoginCountDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	UserLoginCountDataIPtr res = new UserLoginCountDataI();
	res->uID = uID;
	res->logincount = logincount;
	return res;
}
//--------------------------------------------------------------------------------------

Ice::ObjectPtr UserLoginCountFactoryI::create(Ice::Int id)
{
	MCE_INFO("UserLoginCountFactoryI::create " << id);
	
	UserLoginCountDataIPtr rs = new UserLoginCountDataI ;
	BatchLoginCountResHandlerI handler(rs);
	Statement sql;
	sql << "SELECT id, logincount FROM user_logincount WHERE id = '" << id<<"'";
	size_t resultSize = QueryRunner(DB_USER_LOGINCOUNT, CDbRServer ).query(sql, handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserLoginCountFactoryI::create NoSuchUserLoginCountException [" << id << "]");
		rs->uID = id;
		rs->logincount = 0;
	}
	if (rs->uID != id) {
		MCE_WARN("UserLoginCountFactoryI::create UserLoginCountIdErrorException id=" << id << ", resultID="<<rs->uID);
		rs->uID = id;
		rs->logincount = 0;
	}

	MCE_INFO( "UserLoginCountFactoryI::create " << id << " done.");
	return rs;
}
//-------------------------------BatchLoginCountResHandlerI-----------------------
bool BatchLoginCountResHandlerI::handle(mysqlpp::Row& row) const
{
	m_rs->parse(row);
	if(m_rs == 0)
	{
		MCE_WARN( "BatchLoginCountResHandlerI::parse ParseFailedException! ");
		return false;
	}
	return true;
}


//---------------------------------------------------------------------------

void UserUrlDataI::setProperities(const MyUtil::Str2StrMap& properities) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	for (MyUtil::Str2StrMap::const_iterator it = properities.begin(); it != properities.end(); ++it) {
		if((*it).first == "LARGEURL") {
			largeUrl = (*it).second;
		}else if ((*it).first == "MAINURL") {
			mainUrl = (*it).second;
		}else if ((*it).first == "HEADURL") {
			headUrl = (*it).second;
		}else if((*it).first == "TINYURL"){
			tinyUrl = (*it).second;
		}
	}
}

void UserUrlDataI::parse(Ice::ObjectPtr& obj)
{
	UserUrlDataPtr data = UserUrlDataPtr::dynamicCast(obj);
	id = data->id ;
	largeUrl = data->largeUrl ;
	mainUrl = data->mainUrl ;
	headUrl = data->headUrl ;
	tinyUrl = data->tinyUrl ;

}

void UserUrlDataI::parse(mysqlpp::Row& row )
{
	id = (int) row["id"];
	largeUrl = (string) row["largeurl"];
	mainUrl = (string) row["mainurl"];
	headUrl = (string) row["headurl"];
	tinyUrl = (string) row["tinyurl"];

}

UserUrlDataIPtr UserUrlDataI::getData() {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	UserUrlDataIPtr newData = new UserUrlDataI;
	newData->id = id;
	newData->largeUrl = largeUrl;
	newData->mainUrl = mainUrl;
	newData->headUrl = headUrl;
	newData->tinyUrl = tinyUrl;
	return newData;
}
//-------------------------------UserUrlFactoryI---------------------
Ice::ObjectPtr UserUrlFactoryI::create(Ice::Int id) {
	MCE_INFO("UserUrlFactoryI::create " << id);
	UserUrlDataIPtr result = new UserUrlDataI;
	Statement sql;
	sql << "SELECT id, largeurl, mainurl, headurl, tinyurl FROM user_url WHERE id = '" << id << "'";
	UserUrlResultHandlerI handler(result);
	size_t resultSize = QueryRunner(DB_USER_URL, CDbRServer).query(sql, handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserUrlFactoryI::create NoSuchUserUrlException [" << id << "]");
		return NULL;
	}
	
	if (result->id!= id) {
		MCE_WARN("UserUrlFactoryI::create UserUrlIdErrorException id=" << id << ", resultID="<<result->id);
		return NULL;
	}
	
	MCE_INFO("UserUrlFactoryI::create " << id << " done.");
	return result;
}
//---------------------------------------------------------------------------
UserUrlResultHandlerI::UserUrlResultHandlerI(const UserUrlDataIPtr& result) :
	_result(result) {
}
bool UserUrlResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->id = (int) row["id"];
	_result->largeUrl = (string) row["largeurl"];
	_result->mainUrl = (string) row["mainurl"];
	_result->headUrl = (string) row["headurl"];
	_result->tinyUrl = (string) row["tinyurl"];
	return true;
}
//---------------------------------------------------------------------------

void UserBasicDataI::setProperty(const string& key, const string& val)
{
	if (!strcasecmp(key.c_str(), CUBID.c_str())) {
		id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBUNIV.c_str())) {
		univ = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBNAME.c_str())) {
		name = val;
	} else if (!strcasecmp(key.c_str(), CUBSTATUS.c_str())) {
		status = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBGENDER.c_str())) {
		gender = val;
	} else if (!strcasecmp(key.c_str(), CUBBIRTHDAY.c_str())) {
		birthDay = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBBIRTHMONTH.c_str())) {
		birthMonth = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBBIRTHYEAR.c_str())) {
		birthYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBISLUNARBIRTH.c_str())) {
		isLunarBirth = boost::lexical_cast<bool>(val);
	} else if (!strcasecmp(key.c_str(), CUBHOMEPROVINCE.c_str())) {
		homeProvince = val;
	} else if (!strcasecmp(key.c_str(), CUBHOMECITY.c_str())) {
		homeCity = val;
	} else if (!strcasecmp(key.c_str(), CUBCITYCODE.c_str())) {
		cityCode = val;
	} else if (!strcasecmp(key.c_str(), CUBASTROLOGY.c_str())) {
		astrology = val;
	} else if (!strcasecmp(key.c_str(), CUBHEADURL.c_str())) {
		headUrl = val;
	} else if (!strcasecmp(key.c_str(), CUBTINYURL.c_str())) {
		tinyUrl = val;
	} else if (!strcasecmp(key.c_str(), CUBSTAR.c_str())) {
		star = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBDOING.c_str())) {
		doing = val;
	} else if (!strcasecmp(key.c_str(), CUBDOINGTIME.c_str())) {
		doingTime = (int)mysqlpp::DateTime(val);
	} else if (!strcasecmp(key.c_str(), CUBSTAGE.c_str())) {
		stage = boost::lexical_cast<Ice::Int>(val);
	} 
}


UserBasicDataNPtr UserBasicDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	
	UserBasicDataNPtr result = new UserBasicDataN;
	
	result->id= id;
	result->univ= univ; 
	result->name= name;
	result->status= status;
	result->gender= gender;
	result->birthDay= birthDay;
	result->birthMonth= birthMonth;
	result->birthYear= birthYear;
	result->isLunarBirth = isLunarBirth;
	result->homeProvince= homeProvince;
	result->homeCity= homeCity;
	result->cityCode= cityCode;
	result->astrology= astrology;
	result->headUrl= headUrl;
	result->tinyUrl= tinyUrl;
	result->star= star;
	result->doing= doing;
	result->doingTime= doingTime;
	result->stage= stage;
	return result;
}

void UserBasicDataI::setProperties(const Str2StrMap& props)
{
	Str2StrMap::const_iterator it;
    IceUtil::Mutex::Lock lock(m_mutex);
	for (it = props.begin(); it != props.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

void UserBasicDataI::parse(Ice::ObjectPtr& obj)
{
	UserBasicDataNPtr data = UserBasicDataNPtr::dynamicCast(obj);
	id = data->id ;
	univ = data->univ ;
	name = data->name ;
	status = data->status ;
	gender = data->gender ;
	birthDay = data->birthDay ;
	birthMonth = data->birthMonth ;
	birthYear = data->birthYear ;
	isLunarBirth = data->isLunarBirth;
	homeProvince = data->homeProvince ;
	homeCity = data->homeCity ;
	cityCode = data->cityCode ;
	astrology = data->astrology ;
	headUrl = data->headUrl ;
	tinyUrl = data->tinyUrl ;
	star = data->star ;
	doing = data->doing ;
	doingTime = data->doingTime ;
	stage = data->stage ;
}
void UserBasicDataI::parse(mysqlpp::Row& row)
{
	id = (int)row[ CUBID.c_str()];
	univ = (int)row[ CUBUNIV.c_str()];
#ifndef NEWARCH
	name = row[ CUBNAME.c_str()].get_string();
#else
	name = row[ CUBNAME.c_str()].data();
#endif
	status = (int)row[ CUBSTATUS.c_str()];
#ifndef NEWARCH
	gender = row[ CUBGENDER.c_str()].get_string();
#else
	gender = row[ CUBGENDER.c_str()].data();
#endif
	
	if(row[ CUBBIRTHDAY.c_str()].is_null()) birthDay = 0;
	else birthDay = (int)row[ CUBBIRTHDAY.c_str()];
	
	if(row[ CUBBIRTHMONTH.c_str()].is_null()) birthMonth = 0;
	else birthMonth = (int)row[ CUBBIRTHMONTH.c_str()];
	
	if(row[ CUBBIRTHYEAR.c_str()].is_null()) birthYear = 0;
	else birthYear = (int)row[ CUBBIRTHYEAR.c_str()];
	
	if(row[ CUBISLUNARBIRTH.c_str()].is_null()) isLunarBirth = false;
	else isLunarBirth = (bool)row[ CUBISLUNARBIRTH.c_str()];
	
#ifndef NEWARCH
	homeProvince = row[ CUBHOMEPROVINCE.c_str()].get_string();
	homeCity = row[ CUBHOMECITY.c_str()].get_string();
	cityCode = row[ CUBCITYCODE.c_str()].get_string();
	astrology = row[ CUBASTROLOGY.c_str()].get_string();
	headUrl = row[ CUBHEADURL.c_str()].get_string();
	tinyUrl = row[ CUBTINYURL.c_str()].get_string();
#else
	homeProvince = row[ CUBHOMEPROVINCE.c_str()].data();
	homeCity = row[ CUBHOMECITY.c_str()].data();
	cityCode = row[ CUBCITYCODE.c_str()].data();
	astrology = row[ CUBASTROLOGY.c_str()].data();
	headUrl = row[ CUBHEADURL.c_str()].data();
	tinyUrl = row[ CUBTINYURL.c_str()].data();
#endif
	star = (int)row[ CUBSTAR.c_str()];
#ifndef NEWARCH
	doing = row[ CUBDOING.c_str()].get_string();
#else
	doing = row[ CUBDOING.c_str()].data();
#endif
	doingTime = (int)mysqlpp::DateTime(row[ CUBDOINGTIME.c_str()]);
	stage = (int)row[ CUBSTAGE.c_str()];
}
//---------------------------------------------------------------------------
Ice::ObjectPtr UserBasicFactoryI::create(Ice::Int id) {
	MCE_INFO("UserBasicFactoryI::create " << id);
	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(id);
	UserBasicDataIPtr rs = new UserBasicDataI ;
	UserBasicResultHandlerI handler(rs);
	size_t resultSize = QueryRunner(DB_USER_BASIC, CDbRServer).query(Statement::buildSelect(TABLE_USER_BASIC, filter), handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserBasicFactoryI::create NoSuchUserBasicException [" << id << "]");
		return NULL;
	}
	
	if (rs->id!= id) {
		MCE_WARN("UserBasicFactoryI::create UserBasicIdErrorException id=" << id << ", resultID="<<rs->id);
		return NULL;
	}
	MCE_INFO("UserBasicFactoryI::create " << id << " done.");
	return rs;
}

//---------------------------------------------------------------------------
UserBasicResultHandlerI::UserBasicResultHandlerI(const UserBasicDataIPtr& result) :	m_result(result) {
}

bool UserBasicResultHandlerI::handle(mysqlpp::Row& row) const {
	m_result->parse(row);
	if(m_result== 0)
	{
		MCE_WARN( "BatchBasicResHandlerI::parse ParseFailedException! ");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

void UserBornDataI::setProperty(const string& key, const string& val)
{
	if (!strcasecmp(key.c_str(), CUBOID.c_str())) {
		id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBOGENDER.c_str())) {
		gender = val;
	} else if (!strcasecmp(key.c_str(), CUBOBIRTHDAY.c_str())) {
		birthDay = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBOBIRTHMONTH.c_str())) {
		birthMonth = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBOBIRTHYEAR.c_str())) {
		birthYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUBOHOMEPROVINCE.c_str())) {
		homeProvince = val;
	} else if (!strcasecmp(key.c_str(), CUBOHOMECITY.c_str())) {
		homeCity = val;
	} else if (!strcasecmp(key.c_str(), CUBOCITYCODE.c_str())) {
		cityCode = val;
	} else if (!strcasecmp(key.c_str(), CUBOASTROLOGY.c_str())) {
		astrology = val;
	} else if (!strcasecmp(key.c_str(), CUBOISLUNARBIRTH.c_str())) {
		isLunarBirth = boost::lexical_cast<Ice::Int>(val);
	} 
}


UserBornDataIPtr UserBornDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	
	UserBornDataIPtr result = new UserBornDataI();
	
	result->id= id;
	result->gender= gender;
	result->birthDay= birthDay;
	result->birthMonth= birthMonth;
	result->birthYear= birthYear;
	result->homeProvince= homeProvince;
	result->homeCity= homeCity;
	result->cityCode= cityCode;
	result->astrology= astrology;
	result->isLunarBirth = isLunarBirth;
	return result;
}

void UserBornDataI::setProperties(const Str2StrMap& props)
{
	Str2StrMap::const_iterator it;
    IceUtil::Mutex::Lock lock(m_mutex);
	for (it = props.begin(); it != props.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

void UserBornDataI::parse(Ice::ObjectPtr& obj)
{
	UserBornDataNPtr data = UserBornDataNPtr::dynamicCast(obj);
	id = data->id ;
	gender = data->gender ;
	birthDay = data->birthDay ;
	birthMonth = data->birthMonth ;
	birthYear = data->birthYear ;
	homeProvince = data->homeProvince ;
	homeCity = data->homeCity ;
	cityCode = data->cityCode ;
	astrology = data->astrology ;
	isLunarBirth = data->isLunarBirth;
}
void UserBornDataI::parse(mysqlpp::Row& row)
{
	id = (int)row[ CUBOID.c_str()];
#ifndef NEWARCH
	gender = row[ CUBOGENDER.c_str()].get_string();
#else
	gender = row[ CUBOGENDER.c_str()].data();
#endif
	
	if(row[ CUBOBIRTHDAY.c_str()].is_null()) birthDay = 0;
	else birthDay = (int)row[ CUBOBIRTHDAY.c_str()];
	
	if(row[ CUBOBIRTHMONTH.c_str()].is_null()) birthMonth = 0;
	else birthMonth = (int)row[ CUBOBIRTHMONTH.c_str()];
	
	if(row[ CUBOBIRTHYEAR.c_str()].is_null()) birthYear = 0;
	else birthYear = (int)row[ CUBOBIRTHYEAR.c_str()];
	
	if(row[ CUBOISLUNARBIRTH.c_str()].is_null()) isLunarBirth = false;
	else isLunarBirth = (int)row[ CUBOISLUNARBIRTH.c_str()];
	
#ifndef NEWARCH
	homeProvince = row[ CUBOHOMEPROVINCE.c_str()].get_string();
	homeCity = row[ CUBOHOMECITY.c_str()].get_string();
	cityCode = row[ CUBOCITYCODE.c_str()].get_string();
	astrology = row[ CUBOASTROLOGY.c_str()].get_string();
#else
	homeProvince = row[ CUBOHOMEPROVINCE.c_str()].data();
	homeCity = row[ CUBOHOMECITY.c_str()].data();
	cityCode = row[ CUBOCITYCODE.c_str()].data();
	astrology = row[ CUBOASTROLOGY.c_str()].data();
#endif
}
//---------------------------------------------------------------------------
Ice::ObjectPtr UserBornFactoryI::create(Ice::Int id) {
	MCE_INFO("UserBornFactoryI::create " << id);
	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(id);
	UserBornDataIPtr rs = new UserBornDataI ;
	UserBornResultHandlerI handler(rs);
	size_t resultSize = QueryRunner(DB_USER_BORN, CDbRServer).query(Statement::buildSelect(TABLE_USER_BORN, filter), handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserBornFactoryI::create NoSuchUserBornException [" << id << "]");
		return NULL;
}
	
	if (rs->id!= id) {
		MCE_WARN("UserBornFactoryI::create UserBornIdErrorException id=" << id << ", resultID="<<rs->id);
		return NULL;
	}
	MCE_INFO("UserBornFactoryI::create " << id << " done.");
	return rs;
}

//---------------------------------------------------------------------------
UserBornResultHandlerI::UserBornResultHandlerI(const UserBornDataIPtr& result) :	m_result(result) {
}

bool UserBornResultHandlerI::handle(mysqlpp::Row& row) const {
	m_result->parse(row);
	if(m_result== 0)
	{
		MCE_WARN( "UserBornResultHandlerI::handle::parse ParseFailedException! ");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

void UserStageDataI::setProperty(const string& key, const string& val)
{
	if (!strcasecmp(key.c_str(), CUSGID.c_str())) {
		id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUSGUNIV.c_str())) {
		univ = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUSGSTAGE.c_str())) {
		stage = boost::lexical_cast<Ice::Int>(val);
	} 
}


UserStageDataIPtr UserStageDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	
	UserStageDataIPtr result = new UserStageDataI();
	
	result->id= id;
	result->univ= univ; 
	result->stage= stage;
	return result;
}

void UserStageDataI::setProperties(const Str2StrMap& props)
{
	Str2StrMap::const_iterator it;
    IceUtil::Mutex::Lock lock(m_mutex);
	for (it = props.begin(); it != props.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

void UserStageDataI::parse(Ice::ObjectPtr& obj)
{
	UserStageDataPtr data = UserStageDataPtr::dynamicCast(obj);
	id = data->id ;
	univ = data->univ ;
	stage = data->stage ;
}
void UserStageDataI::parse(mysqlpp::Row& row)
{
	id = (int)row[ CUSGID.c_str()];
	univ = (int)row[ CUSGUNIV.c_str()];
	stage = (int)row[ CUSGSTAGE.c_str()];
}
//---------------------------------------------------------------------------
Ice::ObjectPtr UserStageFactoryI::create(Ice::Int id) {
	MCE_INFO("UserStageFactoryI::create " << id);
	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(id);
	UserStageDataIPtr rs = new UserStageDataI ;
	UserStageResultHandlerI handler(rs);
	size_t resultSize = QueryRunner(DB_USER_STAGE, CDbRServer).query(Statement::buildSelect(TABLE_USER_STAGE, filter), handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserStageFactoryI::create NoSuchUserStageException [" << id << "]");
		return NULL;
	}
	
	if (rs->id!= id) {
		MCE_WARN("UserStageFactoryI::create UserStageIdErrorException id=" << id << ", resultID="<<rs->id);
		return NULL;
	}
	MCE_INFO("UserStageFactoryI::create " << id << " done.");
	return rs;
}

//---------------------------------------------------------------------------
UserStageResultHandlerI::UserStageResultHandlerI(const UserStageDataIPtr& result) :	m_result(result) {
}

bool UserStageResultHandlerI::handle(mysqlpp::Row& row) const {
	m_result->parse(row);
	if(m_result== 0)
	{
		MCE_WARN( "UserStageResultHandlerI::handl::parse ParseFailedException! ");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

void UserDoingDataI::setProperty(const string& key, const string& val)
{
	if (!strcasecmp(key.c_str(), CUDID.c_str())) {
		id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CUDDOING.c_str())) {
		doing = val;
	} else if (!strcasecmp(key.c_str(), CUDDOINGTIME.c_str())) {
		doingTime = (int)mysqlpp::DateTime(val);
	} 
}


UserDoingDataIPtr UserDoingDataI::getData()
{
	IceUtil::Mutex::Lock lock(m_mutex);
	
	UserDoingDataIPtr result = new UserDoingDataI();
	
	result->id= id;
	result->doing= doing;
	result->doingTime= doingTime;
	return result;
}

void UserDoingDataI::setProperties(const Str2StrMap& props)
{
	Str2StrMap::const_iterator it;
    IceUtil::Mutex::Lock lock(m_mutex);
	for (it = props.begin(); it != props.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

void UserDoingDataI::parse(Ice::ObjectPtr& obj)
{
	UserDoingDataPtr data = UserDoingDataPtr::dynamicCast(obj);
	id = data->id ;
	doing= data->doing;
	doingTime= data->doingTime;
}

void UserDoingDataI::parse(mysqlpp::Row& row)
{
	id = (int)row[ CUDID.c_str()];
#ifndef NEWARCH
	doing = row[ CUDDOING.c_str()].get_string();
#else
	doing = row[ CUDDOING.c_str()].data();
#endif
	doingTime = (int)mysqlpp::DateTime(row[ CUBDOINGTIME.c_str()]);
}
//---------------------------------------------------------------------------
Ice::ObjectPtr UserDoingFactoryI::create(Ice::Int id) {
	MCE_INFO("UserDoingFactoryI::create " << id);
	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(id);
	UserDoingDataIPtr rs = new UserDoingDataI ;
	UserDoingResultHandlerI handler(rs);
	size_t resultSize = QueryRunner(DB_USER_DOING, CDbRServer).query(Statement::buildSelect(TABLE_USER_DOING, filter), handler);
	
	if (resultSize == 0) {
		MCE_WARN("UserDoingFactoryI::create NoSuchUserDoingException [" << id << "]");
		return NULL;
	}
	
	if (rs->id!= id) {
		MCE_WARN("UserDoingFactoryI::create UserDoingIdErrorException id=" << id << ", resultID="<<rs->id);
		return NULL;
	}
	MCE_INFO("UserDoingFactoryI::create " << id << " done.");
	return rs;
}

//---------------------------------------------------------------------------
UserDoingResultHandlerI::UserDoingResultHandlerI(const UserDoingDataIPtr& result) :	m_result(result) {
}

bool UserDoingResultHandlerI::handle(mysqlpp::Row& row) const {
	m_result->parse(row);
	if(m_result== 0)
	{
		MCE_WARN( "UserDoingResultHandlerI::handle::parse ParseFailedException! ");
		return false;
	}
	return true;
}

