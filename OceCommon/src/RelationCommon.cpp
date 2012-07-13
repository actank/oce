#include "RelationCommon.h"
#include "Date.h"
using namespace mop::hi::svc::relation;
using namespace com::xiaonei::xce;
using namespace MyUtil;


//---------------------------------------------------------------------------
string
BuddyHandle::getProperty(const string& key)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CRGUEST.c_str())) {
		return boost::lexical_cast<string>(value<BuddyDataPtr>()->buddyId);
	} else if (!strcasecmp(key.c_str(), CRHOST.c_str())) {
		return boost::lexical_cast<string>(value<BuddyDataPtr>()->ownerId);
	} else if (!strcasecmp(key.c_str(), CRGROUPID.c_str())) {
		return boost::lexical_cast<string>(value<BuddyDataPtr>()->groupId);
	} else if (!strcasecmp(key.c_str(), CRTIME.c_str())) {
		return MyUtil::Date(value<BuddyDataPtr>()->timestamp).str();
	}
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void
BuddyHandle::setProperty(const string& key, const string& val)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CRGUEST.c_str())) {
		value<BuddyDataPtr>()->buddyId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRHOST.c_str())) {
		value<BuddyDataPtr>()->ownerId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRGROUPID.c_str())) {
		value<BuddyDataPtr>()->groupId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRTIME.c_str())) {
		value<BuddyDataPtr>()->timestamp = MyUtil::Date(val).time();
	}
}

Str2StrMap
BuddyHandle::getProperties()
{
	Str2StrMap result;
	result[CRHOST] = getProperty(CRHOST);
	result[CRGUEST] = getProperty(CRGUEST);
	result[CRGROUPID] = getProperty(CRGROUPID);
	result[CRTIME] = getProperty(CRTIME);
	return result;
}

void
BuddyHandle::setProperties(const Str2StrMap& properties)
{
	Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
    	setProperty(it->first, it->second);
    }
}
/*
DataHandlePtr
BuddyHandle::parse(Connection& conn, int row)
{
	DataHandlePtr d = new BuddyHandle;
	BuddyDataPtr b = new BuddyData;
	b->ownerId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRHOST.c_str());
	b->buddyId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRGUEST.c_str());
	b->groupId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRGROUPID.c_str());
	b->timestamp = Date(conn.cursor()->getField(row, CRTIME.c_str())).time();
	d->value(b);
	return d;
};
*/

DataHandlePtr
BuddyHandle::parse(mysqlpp::Row& row)
{
	DataHandlePtr d = new BuddyHandle;
	BuddyDataPtr b = new BuddyData;
	b->ownerId = (int)row[CRHOST.c_str()];
	b->buddyId = (int)row[CRGUEST.c_str()];
	b->groupId = (int)row[CRGROUPID.c_str()];
#ifndef NEWARCH
	b->timestamp = MyUtil::Date(row[CRTIME.c_str()].get_string()).time();
#else
	b->timestamp = MyUtil::Date(row[CRTIME.c_str()].data()).time();
#endif
	d->value(b);
	return d;
};


//---------------------------------------------------------------------------
string
GroupHandle::getProperty(const string& key)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CGNAME.c_str())) {
		return value<GroupDataPtr>()->name;
	} else if (!strcasecmp(key.c_str(), CGGROUP.c_str())) {
		return boost::lexical_cast<string>(value<GroupDataPtr>()->groupId);
	} else if (!strcasecmp(key.c_str(), CGHOST.c_str())) {
		return boost::lexical_cast<string>(value<GroupDataPtr>()->ownerId);
	}
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void
GroupHandle::setProperty(const string& key, const string& val)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CGNAME.c_str())) {
		value<GroupDataPtr>()->name = val;
	} else if (!strcasecmp(key.c_str(), CGHOST.c_str())) {
		value<GroupDataPtr>()->ownerId = boost::lexical_cast<Ice::Int>(val);
	}  else if (!strcasecmp(key.c_str(), CGGROUP.c_str())) {
		value<GroupDataPtr>()->groupId = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap
GroupHandle::getProperties()
{
	Str2StrMap result;
	result[CGNAME] = getProperty(CGNAME);
	result[CGHOST] = getProperty(CGHOST);
	result[CGGROUP] = getProperty(CGGROUP);
	return result;
}

void
GroupHandle::setProperties(const Str2StrMap& properties)
{
	Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
    	setProperty(it->first, it->second);
    }
}
/*
DataHandlePtr
GroupHandle::parse(Connection& conn, int row)
{
	DataHandlePtr d = new GroupHandle;
	GroupDataPtr b = new GroupData;
	b->name = conn.cursor()->getField(row, CGNAME.c_str());
	b->ownerId = conn.cursor()->getFieldAsInteger(row, CGHOST.c_str());
	b->groupId = conn.cursor()->getFieldAsInteger(row, CGGROUP.c_str());
	d->value(b);
	return d;
};
*/
DataHandlePtr
GroupHandle::parse(mysqlpp::Row&  row)
{
	DataHandlePtr d = new GroupHandle;
	GroupDataPtr b = new GroupData;
#ifndef NEWARCH
	b->name = row[CGNAME.c_str()].get_string();
#else
	b->name = row[CGNAME.c_str()].data();
#endif
	b->ownerId = (int)row[CGHOST.c_str()];
	b->groupId = (int)row[CGGROUP.c_str()];
	d->value(b);
	return d;
};


//---------------------------------------------------------------------------
string
BlockHandle::getProperty(const string& key)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CBGUEST.c_str())) {
		return boost::lexical_cast<string>(value<BlockDataPtr>()->blockeeId);
	} else if (!strcasecmp(key.c_str(), CBHOST.c_str())) {
		return boost::lexical_cast<string>(value<BlockDataPtr>()->blockerId);
	} else if (!strcasecmp(key.c_str(), CBTIME.c_str())) {
		return MyUtil::Date(value<BlockDataPtr>()->timestamp).str();
	}
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void
BlockHandle::setProperty(const string& key, const string& val)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CBGUEST.c_str())) {
		value<BlockDataPtr>()->blockeeId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CBHOST.c_str())) {
		value<BlockDataPtr>()->blockerId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CBTIME.c_str())) {
		value<BlockDataPtr>()->timestamp = MyUtil::Date(val).time();
	}
}

Str2StrMap
BlockHandle::getProperties()
{
	Str2StrMap result;
	result[CBHOST] = getProperty(CBHOST);
	result[CBGUEST] = getProperty(CBGUEST);
	result[CBTIME] = getProperty(CBTIME);
	return result;
}

void
BlockHandle::setProperties(const Str2StrMap& properties)
{
	Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
    	setProperty(it->first, it->second);
    }
}
/*
DataHandlePtr
BlockHandle::parse(Connection& conn, int row)
{
	DataHandlePtr d = new BlockHandle;
	BlockDataPtr b = new BlockData;
	b->blockerId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CBHOST.c_str());
	b->blockeeId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CBGUEST.c_str());
	b->timestamp = Date(conn.cursor()->getField(row, CBTIME.c_str())).time();
	d->value(b);
	return d;
};*/

DataHandlePtr
BlockHandle::parse(mysqlpp::Row& row)
{
	DataHandlePtr d = new BlockHandle;
	BlockDataPtr b = new BlockData;
	b->blockerId = (int)row[CBHOST.c_str()];
	b->blockeeId = (int)row[CBGUEST.c_str()];
#ifndef NEWARCH
	b->timestamp = MyUtil::Date(row[CBTIME.c_str()].get_string()).time();
#else
	b->timestamp = MyUtil::Date(row[CBTIME.c_str()].data()).time();
#endif
	d->value(b);
	return d;
};


//---------------------------------------------------------------------------
string
BlockedHandle::getProperty(const string& key)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CBGUEST.c_str())) {
		return boost::lexical_cast<string>(value<BlockedDataPtr>()->blockerId);
	} else if (!strcasecmp(key.c_str(), CBHOST.c_str())) {
		return boost::lexical_cast<string>(value<BlockedDataPtr>()->blockerId);
	} else if (!strcasecmp(key.c_str(), CBTIME.c_str())) {
		return MyUtil::Date(value<BlockedDataPtr>()->timestamp).str();
	}
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void
BlockedHandle::setProperty(const string& key, const string& val)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CBGUEST.c_str())) {
		value<BlockedDataPtr>()->blockerId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CBHOST.c_str())) {
		value<BlockedDataPtr>()->blockerId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CBTIME.c_str())) {
		value<BlockedDataPtr>()->timestamp = MyUtil::Date(val).time();
	}
}

Str2StrMap
BlockedHandle::getProperties()
{
	Str2StrMap result;
	result[CBHOST] = getProperty(CBHOST);
	result[CBGUEST] = getProperty(CBGUEST);
	result[CBTIME] = getProperty(CBTIME);
	return result;
}

void
BlockedHandle::setProperties(const Str2StrMap& properties)
{
	Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
    	setProperty(it->first, it->second);
    }
}
/*
DataHandlePtr
BlockedHandle::parse(Connection& conn, int row)
{
	DataHandlePtr d = new BlockedHandle;
	BlockedDataPtr b = new BlockedData;
	b->blockerId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CBHOST.c_str());
	b->blockeeId = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CBGUEST.c_str());
	b->timestamp = Date(conn.cursor()->getField(row, CBTIME.c_str())).time();
	d->value(b);
	return d;
};
*/

DataHandlePtr
BlockedHandle::parse(mysqlpp::Row& row)
{
	DataHandlePtr d = new BlockedHandle;
	BlockedDataPtr b = new BlockedData;
	b->blockerId = (int)row[CBHOST.c_str()];
	b->blockeeId = (int)row[CBGUEST.c_str()];
#ifndef NEWARCH
	b->timestamp = MyUtil::Date(row[CBTIME.c_str()].get_string()).time();
#else
	b->timestamp = MyUtil::Date(row[CBTIME.c_str()].data()).time();
#endif
	d->value(b);
	return d;
};
//---------------------------------------------------------------------------
string
RequestHandle::getProperty(const string& key)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CRQAPPLICANT.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->applicant);
	} else if (!strcasecmp(key.c_str(), CRQACCEPTER.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->accepter);
	} else if (!strcasecmp(key.c_str(), CRQTIME.c_str())) {
		return MyUtil::Date(value<RequestDataPtr>()->timestamp).str();
	} else if (!strcasecmp(key.c_str(), CRQAPPLICANTNAME.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->applicantName);
	} else if (!strcasecmp(key.c_str(), CRQENROLLMENTYEAR.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->enrollmentYear);
	} else if (!strcasecmp(key.c_str(), CRQSCHOOLID.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->schoolId);
	} else if (!strcasecmp(key.c_str(), CRQDEPARTMENT.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->department);
	} else if (!strcasecmp(key.c_str(), CRQDORM.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->dorm);
	} else if (!strcasecmp(key.c_str(), CRQHEADURL.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->headUrl);
	} else if (!strcasecmp(key.c_str(), CRQWHY.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->why);
	} else if (!strcasecmp(key.c_str(), CRQSCHOOLNAME.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->schoolName);
	} else if (!strcasecmp(key.c_str(), CRQSELECTED.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->selected);
	} else if (!strcasecmp(key.c_str(), CRQSTAR.c_str())) {
		return boost::lexical_cast<string>(value<RequestDataPtr>()->star);
	}
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void
RequestHandle::setProperty(const string& key, const string& val)
{
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CRQAPPLICANT.c_str())) {
		value<RequestDataPtr>()->applicant = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRQACCEPTER.c_str())) {
		value<RequestDataPtr>()->accepter = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRQTIME.c_str())) {
		value<RequestDataPtr>()->timestamp = MyUtil::Date(val).time();
	} else if (!strcasecmp(key.c_str(), CRQAPPLICANTNAME.c_str())) {
		value<RequestDataPtr>()->applicantName = val;
	} else if (!strcasecmp(key.c_str(), CRQENROLLMENTYEAR.c_str())) {
		value<RequestDataPtr>()->enrollmentYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRQSCHOOLID.c_str())) {
		value<RequestDataPtr>()->schoolId = val;
	} else if (!strcasecmp(key.c_str(), CRQDEPARTMENT.c_str())) {
		value<RequestDataPtr>()->department = val;
	} else if (!strcasecmp(key.c_str(), CRQDORM.c_str())) {
		value<RequestDataPtr>()->dorm = val;
	} else if (!strcasecmp(key.c_str(), CRQHEADURL.c_str())) {
		value<RequestDataPtr>()->headUrl = val;
	} else if (!strcasecmp(key.c_str(), CRQWHY.c_str())) {
		value<RequestDataPtr>()->why = val;
	} else if (!strcasecmp(key.c_str(), CRQSCHOOLNAME.c_str())) {
		value<RequestDataPtr>()->schoolName = val;
	} else if (!strcasecmp(key.c_str(), CRQSELECTED.c_str())) {
		value<RequestDataPtr>()->selected = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CRQSTAR.c_str())) {
		value<RequestDataPtr>()->star = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap
RequestHandle::getProperties()
{
	Str2StrMap result;
	result[CRQAPPLICANT] = getProperty(CRQAPPLICANT);
	result[CRQACCEPTER] = getProperty(CRQACCEPTER);
	result[CRQTIME] = getProperty(CRQTIME);
	result[CRQAPPLICANTNAME] = getProperty(CRQAPPLICANTNAME);
	result[CRQENROLLMENTYEAR] = getProperty(CRQENROLLMENTYEAR);
	result[CRQSCHOOLID] = getProperty(CRQSCHOOLID);
	result[CRQDEPARTMENT] = getProperty(CRQDEPARTMENT);
	result[CRQDORM] = getProperty(CRQDORM);
	result[CRQHEADURL] = getProperty(CRQHEADURL);
	result[CRQWHY] = getProperty(CRQWHY);
	result[CRQSCHOOLNAME] = getProperty(CRQSCHOOLNAME);
	result[CRQSELECTED] = getProperty(CRQSELECTED);
	result[CRQSTAR] = getProperty(CRQSTAR);
	return result;
}

void
RequestHandle::setProperties(const Str2StrMap& properties)
{
	Str2StrMap::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
    	setProperty(it->first, it->second);
    }
}
/*
DataHandlePtr
RequestHandle::parse(Connection& conn, int row)
{
	DataHandlePtr d = new RequestHandle;
	RequestDataPtr r = new RequestData;
	r->applicant = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRQAPPLICANT.c_str());
	r->accepter = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRQACCEPTER.c_str());
	r->timestamp = Date(conn.cursor()->getField(row, CRQTIME.c_str())).time();
	r->applicantName = conn.cursor()->getField(row, CRQAPPLICANTNAME.c_str());
	r->enrollmentYear = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRQENROLLMENTYEAR.c_str());
	r->schoolId = conn.cursor()->getField(row, CRQSCHOOLID.c_str());
	r->department = conn.cursor()->getField(row, CRQDEPARTMENT.c_str());
	r->dorm = conn.cursor()->getField(row, CRQDORM.c_str());
	r->headUrl = conn.cursor()->getField(row, CRQHEADURL.c_str());
	r->why = conn.cursor()->getField(row, CRQWHY.c_str());
	r->schoolName = conn.cursor()->getField(row, CRQSCHOOLNAME.c_str());
	r->selected = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRQSELECTED.c_str());
	r->star = (Ice::Int)conn.cursor()->getFieldAsInteger(row, CRQSTAR.c_str());
	d->value(r);
	return d;
};*/

DataHandlePtr 
RequestHandle::parse(mysqlpp::Row& row)
{
	DataHandlePtr d = new RequestHandle;
	RequestDataPtr r = new RequestData;
	r->applicant = (int) row[CRQAPPLICANT.c_str()];
	r->accepter = (int) row[CRQACCEPTER.c_str()];
#ifndef NEWARCH
	r->timestamp = MyUtil::Date(row[CRQTIME.c_str()].get_string()).time();
	r->applicantName = row[CRQAPPLICANTNAME.c_str()].get_string();
#else
	r->timestamp = MyUtil::Date(row[CRQTIME.c_str()].data()).time();
	r->applicantName = row[CRQAPPLICANTNAME.c_str()].data();
#endif
	r->enrollmentYear = (int)row[CRQENROLLMENTYEAR.c_str()];
#ifndef NEWARCH
	r->schoolId = row[CRQSCHOOLID.c_str()].get_string();
	r->department = row[CRQDEPARTMENT.c_str()].get_string();
	r->dorm = row[CRQDORM.c_str()].get_string();
	r->headUrl = row[CRQHEADURL.c_str()].get_string();
	r->why = row[CRQWHY.c_str()].get_string();
	r->schoolName = row[CRQSCHOOLNAME.c_str()].get_string();
#else
	r->schoolId = row[CRQSCHOOLID.c_str()].data();
	r->department = row[CRQDEPARTMENT.c_str()].data();
	r->dorm = row[CRQDORM.c_str()].data();
	r->headUrl = row[CRQHEADURL.c_str()].data();
	r->why = row[CRQWHY.c_str()].data();
	r->schoolName = row[CRQSCHOOLNAME.c_str()].data();
#endif
	r->selected = (int)row[CRQSELECTED.c_str()];
	r->star = (int)row[CRQSTAR.c_str()];
	d->value(r);
	return d;
}

