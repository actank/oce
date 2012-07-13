#ifndef _USER_NETWORK_ICE
#define _USER_NETWORK_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module mop
{
module hi
{
module svc
{
module model
{	
	const string NetColID = "ID";
	const string NetColUSERID = "USERID";
	const string NetColSTAGE = "STAGE";
	const string NetColINFOID = "INFO_ID";
	const string NetColJOINTIME = "JOIN_TIME";
	const string NetColSTATUS = "STATUS";
	const string NetColNetworkName = "NETWORK_NAME";
	const string NetColNetworkId = "NETWORK_ID";
	//const string NetColINFOTYPE = "INFO_TYPE";

	const string HighColID = "ID";
	const string HighColUSERID = "USERID";
	const string HighColHighSchoolID = "HIGH_SCHOOL_ID";
	const string HighColHighSchoolNAME = "HIGH_SCHOOL_NAME";
	const string HighColENROLLYEAR = "ENROLL_YEAR";
	const string HighColHCLASS1 = "H_CLASS1";
	const string HighColHCLASS2 = "H_CLASS2";
	const string HighColHCLASS3 = "H_CLASS3";

	const string UniColID = "ID";
	const string UniColUSERID = "USERID";
	const string UniColUNIVERSITYID = "UNIVERSITY_ID";
	const string UniColUNIVERSITYNAME = "UNIVERSITY_NAME";
	const string UniColDORM = "DORM";
	const string UniColDEPARTMENT = "DEPARTMENT";
	const string UniColTYPEOFCOURSE = "TYPE_OF_COURSE";
	const string UniColTYPEENROLLYEAR = "ENROLL_YEAR";

	const string WorkColID = "ID";
	const string WorkColUSERID = "USERID";
	const string WorkColWORKPLACEID = "WORKPLACE_ID";
	const string WorkColWORKPLACENAME = "WORKPLACE_NAME";
	const string WorkColADDRESS = "ADDRESS";
	const string WorkColDESCRIPTION = "DESCRIPTION";
	const string WorkColPROVINCE = "PROVINCE";
	const string WorkColCITYID = "CITY_ID";
	const string WorkColCITYNAME = "CITY_NAME";
	const string WorkColJOINYEAR = "JOIN_YEAR";
	const string WorkColJOINMONTH = "JOIN_MONTH";
	const string WorkColQUITYEAR = "QUIT_YEAR";
	const string WorkColQUITMONTH = "QUIT_MONTH";
	const string WorkColTYPE = "TYPE";
	const string WorkColJOBTITLEID = "JOB_TITLE_ID";
	const string WorkColPOSITIONID = "POSITION_ID";

	const string CollegeColID = "ID";
	const string CollegeColUSERID = "USERID";
	const string CollegeColCOLLEGEID = "COLLEGE_ID";
	const string CollegeColCOLLEGENAME = "COLLEGE_NAME";
	const string CollegeColDEPARTMENT = "DEPARTMENT";
	const string CollegeColENROLLYEAR = "ENROLL_YEAR";

	const string ElemColID = "ID";
	const string ElemColUSERID = "USERID";
	const string ElemColELEMENTARYSCHOOLID = "ELEMENTARY_SCHOOL_ID";
	const string ElemColELEMENTARYSCHOOLNAME = "ELEMENTARY_SCHOOL_NAME";
	const string ElemColELEMENTARYSCHOOLYEAR = "ELEMENTARY_SCHOOL_YEAR";

	const string JunHighColID = "ID";
	const string JunHighColUSERID = "USERID";
	const string JunHighColJUNIORHIGHSCHOOLID = "JUNIOR_HIGH_SCHOOL_ID";
	const string JunHighColJUNIORHIGHSCHOOLNAME = "JUNIOR_HIGH_SCHOOL_NAME";
	const string JunHighColJUNIORHIGHSCHOOLYEAR = "JUNIOR_HIGH_SCHOOL_YEAR";

	const string RegionColID = "ID";
	const string RegionColUSERID = "USERID";
	const string RegionColREGIONID = "REGION_ID";
	const string RegionColPROVINCENAME = "PROVINCE_NAME";
	const string RegionColCITYNAME = "CITY_NAME";

	exception NetworkException extends MyUtil::MceException {};
	exception HighSchoolInfoException extends MyUtil::MceException {};
	exception UniversityInfoException extends MyUtil::MceException {};
	exception WorkplaceInfoException extends MyUtil::MceException {};

	class Network
	{
		int id;
		int userId;
		int stage;
		int infoId;
		string joinTime;
		int status;
		string networkName;
		int infoType;
		int networkId;
	};
	sequence<Network> NetworkSeq;
	dictionary<int, Network> NetworkMap;

	class HighSchoolInfo
	{
		int id;
		int userId;
		int HighSchoolId;
		string HighSchoolName;
		int enrollYear;
		string hClass1;
		string hClass2;
		string hClass3;
	};
	sequence<HighSchoolInfo> HighSchoolInfoSeq;
	dictionary<int, HighSchoolInfo> HighSchoolInfoMap;

	class UniversityInfo
	{
		int id;
		int userId;
		int universityId;
		string universityName;
		string dorm;
		string department;
		string typeOfCourse;
		int enrollYear;
	};
	sequence<UniversityInfo> UniversityInfoSeq;
	dictionary<int, UniversityInfo> UniversityInfoMap;

	class WorkspaceInfo
	{
		int id;
		int userId;
		int workplaceId;
		string workplaceName;
		string address;
		string description;
		string province;
		int cityId;
		string cityName;
		int joinYear;
		int joinMonth;
		int quitYear;
		int quitMonth;
		int type;
		int jobTitleId;
		int positionId;
	};

	sequence<WorkspaceInfo> WorkspaceInfoSeq;
	dictionary<int, WorkspaceInfo> WorkspaceInfoMap;

	class WorkplaceInfo extends WorkspaceInfo {};
	sequence<WorkplaceInfo> WorkplaceInfoSeq;
	dictionary<int, WorkplaceInfo> WorkplaceInfoMap;

	class CollegeInfo
	{
		int id;
		int userId;
		int collegeId;
		string collegeName;
		string department;
		int enrollYear;
	};
	sequence<CollegeInfo> CollegeInfoSeq;
	dictionary<int, CollegeInfo> CollegeInfoMap;

	class ElementarySchoolInfo
	{
		int userId;
		int elementarySchoolId;
		string elementarySchoolName;
		int elementarySchoolYear;
	};
	class ElementarySchoolInfoN extends ElementarySchoolInfo
	{
		int id;
	};

	sequence<ElementarySchoolInfo> ElementarySchoolInfoSeq;
	dictionary<int, ElementarySchoolInfo> ElementarySchoolInfoMap;


	class JuniorHighSchoolInfo
	{
		int userId;
		int juniorHighSchoolId;
		string juniorHighSchoolName;
		int juniorHighSchoolYear;
	};
	class JuniorHighSchoolInfoN extends JuniorHighSchoolInfo
	{
		int id;
	};
	sequence<JuniorHighSchoolInfo> JuniorHighSchoolInfoSeq;
	dictionary<int, JuniorHighSchoolInfo> JuniorHighSchoolInfoMap;


	class RegionInfo
	{
		int id;
		int userId;
		int regionId;
		string provinceName;
		string cityName;
	};
	sequence<RegionInfo> RegionInfoSeq;
	dictionary<int, RegionInfo> RegionInfoMap;

	class NetworkBigObject
	{
		NetworkSeq networks;
		HighSchoolInfoSeq highschools;
		UniversityInfoSeq universitys;
		WorkspaceInfoSeq workspaces;
	};
	class NetworkBigObjectN
	{
		NetworkSeq networks;
		HighSchoolInfoSeq highschools;
		UniversityInfoSeq universitys;
		WorkspaceInfoSeq workspaces;
		CollegeInfoSeq collages;
		ElementarySchoolInfo elementary;
		JuniorHighSchoolInfo juniorhighschool;
	};

	class NetworkData
	{
		int userId;
		NetworkSeq              networks;
		HighSchoolInfoSeq       highschools;
		UniversityInfoSeq       universities;
		WorkspaceInfoSeq        workspaces;
		CollegeInfoSeq          colleges;
		ElementarySchoolInfoSeq elementaries;
		JuniorHighSchoolInfoSeq juniorhighschools;
		RegionInfoSeq           regions;
	};

	class NetworkManager
	{
		void reloadNetworkSeq(int userId);
		Network getNetwork(int userId, int id);
		Network getNetworkByNetId(int userId, int networkId);
		NetworkSeq getNetworkSeq(int userId);
		NetworkSeq getNetworkSeqByStage(int userId, int stage);
		NetworkSeq getNetworkSeqByStatus(int userId, int status);
		NetworkSeq getNetworkSeqByStageAndStatus(int userId, int stage, MyUtil::IntSeq status);
		NetworkSeq getNetworkSeqByStatuses(int userId,  MyUtil::IntSeq status);

		Network getRegionalNetwork(int userId);

		int addNetwork(int userId, MyUtil::Str2StrMap network);
		void removeNetwork(int userId, int networkId);
		void removeNetworkByStage(int userId, int stage);
		void setNetwork(int userId, int networkId, MyUtil::Str2StrMap network);
		int getNetworkCountByStage(int userId, int stage);
		int getValidNetworkCountByStage(int userId, int stage);
		bool isExistInNetwork(int  userid, int networkid );


		void reloadHighSchoolInfoSeq(int userId);
		HighSchoolInfo getHighSchoolInfo(int userId, int id);
		HighSchoolInfo getHighSchoolInfoBySchoolId(int userId, int id);
		HighSchoolInfoSeq getHighSchoolInfoSeq(int userId);
		int addHighSchoolInfo(int userId, MyUtil::Str2StrMap HighSchoolInfo);
		void removeHighSchoolInfo(int userId, int id);
		void removeAllHighSchoolInfo(int userId);
		void setHighSchoolInfo(int userId,int id, MyUtil::Str2StrMap highSchoolInfo);
		int getHighSchoolInfoCount(int userId);

		void reloadUniversityInfoSeq(int userId);
		UniversityInfo getUniversityInfo(int userId, int id);
		UniversityInfo getUniversityInfoByUnivId(int userId, int id);
		UniversityInfoSeq getUniversityInfoSeq(int userId);
		int addUniversityInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeUniversityInfo(int userId, int id);
		void removeUniversityInfoByUnivId(int userId, int universityId);
		void removeAllUniversityInfo(int userId);
		void setUniversityInfo(int userId,int id, MyUtil::Str2StrMap universityInfo);
		int getUniversityInfoCount(int userId);

		void reloadWorkplaceInfoSeq(int userId);
		WorkplaceInfo getWorkplaceInfo(int userId, int id);
		WorkplaceInfo getWorkplaceInfoByWorkId(int userId, int id);
		WorkplaceInfoSeq getWorkplaceInfoSeq(int userId);
		int addWorkplaceInfo(int userId, MyUtil::Str2StrMap workplaceInfo);
		void removeWorkplaceInfo(int userId, int id);
		void removeWorkplaceInfoByWorkId(int userId, int workplaceId);
		void removeAllWorkplaceInfo(int userId);
		void setWorkplaceInfo(int userId,int id, MyUtil::Str2StrMap workplaceInfo);
		int getWorkplaceInfoCount(int userId);

		WorkspaceInfo getWorkspaceInfo(int userId, int id);
		WorkspaceInfo getWorkspaceInfoByWorkId(int userId, int id);
		WorkspaceInfoSeq getWorkspaceInfoSeq(int userId);
		int addWorkspaceInfo(int userId, MyUtil::Str2StrMap workspaceInfo);
		void removeWorkspaceInfo(int userId, int id);
		void setWorkspaceInfo(int userId,int id, MyUtil::Str2StrMap workspaceInfo);

		void reloadCollegeInfoSeq(int userId);
		CollegeInfo getCollegeInfo(int userId, int id);
		CollegeInfo getCollegeInfoByCollegeId(int userId, int id);
		CollegeInfoSeq getCollegeInfoSeq(int userId);
		int addCollegeInfo(int userId, MyUtil::Str2StrMap collegeInfo);
		void removeCollegeInfo(int userId, int id);
		void removeAllCollegeInfo(int userId);
		void setCollegeInfo(int userId,int id, MyUtil::Str2StrMap collegeInfo);
		int getCollegeInfoCount(int userId);
	
		void reloadElementarySchoolInfo(int userId);
		ElementarySchoolInfoSeq getElementarySchoolInfoSeq(int userId);
		void removeAllElementarySchoolInfo(int userId);
		void removeElementarySchoolInfoById(int userId, int id);
		ElementarySchoolInfo getElementarySchoolInfoBySchoolId(int userId, int id);
		ElementarySchoolInfo getElementarySchoolInfoById(int userId, int id);
		void setElementarySchoolInfo(int userId, int id, MyUtil::Str2StrMap elementaryInfo);
		ElementarySchoolInfo getElementarySchoolInfo(int userId);
		int addElementarySchoolInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeElementarySchoolInfo(int userId);
		
		void reloadJuniorHighSchoolInfo(int userId);
		JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId);
		int addJuniorHighSchoolInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeJuniorHighSchoolInfo(int userId);
		JuniorHighSchoolInfo getJuniorHighSchoolInfoBySchoolId(int userId, int id);
		JuniorHighSchoolInfo getJuniorHighSchoolInfoById(int userId, int id);
		JuniorHighSchoolInfoSeq getJuniorHighSchoolInfoSeq(int userId);
		void removeJuniorHighSchoolInfoById(int userId, int id);
		void removeAllJuniorHighSchoolInfo(int userId);
		void setJuniorHighSchoolInfo(int userId, int id, MyUtil::Str2StrMap juniorschoolInfo);
	
		void reloadRegionInfo(int userId);
		RegionInfo getRegionInfo(int userId, int regionId);
		RegionInfoSeq getRegionInfoSeq(int userId);
		int addRegionInfo(int userId, MyUtil::Str2StrMap regionInfo);
		void removeRegionInfo(int userId, int regionId);
		void removeAllRegionInfo(int userId);
		void setRegionInfo(int userId, int regionId, MyUtil::Str2StrMap regionInfo);
	
		NetworkBigObject getBigObject(int userId);
		NetworkBigObjectN getBigObjectN(int userId);
		NetworkData getNetworkFullData(int userId);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
	};

	class NetworkReader
	{
//		void reloadNetworkSeq(int userId);
		Network getNetwork(int userId, int id);
		Network getNetworkByNetId(int userId, int networkId);
		NetworkSeq getNetworkSeq(int userId);
		NetworkSeq getNetworkSeqByStage(int userId, int stage);
		NetworkSeq getNetworkSeqByStatus(int userId, int status);
		NetworkSeq getNetworkSeqByStageAndStatus(int userId, int stage, MyUtil::IntSeq status);
		NetworkSeq getNetworkSeqByStatuses(int userId,  MyUtil::IntSeq status);

		Network getRegionalNetwork(int userId);

		void addNetwork(int userId, MyUtil::Str2StrMap network);
		void removeNetwork(int userId, int networkId);
		void removeNetworkByStage(int userId, int stage);
		void setNetwork(int userId, int networkId, MyUtil::Str2StrMap network);
		int getNetworkCountByStage(int userId, int stage);
		int getValidNetworkCountByStage(int userId, int stage);
		bool isExistInNetwork(int  userid, int networkid );


//		void reloadHighSchoolInfoSeq(int userId);
		HighSchoolInfo getHighSchoolInfo(int userId, int id);
		HighSchoolInfo getHighSchoolInfoBySchoolId(int userId, int id);
		HighSchoolInfoSeq getHighSchoolInfoSeq(int userId);
		void addHighSchoolInfo(int userId, MyUtil::Str2StrMap HighSchoolInfo);
		void removeHighSchoolInfo(int userId, int id);
		void removeAllHighSchoolInfo(int userId);
		void setHighSchoolInfo(int userId,int id, MyUtil::Str2StrMap highSchoolInfo);
		int getHighSchoolInfoCount(int userId);

//		void reloadUniversityInfoSeq(int userId);
		UniversityInfo getUniversityInfo(int userId, int id);
		UniversityInfo getUniversityInfoByUnivId(int userId, int id);
		UniversityInfoSeq getUniversityInfoSeq(int userId);
		void addUniversityInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeUniversityInfo(int userId, int id);
		void removeUniversityInfoByUnivId(int userId, int universityId);
		void removeAllUniversityInfo(int userId);
		void setUniversityInfo(int userId,int id, MyUtil::Str2StrMap universityInfo);
		int getUniversityInfoCount(int userId);

//		void reloadWorkplaceInfoSeq(int userId);
		WorkplaceInfo getWorkplaceInfo(int userId, int id);
		WorkplaceInfo getWorkplaceInfoByWorkId(int userId, int id);
		WorkplaceInfoSeq getWorkplaceInfoSeq(int userId);
		void addWorkplaceInfo(int userId, MyUtil::Str2StrMap workplaceInfo);
		void removeWorkplaceInfo(int userId, int id);
		void removeWorkplaceInfoByWorkId(int userId, int workplaceId);
		void removeAllWorkplaceInfo(int userId);
		void setWorkplaceInfo(int userId,int id, MyUtil::Str2StrMap workplaceInfo);
		int getWorkplaceInfoCount(int userId);

		WorkspaceInfo getWorkspaceInfo(int userId, int id);
		WorkspaceInfo getWorkspaceInfoByWorkId(int userId, int id);
		WorkspaceInfoSeq getWorkspaceInfoSeq(int userId);
		void addWorkspaceInfo(int userId, MyUtil::Str2StrMap workspaceInfo);
		void removeWorkspaceInfo(int userId, int id);
		void setWorkspaceInfo(int userId,int id, MyUtil::Str2StrMap workspaceInfo);

//		void reloadCollegeInfoSeq(int userId);
		CollegeInfo getCollegeInfo(int userId, int id);
		CollegeInfo getCollegeInfoByCollegeId(int userId, int id);
		CollegeInfoSeq getCollegeInfoSeq(int userId);
		void addCollegeInfo(int userId, MyUtil::Str2StrMap collegeInfo);
		void removeCollegeInfo(int userId, int id);
		void removeAllCollegeInfo(int userId);
		void setCollegeInfo(int userId,int id, MyUtil::Str2StrMap collegeInfo);
		int getCollegeInfoCount(int userId);

//		void reloadElementarySchoolInfo(int userId);
		ElementarySchoolInfoSeq getElementarySchoolInfoSeq(int userId);
		void removeAllElementarySchoolInfo(int userId);
		void removeElementarySchoolInfoById(int userId, int id);
		ElementarySchoolInfo getElementarySchoolInfoBySchoolId(int userId, int id);
		ElementarySchoolInfo getElementarySchoolInfoById(int userId, int id);
		void setElementarySchoolInfo(int userId, int id, MyUtil::Str2StrMap elementaryInfo);
		ElementarySchoolInfo getElementarySchoolInfo(int userId);
		void addElementarySchoolInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeElementarySchoolInfo(int userId);

//		void reloadJuniorHighSchoolInfo(int userId);
		JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId);
		void addJuniorHighSchoolInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeJuniorHighSchoolInfo(int userId);
		JuniorHighSchoolInfo getJuniorHighSchoolInfoBySchoolId(int userId, int id);
		JuniorHighSchoolInfo getJuniorHighSchoolInfoById(int userId, int id);
		JuniorHighSchoolInfoSeq getJuniorHighSchoolInfoSeq(int userId);
		void removeJuniorHighSchoolInfoById(int userId, int id);
		void removeAllJuniorHighSchoolInfo(int userId);
		void setJuniorHighSchoolInfo(int userId, int id, MyUtil::Str2StrMap juniorschoolInfo);

//		void reloadRegionInfo(int userId);
		RegionInfo getRegionInfo(int userId, int regionId);
		RegionInfoSeq getRegionInfoSeq(int userId);
		void addRegionInfo(int userId, MyUtil::Str2StrMap regionInfo);
		void removeRegionInfo(int userId, int regionId);
		void removeAllRegionInfo(int userId);
		void setRegionInfo(int userId, int regionId, MyUtil::Str2StrMap regionInfo);

		NetworkBigObject getBigObject(int userId);
		NetworkBigObjectN getBigObjectN(int userId);
		NetworkData getNetworkFullData(int userId);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
	};

	class NetworkWriter
	{
		void reloadNetworkSeq(int userId);
//		Network getNetwork(int userId, int id);
//		Network getNetworkByNetId(int userId, int networkId);
//		NetworkSeq getNetworkSeq(int userId);
//		NetworkSeq getNetworkSeqByStage(int userId, int stage);
//		NetworkSeq getNetworkSeqByStatus(int userId, int status);
//		NetworkSeq getNetworkSeqByStageAndStatus(int userId, int stage, MyUtil::IntSeq status);
//		NetworkSeq getNetworkSeqByStatuses(int userId,  MyUtil::IntSeq status);
//
//		Network getRegionalNetwork(int userId);

		int addNetwork(int userId, MyUtil::Str2StrMap network);
		void removeNetwork(int userId, int networkId);
		void removeNetworkByStage(int userId, int stage);
		void setNetwork(int userId, int networkId, MyUtil::Str2StrMap network);
//		int getNetworkCountByStage(int userId, int stage);
//		int getValidNetworkCountByStage(int userId, int stage);
//		bool isExistInNetwork(int  userid, int networkid );


		void reloadHighSchoolInfoSeq(int userId);
//		HighSchoolInfo getHighSchoolInfo(int userId, int id);
//		HighSchoolInfo getHighSchoolInfoBySchoolId(int userId, int id);
//		HighSchoolInfoSeq getHighSchoolInfoSeq(int userId);
		int addHighSchoolInfo(int userId, MyUtil::Str2StrMap HighSchoolInfo);
		void removeHighSchoolInfo(int userId, int id);
		void removeAllHighSchoolInfo(int userId);
		void setHighSchoolInfo(int userId,int id, MyUtil::Str2StrMap highSchoolInfo);
//		int getHighSchoolInfoCount(int userId);

		void reloadUniversityInfoSeq(int userId);
//		UniversityInfo getUniversityInfo(int userId, int id);
//		UniversityInfo getUniversityInfoByUnivId(int userId, int id);
//		UniversityInfoSeq getUniversityInfoSeq(int userId);
		int addUniversityInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeUniversityInfo(int userId, int id);
		void removeUniversityInfoByUnivId(int userId, int universityId);
		void removeAllUniversityInfo(int userId);
		void setUniversityInfo(int userId,int id, MyUtil::Str2StrMap universityInfo);
//		int getUniversityInfoCount(int userId);

		void reloadWorkplaceInfoSeq(int userId);
//		WorkplaceInfo getWorkplaceInfo(int userId, int id);
//		WorkplaceInfo getWorkplaceInfoByWorkId(int userId, int id);
//		WorkplaceInfoSeq getWorkplaceInfoSeq(int userId);
		int addWorkplaceInfo(int userId, MyUtil::Str2StrMap workplaceInfo);
		void removeWorkplaceInfo(int userId, int id);
		void removeWorkplaceInfoByWorkId(int userId, int workplaceId);
		void removeAllWorkplaceInfo(int userId);
		void setWorkplaceInfo(int userId,int id, MyUtil::Str2StrMap workplaceInfo);
//		int getWorkplaceInfoCount(int userId);

//		WorkspaceInfo getWorkspaceInfo(int userId, int id);
//		WorkspaceInfo getWorkspaceInfoByWorkId(int userId, int id);
//		WorkspaceInfoSeq getWorkspaceInfoSeq(int userId);
		int addWorkspaceInfo(int userId, MyUtil::Str2StrMap workspaceInfo);
		void removeWorkspaceInfo(int userId, int id);
		void setWorkspaceInfo(int userId,int id, MyUtil::Str2StrMap workspaceInfo);

		void reloadCollegeInfoSeq(int userId);
//		CollegeInfo getCollegeInfo(int userId, int id);
//		CollegeInfo getCollegeInfoByCollegeId(int userId, int id);
//		CollegeInfoSeq getCollegeInfoSeq(int userId);
		int addCollegeInfo(int userId, MyUtil::Str2StrMap collegeInfo);
		void removeCollegeInfo(int userId, int id);
		void removeAllCollegeInfo(int userId);
		void setCollegeInfo(int userId,int id, MyUtil::Str2StrMap collegeInfo);
//		int getCollegeInfoCount(int userId);

		void reloadElementarySchoolInfo(int userId);
//		ElementarySchoolInfoSeq getElementarySchoolInfoSeq(int userId);
		void removeAllElementarySchoolInfo(int userId);
		void removeElementarySchoolInfoById(int userId, int id);
//		ElementarySchoolInfo getElementarySchoolInfoBySchoolId(int userId, int id);
//		ElementarySchoolInfo getElementarySchoolInfoById(int userId, int id);
		void setElementarySchoolInfo(int userId, int id, MyUtil::Str2StrMap elementaryInfo);
//		ElementarySchoolInfo getElementarySchoolInfo(int userId);
		int addElementarySchoolInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeElementarySchoolInfo(int userId);

		void reloadJuniorHighSchoolInfo(int userId);
//		JuniorHighSchoolInfo getJuniorHighSchoolInfo(int userId);
		int addJuniorHighSchoolInfo(int userId, MyUtil::Str2StrMap universityInfo);
		void removeJuniorHighSchoolInfo(int userId);
//		JuniorHighSchoolInfo getJuniorHighSchoolInfoBySchoolId(int userId, int id);
//		JuniorHighSchoolInfo getJuniorHighSchoolInfoById(int userId, int id);
//		JuniorHighSchoolInfoSeq getJuniorHighSchoolInfoSeq(int userId);
		void removeJuniorHighSchoolInfoById(int userId, int id);
		void removeAllJuniorHighSchoolInfo(int userId);
		void setJuniorHighSchoolInfo(int userId, int id, MyUtil::Str2StrMap juniorschoolInfo);

		void reloadRegionInfo(int userId);
//		RegionInfo getRegionInfo(int userId, int regionId);
//		RegionInfoSeq getRegionInfoSeq(int userId);
		int addRegionInfo(int userId, MyUtil::Str2StrMap regionInfo);
		void removeRegionInfo(int userId, int regionId);
		void removeAllRegionInfo(int userId);
		void setRegionInfo(int userId, int regionId, MyUtil::Str2StrMap regionInfo);

//		NetworkBigObject getBigObject(int userId);
//		NetworkBigObjectN getBigObjectN(int userId);
//		NetworkData getNetworkFullData(int userId);
		void setValid(bool newState);
//		void setData(MyUtil::ObjectResult userData);

		//加载详细信息
		void reloadAll(int userId);
	};

};
};
};
};
#endif

