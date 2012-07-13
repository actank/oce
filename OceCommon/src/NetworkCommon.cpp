#include "NetworkCommon.h"

using namespace mop::hi::svc::model;
using namespace MyUtil;

string NetworkHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), NetColID.c_str())) {
		return boost::lexical_cast<string>(value<NetworkPtr>()->id);
	} else if (!strcasecmp(key.c_str(), NetColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<NetworkPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), NetColSTAGE.c_str())) {
		return boost::lexical_cast<string>(value<NetworkPtr>()->stage);
	} else if (!strcasecmp(key.c_str(), NetColINFOID.c_str())) {
		return boost::lexical_cast<string>(value<NetworkPtr>()->infoId);
	} else if (!strcasecmp(key.c_str(), NetColJOINTIME.c_str())) {
		return value<NetworkPtr>()->joinTime;
	} else if (!strcasecmp(key.c_str(), NetColSTATUS.c_str())) {
		return boost::lexical_cast<string>(value<NetworkPtr>()->status);
	} else if (!strcasecmp(key.c_str(), NetColNetworkName.c_str())) {
		return value<NetworkPtr>()->networkName;
	} else if (!strcasecmp(key.c_str(), NetColNetworkId.c_str())) {
		return boost::lexical_cast<string>(value<NetworkPtr>()->networkId);
	}

	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void NetworkHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), NetColID.c_str())) {
		value<NetworkPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), NetColUSERID.c_str())) {
		value<NetworkPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), NetColSTAGE.c_str())) {
		value<NetworkPtr>()->stage = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), NetColINFOID.c_str())) {
		value<NetworkPtr>()->infoId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), NetColJOINTIME.c_str())) {
		value<NetworkPtr>()->joinTime = val;
	} else if (!strcasecmp(key.c_str(), NetColSTATUS.c_str())) {
		value<NetworkPtr>()->status = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), NetColNetworkName.c_str())) {
		value<NetworkPtr>()->networkName = val;
	} else if (!strcasecmp(key.c_str(), NetColNetworkId.c_str())) {
		value<NetworkPtr>()->networkId = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap NetworkHandle::getProperties() {
	Str2StrMap result;
	result[NetColUSERID] = getProperty(NetColUSERID);
	result[NetColID] = getProperty(NetColID);
	result[NetColSTAGE] = getProperty(NetColSTAGE);
	result[NetColINFOID] = getProperty(NetColINFOID);
	result[NetColJOINTIME] = getProperty(NetColJOINTIME);
	result[NetColSTATUS] = getProperty(NetColSTATUS);
	result[NetColNetworkName] = getProperty(NetColNetworkName);
	result[NetColNetworkId] = getProperty(NetColNetworkId);
	return result;
}

void NetworkHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr NetworkHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new NetworkHandle;
	NetworkPtr b = new Network;
	b->id = (Ice::Int)row[ NetColID.c_str()];
	b->userId = (Ice::Int)row[ NetColUSERID.c_str()];
	b->stage = (Ice::Int)row[ NetColSTAGE.c_str()];
	b->infoId = (Ice::Int)row[ NetColINFOID.c_str()];
#ifndef NEWARCH
	b->joinTime = row[ NetColJOINTIME.c_str()].get_string();
#else
	b->joinTime = row[ NetColJOINTIME.c_str()].data();
#endif
	b->status = (Ice::Int)row[ NetColSTATUS.c_str()];
#ifndef NEWARCH
	b->networkName = row[ NetColNetworkName.c_str()].get_string();
#else
	b->networkName = row[ NetColNetworkName.c_str()].data();
#endif
	b->networkId = (Ice::Int)row[ NetColNetworkId.c_str()];
	d->value(b);
	return d;
}
;

//--------------------------------------------------------------------------------

string HighSchoolInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), HighColID.c_str())) {
		return boost::lexical_cast<string>(value<HighSchoolInfoPtr>()->id);
	} else if (!strcasecmp(key.c_str(), HighColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<HighSchoolInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), HighColHighSchoolID.c_str())) {
		return boost::lexical_cast<string>(value<HighSchoolInfoPtr>()->HighSchoolId);
	} else if (!strcasecmp(key.c_str(), HighColHighSchoolNAME.c_str())) {
		return value<HighSchoolInfoPtr>()->HighSchoolName;
	} else if (!strcasecmp(key.c_str(), HighColENROLLYEAR.c_str())) {
		return boost::lexical_cast<string>(value<HighSchoolInfoPtr>()->enrollYear);
	} else if (!strcasecmp(key.c_str(), HighColHCLASS1.c_str())) {
		return value<HighSchoolInfoPtr>()->hClass1;
	} else if (!strcasecmp(key.c_str(), HighColHCLASS2.c_str())) {
		return value<HighSchoolInfoPtr>()->hClass2;
	} else if (!strcasecmp(key.c_str(), HighColHCLASS3.c_str())) {
		return value<HighSchoolInfoPtr>()->hClass3;
		//	} else if (!strcasecmp(key.c_str(), HighColTRIBE.c_str())) {
		//	    return boost::lexical_cast<string>(value<HighSchoolInfoPtr>()->tribe);
	}

	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void HighSchoolInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), HighColID.c_str())) {
		value<HighSchoolInfoPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), HighColUSERID.c_str())) {
		value<HighSchoolInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), HighColHighSchoolID.c_str())) {
		value<HighSchoolInfoPtr>()->HighSchoolId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), HighColHighSchoolNAME.c_str())) {
		value<HighSchoolInfoPtr>()->HighSchoolName = val;
	} else if (!strcasecmp(key.c_str(), HighColENROLLYEAR.c_str())) {
		value<HighSchoolInfoPtr>()->enrollYear = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), HighColHCLASS1.c_str())) {
		value<HighSchoolInfoPtr>()->hClass1 = val;
	} else if (!strcasecmp(key.c_str(), HighColHCLASS2.c_str())) {
		value<HighSchoolInfoPtr>()->hClass2 = val;
	} else if (!strcasecmp(key.c_str(), HighColHCLASS3.c_str())) {
		value<HighSchoolInfoPtr>()->hClass3 = val;
		//	} else if (!strcasecmp(key.c_str(), HighColTRIBE.c_str())) {
		//	    value<HighSchoolInfoPtr>()->tribe = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap HighSchoolInfoHandle::getProperties() {
	Str2StrMap result;
	result[HighColID] = getProperty(HighColID);
	result[HighColUSERID] = getProperty(HighColUSERID);
	result[HighColHighSchoolID] = getProperty(HighColHighSchoolID);
	result[HighColHighSchoolNAME ] = getProperty(HighColHighSchoolNAME);
	result[HighColENROLLYEAR] = getProperty(HighColENROLLYEAR);
	result[HighColHCLASS1] = getProperty(HighColHCLASS1);
	result[HighColHCLASS2] = getProperty(HighColHCLASS2);
	result[HighColHCLASS3] = getProperty(HighColHCLASS3);
	//	result[HighColTRIBE] = getProperty(HighColTRIBE);
	return result;
}

void HighSchoolInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr HighSchoolInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new HighSchoolInfoHandle;
	HighSchoolInfoPtr b = new HighSchoolInfo();
	b->id = (int)row[ HighColID.c_str()];
	b->userId = (int)row[ HighColUSERID.c_str()];
	b->HighSchoolId = (int)row[ HighColHighSchoolID.c_str()];
#ifndef NEWARCH
	b->HighSchoolName = row[ HighColHighSchoolNAME.c_str()].get_string();
#else
	b->HighSchoolName = row[ HighColHighSchoolNAME.c_str()].data();
#endif
	b->enrollYear = (int)row[ HighColENROLLYEAR.c_str()];
#ifndef NEWARCH
	b->hClass1 = row[ HighColHCLASS1.c_str()].get_string();
	b->hClass2 = row[ HighColHCLASS2.c_str()].get_string();
	b->hClass3 = row[ HighColHCLASS3.c_str()].get_string();
#else
	b->hClass1 = row[ HighColHCLASS1.c_str()].data();
	b->hClass2 = row[ HighColHCLASS2.c_str()].data();
	b->hClass3 = row[ HighColHCLASS3.c_str()].data();
#endif

	d->value(b);
	return d;
}
;

//--------------------------------------------------------------------------------

string UniversityInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), UniColID.c_str())) {
		return boost::lexical_cast<string>(value<UniversityInfoPtr>()->id);
	} else if (!strcasecmp(key.c_str(), UniColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<UniversityInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), UniColUNIVERSITYID.c_str())) {
		return boost::lexical_cast<string>(value<UniversityInfoPtr>()->universityId);
	} else if (!strcasecmp(key.c_str(), UniColUNIVERSITYNAME.c_str())) {
		return value<UniversityInfoPtr>()->universityName;
	} else if (!strcasecmp(key.c_str(), UniColDORM.c_str())) {
		return value<UniversityInfoPtr>()->dorm;
	} else if (!strcasecmp(key.c_str(), UniColDEPARTMENT.c_str())) {
		return value<UniversityInfoPtr>()->department;
	} else if (!strcasecmp(key.c_str(), UniColTYPEOFCOURSE.c_str())) {
		return value<UniversityInfoPtr>()->typeOfCourse;
	} else if (!strcasecmp(key.c_str(), UniColTYPEENROLLYEAR.c_str())) {
		return boost::lexical_cast<string>(value<UniversityInfoPtr>()->enrollYear);
	}

	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void UniversityInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), UniColID.c_str())) {
		value<UniversityInfoPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), UniColUSERID.c_str())) {
		value<UniversityInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), UniColUNIVERSITYID.c_str())) {
		value<UniversityInfoPtr>()->universityId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), UniColUNIVERSITYNAME.c_str())) {
		value<UniversityInfoPtr>()->universityName = val;
	} else if (!strcasecmp(key.c_str(), UniColDORM.c_str())) {
		value<UniversityInfoPtr>()->dorm = val;
	} else if (!strcasecmp(key.c_str(), UniColDEPARTMENT.c_str())) {
		value<UniversityInfoPtr>()->department = val;
	} else if (!strcasecmp(key.c_str(), UniColTYPEOFCOURSE.c_str())) {
		value<UniversityInfoPtr>()->typeOfCourse = val;
	} else if (!strcasecmp(key.c_str(), UniColTYPEENROLLYEAR.c_str())) {
		value<UniversityInfoPtr>()->enrollYear = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap UniversityInfoHandle::getProperties() {
	Str2StrMap result;
	result[UniColID] = getProperty(UniColID);
	result[UniColUSERID] = getProperty(UniColUSERID);
	result[UniColUNIVERSITYID] = getProperty(UniColUNIVERSITYID);
	result[UniColUNIVERSITYNAME ] = getProperty(UniColUNIVERSITYNAME);
	result[UniColDORM] = getProperty(UniColDORM);
	result[UniColDEPARTMENT] = getProperty(UniColDEPARTMENT);
	result[UniColTYPEOFCOURSE] = getProperty(UniColTYPEOFCOURSE);
	result[UniColTYPEENROLLYEAR] = getProperty(UniColTYPEENROLLYEAR);
	return result;
}

void UniversityInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr UniversityInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new UniversityInfoHandle;
	UniversityInfoPtr b = new UniversityInfo();
	b->id = (int)row[ UniColID.c_str()];
	b->userId = (int)row[ UniColUSERID.c_str()];
	b->universityId = (int)row[ UniColUNIVERSITYID.c_str()];
#ifndef NEWARCH
	b->universityName = row[ UniColUNIVERSITYNAME.c_str()].get_string();
	b->dorm = row[ UniColDORM.c_str()].get_string();
	b->department = row[ UniColDEPARTMENT.c_str()].get_string();
	b->typeOfCourse = row[ UniColTYPEOFCOURSE.c_str()].get_string();
#else
	b->universityName = row[ UniColUNIVERSITYNAME.c_str()].data();
	b->dorm = row[ UniColDORM.c_str()].data();
	b->department = row[ UniColDEPARTMENT.c_str()].data();
	b->typeOfCourse = row[ UniColTYPEOFCOURSE.c_str()].data();
#endif
	b->enrollYear = (int)row[ UniColTYPEENROLLYEAR.c_str()];

	d->value(b);
	return d;
}
;

//--------------------------------------------------------------------------------


string WorkplaceInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), WorkColID.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->id);
	} else if (!strcasecmp(key.c_str(), WorkColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), WorkColWORKPLACEID.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->workplaceId);
	} else if (!strcasecmp(key.c_str(), WorkColWORKPLACENAME.c_str())) {
		return value<WorkspaceInfoPtr>()->workplaceName;
	} else if (!strcasecmp(key.c_str(), WorkColADDRESS.c_str())) {
		return value<WorkspaceInfoPtr>()->address;
	} else if (!strcasecmp(key.c_str(), WorkColDESCRIPTION.c_str())) {
		return value<WorkspaceInfoPtr>()->description;
	} else if (!strcasecmp(key.c_str(), WorkColPROVINCE.c_str())) {
		return value<WorkspaceInfoPtr>()->province;
	} else if (!strcasecmp(key.c_str(), WorkColCITYID.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->cityId);
	} else if (!strcasecmp(key.c_str(), WorkColCITYNAME.c_str())) {
		return value<WorkspaceInfoPtr>()->cityName;
	} else if (!strcasecmp(key.c_str(), WorkColJOINYEAR.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->joinYear);
	} else if (!strcasecmp(key.c_str(), WorkColJOINMONTH.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->joinMonth);
	} else if (!strcasecmp(key.c_str(), WorkColQUITYEAR.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->quitYear);
	} else if (!strcasecmp(key.c_str(), WorkColQUITMONTH.c_str())) {
		return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->quitMonth);
	}else if (!strcasecmp(key.c_str(), WorkColTYPE.c_str())) {
	    return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->type);
	}else if (!strcasecmp(key.c_str(), WorkColJOBTITLEID.c_str())) {
	    return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->jobTitleId);
	}else if (!strcasecmp(key.c_str(), WorkColPOSITIONID.c_str())) {
	    return boost::lexical_cast<string>(value<WorkspaceInfoPtr>()->positionId);
	}	   
	
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void WorkplaceInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), WorkColID.c_str())) {
		value<WorkspaceInfoPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColUSERID.c_str())) {
		value<WorkspaceInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColWORKPLACEID.c_str())) {
		value<WorkspaceInfoPtr>()->workplaceId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColWORKPLACENAME.c_str())) {
		value<WorkspaceInfoPtr>()->workplaceName = val;
	} else if (!strcasecmp(key.c_str(), WorkColADDRESS.c_str())) {
		value<WorkspaceInfoPtr>()->address = val;
	} else if (!strcasecmp(key.c_str(), WorkColDESCRIPTION.c_str())) {
		value<WorkspaceInfoPtr>()->description = val;
	} else if (!strcasecmp(key.c_str(), WorkColPROVINCE.c_str())) {
		value<WorkspaceInfoPtr>()->province = val;
	} else if (!strcasecmp(key.c_str(), WorkColCITYID.c_str())) {
		value<WorkspaceInfoPtr>()->cityId = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColCITYNAME.c_str())) {
		value<WorkspaceInfoPtr>()->cityName = val;
	} else if (!strcasecmp(key.c_str(), WorkColJOINYEAR.c_str())) {
		value<WorkspaceInfoPtr>()->joinYear = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColJOINMONTH.c_str())) {
		value<WorkspaceInfoPtr>()->joinMonth = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColQUITYEAR.c_str())) {
		value<WorkspaceInfoPtr>()->quitYear = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColQUITMONTH.c_str())) {
		value<WorkspaceInfoPtr>()->quitMonth = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColTYPE.c_str())) {
	    value<WorkspaceInfoPtr>()->type = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColJOBTITLEID.c_str())) {
	    value<WorkspaceInfoPtr>()->jobTitleId = boost::lexical_cast<int>(val);
	} else if (!strcasecmp(key.c_str(), WorkColPOSITIONID.c_str())) {
	    value<WorkspaceInfoPtr>()->positionId = boost::lexical_cast<int>(val);
	}
}

Str2StrMap WorkplaceInfoHandle::getProperties() {
	Str2StrMap result;
	result[WorkColID] = getProperty(WorkColID);
	result[WorkColUSERID] = getProperty(WorkColUSERID);
	result[WorkColWORKPLACEID] = getProperty(WorkColWORKPLACEID);
	result[WorkColWORKPLACENAME ] = getProperty(WorkColWORKPLACENAME);
	result[WorkColADDRESS] = getProperty(WorkColADDRESS);
	result[WorkColDESCRIPTION] = getProperty(WorkColDESCRIPTION);
	result[WorkColPROVINCE] = getProperty(WorkColPROVINCE);
	result[WorkColCITYID] = getProperty(WorkColCITYID);
	result[WorkColCITYNAME] = getProperty(WorkColCITYNAME);
	result[WorkColJOINYEAR] = getProperty(WorkColJOINYEAR);
	result[WorkColJOINMONTH] = getProperty(WorkColJOINMONTH);
	result[WorkColQUITYEAR] = getProperty(WorkColQUITYEAR);
	result[WorkColQUITMONTH] = getProperty(WorkColQUITMONTH);
	result[WorkColTYPE] = getProperty(WorkColTYPE);
	result[WorkColJOBTITLEID] = getProperty(WorkColJOBTITLEID);
	result[WorkColPOSITIONID] = getProperty(WorkColPOSITIONID);
	return result;
}

void WorkplaceInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr WorkplaceInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new WorkplaceInfoHandle;
	WorkplaceInfoPtr b = new WorkplaceInfo();
	b->id = (int)row[ WorkColID.c_str()];
	b->userId = (int)row[ WorkColUSERID.c_str()];
	b->workplaceId = (int)row[ WorkColWORKPLACEID.c_str()];
#ifndef NEWARCH
	b->workplaceName = row[ WorkColWORKPLACENAME.c_str()].get_string();
	b->address = row[ WorkColADDRESS.c_str()].get_string();
	b->description = row[ WorkColDESCRIPTION.c_str()].get_string();
	b->province = row[ WorkColPROVINCE.c_str()].get_string();
#else
	b->workplaceName = row[ WorkColWORKPLACENAME.c_str()].data();
	b->address = row[ WorkColADDRESS.c_str()].data();
	b->description = row[ WorkColDESCRIPTION.c_str()].data();
	b->province = row[ WorkColPROVINCE.c_str()].data();
#endif
	b->cityId = (int)row[ WorkColCITYID.c_str()];
#ifndef NEWARCH
	b->cityName = row[ WorkColCITYNAME.c_str()].get_string();
#else
	b->cityName = row[ WorkColCITYNAME.c_str()].data();
#endif
	b->joinYear = (int)row[ WorkColJOINYEAR.c_str()];
	b->joinMonth = (int)row[ WorkColJOINMONTH.c_str()];
	b->quitYear = (int)row[ WorkColQUITYEAR.c_str()];
	b->quitMonth = (int)row[ WorkColQUITMONTH.c_str()];
	b->type = (int)row[ WorkColTYPE.c_str()];
	b->jobTitleId=(int)row[ WorkColJOBTITLEID.c_str()];
	b->positionId=(int)row[ WorkColPOSITIONID.c_str()];
	d->value(b);
	return d;
}
;


//--------------------------------------------------------------------------------


string CollegeInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CollegeColID.c_str())) {
		return boost::lexical_cast<string>(value<CollegeInfoPtr>()->id);
	} else if (!strcasecmp(key.c_str(), CollegeColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<CollegeInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGEID.c_str())) {
		return boost::lexical_cast<string>(value<CollegeInfoPtr>()->collegeId);
	} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGENAME.c_str())) {
		return value<CollegeInfoPtr>()->collegeName;
	} else if (!strcasecmp(key.c_str(), CollegeColDEPARTMENT.c_str())) {
		return value<CollegeInfoPtr>()->department;
	} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGEID.c_str())) {
		return boost::lexical_cast<string>(value<CollegeInfoPtr>()->enrollYear);
	}
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void CollegeInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), CollegeColID.c_str())) {
		value<CollegeInfoPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CollegeColUSERID.c_str())) {
		value<CollegeInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGEID.c_str())) {
		value<CollegeInfoPtr>()->collegeId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGENAME.c_str())) {
		value<CollegeInfoPtr>()->collegeName = val;
	} else if (!strcasecmp(key.c_str(), CollegeColDEPARTMENT.c_str())) {
		value<CollegeInfoPtr>()->department = val;
	} else if (!strcasecmp(key.c_str(), CollegeColENROLLYEAR.c_str())) {
		value<CollegeInfoPtr>()->enrollYear = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap CollegeInfoHandle::getProperties() {
	Str2StrMap result;
	result[CollegeColID] = getProperty(CollegeColID);
	result[CollegeColUSERID] = getProperty(CollegeColUSERID);
	result[CollegeColCOLLEGEID] = getProperty(CollegeColCOLLEGEID);
	result[CollegeColCOLLEGENAME ] = getProperty(CollegeColCOLLEGENAME);
	result[CollegeColDEPARTMENT] = getProperty(CollegeColDEPARTMENT);
	result[CollegeColENROLLYEAR] = getProperty(CollegeColENROLLYEAR);
	return result;
}

void CollegeInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr CollegeInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new CollegeInfoHandle;
	CollegeInfoPtr b = new CollegeInfo();
	b->id = (int)row[ CollegeColID.c_str()];
	b->userId = (int)row[ CollegeColUSERID.c_str()];
	b->collegeId = (int)row[ CollegeColCOLLEGEID.c_str()];
#ifndef NEWARCH
	b->collegeName = row[ CollegeColCOLLEGENAME.c_str()].get_string();
	b->department = row[ CollegeColDEPARTMENT.c_str()].get_string();
#else
	b->collegeName = row[ CollegeColCOLLEGENAME.c_str()].data();
	b->department = row[ CollegeColDEPARTMENT.c_str()].data();
#endif
	b->enrollYear = (int)row[ CollegeColENROLLYEAR.c_str()];
	d->value(b);
	return d;
}
;



//--------------------------------------------------------------------------------


string ElementarySchoolInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), ElemColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<ElementarySchoolInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLID.c_str())) {
		return boost::lexical_cast<string>(value<ElementarySchoolInfoPtr>()->elementarySchoolId);
	} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLNAME.c_str())) {
		return value<ElementarySchoolInfoPtr>()->elementarySchoolName;
	} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLYEAR.c_str())) {
		return boost::lexical_cast<string>(value<ElementarySchoolInfoPtr>()->elementarySchoolYear);
	}
	
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void ElementarySchoolInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), ElemColUSERID.c_str())) {
		value<ElementarySchoolInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLID.c_str())) {
		value<ElementarySchoolInfoPtr>()->elementarySchoolId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLNAME.c_str())) {
		value<ElementarySchoolInfoPtr>()->elementarySchoolName = val;
	} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLYEAR.c_str())) {
		value<ElementarySchoolInfoPtr>()->elementarySchoolYear = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap ElementarySchoolInfoHandle::getProperties() {
	Str2StrMap result;
	result[ElemColUSERID] = getProperty(ElemColUSERID);
	result[ElemColELEMENTARYSCHOOLID] = getProperty(ElemColELEMENTARYSCHOOLID);
	result[ElemColELEMENTARYSCHOOLNAME ] = getProperty(ElemColELEMENTARYSCHOOLNAME);
	result[ElemColELEMENTARYSCHOOLYEAR] = getProperty(ElemColELEMENTARYSCHOOLYEAR);
	return result;
}

void ElementarySchoolInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr ElementarySchoolInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new ElementarySchoolInfoHandle;
	ElementarySchoolInfoPtr b = new ElementarySchoolInfo();
	b->userId = (int)row[ ElemColUSERID.c_str()];
	b->elementarySchoolId = (int)row[ ElemColELEMENTARYSCHOOLID.c_str()];
#ifndef NEWARCH
	b->elementarySchoolName = row[ ElemColELEMENTARYSCHOOLNAME.c_str()].get_string();
#else
	b->elementarySchoolName = row[ ElemColELEMENTARYSCHOOLNAME.c_str()].data();
#endif
	b->elementarySchoolYear = (int)row[ ElemColELEMENTARYSCHOOLYEAR.c_str()];
	d->value(b);
	return d;
}
;



//--------------------------------------------------------------------------------


string JuniorHighSchoolInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), JunHighColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<JuniorHighSchoolInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLID.c_str())) {
		return boost::lexical_cast<string>(value<JuniorHighSchoolInfoPtr>()->juniorHighSchoolId);
	} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLNAME.c_str())) {
		return value<JuniorHighSchoolInfoPtr>()->juniorHighSchoolName;
	} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLYEAR.c_str())) {
		return boost::lexical_cast<string>(value<JuniorHighSchoolInfoPtr>()->juniorHighSchoolYear);
	} 
	
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void JuniorHighSchoolInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), JunHighColUSERID.c_str())) {
		value<JuniorHighSchoolInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLID.c_str())) {
		value<JuniorHighSchoolInfoPtr>()->juniorHighSchoolId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLNAME.c_str())) {
		value<JuniorHighSchoolInfoPtr>()->juniorHighSchoolName = val;
	} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLYEAR.c_str())) {
		value<JuniorHighSchoolInfoPtr>()->juniorHighSchoolYear = boost::lexical_cast<Ice::Int>(val);
	}
}

Str2StrMap JuniorHighSchoolInfoHandle::getProperties() {
	Str2StrMap result;
	result[JunHighColUSERID] = getProperty(JunHighColUSERID);
	result[JunHighColJUNIORHIGHSCHOOLID] = getProperty(JunHighColJUNIORHIGHSCHOOLID);
	result[JunHighColJUNIORHIGHSCHOOLNAME ] = getProperty(JunHighColJUNIORHIGHSCHOOLNAME);
	result[JunHighColJUNIORHIGHSCHOOLYEAR] = getProperty(JunHighColJUNIORHIGHSCHOOLYEAR);
	return result;
}

void JuniorHighSchoolInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr JuniorHighSchoolInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new JuniorHighSchoolInfoHandle;
	JuniorHighSchoolInfoPtr b = new JuniorHighSchoolInfo();
	b->userId = (int)row[ JunHighColUSERID.c_str()];
	b->juniorHighSchoolId = (int)row[ JunHighColJUNIORHIGHSCHOOLID.c_str()];
#ifndef NEWARCH
	b->juniorHighSchoolName = row[ JunHighColJUNIORHIGHSCHOOLNAME.c_str()].get_string();
#else
	b->juniorHighSchoolName = row[ JunHighColJUNIORHIGHSCHOOLNAME.c_str()].data();
#endif
	b->juniorHighSchoolYear = (int)row[ JunHighColJUNIORHIGHSCHOOLYEAR.c_str()];
	d->value(b);
	return d;
}
;


//--------------------------------------------------------------------------------


string RegionInfoHandle::getProperty(const string& key) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), RegionColUSERID.c_str())) {
		return boost::lexical_cast<string>(value<RegionInfoPtr>()->userId);
	} else if (!strcasecmp(key.c_str(), RegionColREGIONID.c_str())) {
		return boost::lexical_cast<string>(value<RegionInfoPtr>()->regionId);
	} else if (!strcasecmp(key.c_str(), RegionColID.c_str())) {
		return boost::lexical_cast<string>(value<RegionInfoPtr>()->id);
	} else if (!strcasecmp(key.c_str(), RegionColPROVINCENAME.c_str())) {
		return value<RegionInfoPtr>()->provinceName;
	} else if (!strcasecmp(key.c_str(), RegionColCITYNAME.c_str())) {
		return boost::lexical_cast<string>(value<RegionInfoPtr>()->cityName);
	} 
	
	NoSuchPropertyException e;
	e.message = key;
	throw e;
}

void RegionInfoHandle::setProperty(const string& key, const string& val) {
	IceUtil::Mutex::Lock lock(*this);
	if (!strcasecmp(key.c_str(), RegionColID.c_str())) {
		value<RegionInfoPtr>()->id = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), RegionColUSERID.c_str())) {
		value<RegionInfoPtr>()->userId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), RegionColREGIONID.c_str())) {
		value<RegionInfoPtr>()->regionId = boost::lexical_cast<Ice::Int>(val);
	} else if (!strcasecmp(key.c_str(), RegionColPROVINCENAME.c_str())) {
		value<RegionInfoPtr>()->provinceName = val;
	} else if (!strcasecmp(key.c_str(), RegionColCITYNAME.c_str())) {
		value<RegionInfoPtr>()->cityName = val;
	}
}

Str2StrMap RegionInfoHandle::getProperties() {
	Str2StrMap result;
	result[RegionColID] = getProperty(RegionColID);
	result[RegionColUSERID] = getProperty(RegionColUSERID);
	result[RegionColREGIONID] = getProperty(RegionColREGIONID);
	result[RegionColPROVINCENAME] = getProperty(RegionColPROVINCENAME);
	result[RegionColCITYNAME] = getProperty(RegionColCITYNAME);
	return result;
}

void RegionInfoHandle::setProperties(const Str2StrMap& properties) {
	Str2StrMap::const_iterator it;
	for (it = properties.begin(); it != properties.end(); ++it) {
		setProperty(it->first, it->second);
	}
}

DataHandlePtr RegionInfoHandle::parse(mysqlpp::Row& row) {
	DataHandlePtr d = new RegionInfoHandle;
	RegionInfoPtr b = new RegionInfo();
	b->id = (int)row[ RegionColID.c_str()];
	b->userId = (int)row[ RegionColUSERID.c_str()];
	b->regionId = (int)row[ RegionColREGIONID.c_str()];
#ifndef NEWARCH
	b->provinceName = row[ RegionColPROVINCENAME.c_str()].get_string();
	b->cityName = row[ RegionColCITYNAME.c_str()].get_string();
#else
	b->provinceName = row[ RegionColPROVINCENAME.c_str()].data();
	b->cityName = row[ RegionColCITYNAME.c_str()].data();
#endif
	d->value(b);
	return d;
}
;
