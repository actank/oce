#ifndef USERED_CLASS_ICE
#define USERED_CLASS_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	const int ADMINTYPE = 1;
	const int FOUNDERTYPE = -1;
    const int MEMBERTYPE = 0;
    const int APPLYTYPE = 10;
    const int BLOCKTYPE = 4;
	
    const int HIGHSCHOOLTYPE = 0;
    const int UNIVTYPE = 1;
    
	class UseredClassSet 
    {
    	void add(int schoolType,int classId , int role);
    	void remove(int schoolType, int classId);
    	
    	int size(int schoolType, MyUtil::IntSeq roles );
    	MyUtil::IntSeq getAll(int schoolType, MyUtil::IntSeq roles, int begin, int limit);
        
        
    };
    
    class UseredClassManager
    {
       	int getAllClassCount(int userId,int schoolType);
       	int getAdminedClassCount(int userId, int schoolType);
       	int getMemberedClassCount(int userId, int schoolType);       	
        int getFoundedClassCount(int userId, int schoolType); 
        
		MyUtil::IntSeq getAllClasses(int userId, int schoolType , int offset, int limit);
		MyUtil::IntSeq getAdminedClasses(int userId, int schoolType, int offset, int limit);
		MyUtil::IntSeq getMemberedClasses(int userId, int schoolType,  int offset, int limit);
        MyUtil::IntSeq getFoundedClasses(int userId, int schoolType, int offset, int limit);
        
        MyUtil::Str2StrMapSeq getAllClassInfo(int userId, int schoolType , int offset, int limit);
        MyUtil::Str2StrMapSeq getAdminedClassInfo(int userId, int schoolType , int offset, int limit);
        MyUtil::Str2StrMapSeq getMemberedClassInfo(int userId, int schoolType , int offset, int limit);
        MyUtil::Str2StrMapSeq getFoundedClassInfo(int userId, int schoolType , int offset, int limit);
        
        MyUtil::IntSeq getDispatchClasses(int userId);
            
		void addMemberedClass(int userId, int schoolType, int classId);
		void removeMemberedClass(int userId, int schoolType, int classId);
		void addAdminedClass(int userId, int schoolType ,int classId);
		void removeAdminedClass(int userId, int schoolType, int classId);
        void addFoundedClass(int userId, int schoolType, int classId);
        void removeFoundedClass(int userId, int schoolType, int classId);
        
     //   void setClassInfo(int classId, MyUtil::Str2StrMap props);
        MyUtil::Str2StrMap getClassInfo(int classId);
       // void removeClassInfo(int classId);
	};

};
};
};
};
#endif
