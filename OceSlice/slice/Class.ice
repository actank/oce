#ifndef CLASS_ICE
#define CLASS_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	exception ClassException extends MyUtil::MceException{};
    exception NoSuchClassException extends ClassException{};
    exception InvalidClassException extends ClassException{};
     
    class ClassCount
    {
        int count;
        void incCount();
        void decCount();
        int getCount();
    };

	const string SCID             = "ID";
	const string SCFOUNDTIME      = "FOUNDTIME";
	const string SCFOUNDER        = "FOUNDER";
	const string SCFOUNDERNAME    = "FOUNDER_NAME";
	const string SCMEMBERCOUNT    = "MEMBER_COUNT";
	const string SCSCHOOLFLAG     = "SCHOOLFLAG";
	const string SCCLASSYEAR      = "CLASSYEAR";
	const string SCCLASSNAME      = "CLASSNAME";
	const string SCCLASSURL       = "CLASSURL";
	const string SCISPUBLIC       = "ISPUBLIC";
	const string SCSPACELEFT      = "SPACELEFT";
	const string SCBEAUTIFY       = "BEAUTIFY";
	const string SCBULLETIN       = "BULLETIN";
	const string SCPROVINCEID     = "PROVINCE_ID";
	const string SCPROVINCENAME   = "PROVINCE_NAME";
	const string SCCITYUNIVID     = "CITY_UNIV_ID";
	const string SCCITYUNIVNAME   = "CITY_UNIV_NAME";
	const string SCSCHOOLDEPTID   = "SCHOOL_DEPT_ID";
	const string SCSCHOOLDEPTNAME = "SCHOOL_DEPT_NAME";
	const string SCFOUNDERTINYURL = "FOUNDER_TINYURL";
	const string SCPHOTOCOUNT     = "PHOTO_COUNT";
	const string SCSPACESIZE      = "SPACESIZE";
	const string SCSPACEUSED      = "SPACEUSED";
	
	class SchoolClass
	{
		int id;
		string foundtime;
		int founder;
		string founderName;
		int memberCount;
		int schoolflag;
		string classyear;
		string classname;
		string classurl;
		int ispublic;
		int spaceleft;
		string beautify;
		string bulletin;
		string provinceId;
		string provinceName;
		string cityUnivId;
		string cityUnivName;
		string schoolDeptId;
		string schoolDeptName;
		string founderTinyurl;
		int photoCount;
		int spacesize;
		int spaceused;
	};

	const string CMID         = "ID";
	const string CMCLASSID    = "CLASSID";
	const string CMMEMBERID   = "MEMBERID";
	const string CMMEMBERNAME = "MEMBERNAME";
	const string CMSCHOOLFLAG = "SCHOOLFLAG";
	const string CMROLE       = "ROLE";
	const string CMLOGONTIME  = "LOGONTIME";
	const string CMLOGONCOUNT = "LOGON_COUNT";
	const string CMTINYURL    = "TINYURL";
	
	class ClassMember
	{
		int id;
		int classid;
		int memberid;
		string membername;
		int schoolflag;
		int role;
		string logontime;
		int logonCount;
		string tinyurl;
	};
	
    class ClassManager
    {
    	void incClassCount(string schoolId);
        void decClassCount(string schoolId);
        int getClassCount(string schoolId);
        MyUtil::IntSeq getClassesCounts(MyUtil::StrSeq schoolIds);
        

        
        int  createSchoolClass(MyUtil::Str2StrMap properties); 
	    void removeSchoolClass(int classId);

        MyUtil::Str2StrMap getSchoolClass(int classId) throws NoSuchClassException;
        MyUtil::Str2StrMapSeq getSchoolClasses(MyUtil::IntSeq classIds);
        void setSchoolClass(int classId, MyUtil::Str2StrMap properties);
    
        int getMemberCount(int classId);
        void incMemberCount(int classId);
        void decMemberCount(int classId);
		// Member
    	void addMemberApply(int classId , int userId, MyUtil::Str2StrMap mem);
        void approveApply(int classId, int userId);
        void cancelApply(int classId, int userId);
        void addRegularMember(int classId, int userId, MyUtil::Str2StrMap mem);
        void removeRegularMember(int classId, int userId);
        void setBlock(int classId, int userId);
        void cancelSetBlock(int classId, int userId);
        void chanageMemberInfo(int classId, int userId, MyUtil::Str2StrMap mem);
        void setAdmin(int classId , int userId);
        void cancelAdmin(int classId, int userId);        
        
        
        //inside interface
        void addMember(int classId, int userId, MyUtil::Str2StrMap mem);
		void removeMember(int classId, int userId);
		void setMember(int classId, int userId, MyUtil::Str2StrMap properties);
   
        
		//int getMemberCount(int classId, MyUtil::Str2StrMap filter) throws NoSuchClassException;
		MyUtil::Str2StrMap getMember(int classId, int userId);        
		MyUtil::Str2StrMapSeq getMembers(int classId, MyUtil::IntSeq memberIds);
		MyUtil::Str2StrMapSeq getMatchedMembers(int classId, MyUtil::Str2StrMap filter, int offset, int limit);
        MyUtil::IntSeq getMatchedMemberIds(int classId, MyUtil::Str2StrMap filter, int offset, int limit);
        int getMatchedCount(int classId, MyUtil::Str2StrMap filter);

	
    };
};
};
};
};
#endif

