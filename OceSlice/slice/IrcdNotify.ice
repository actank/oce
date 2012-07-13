#ifndef __IRCD_NOTIFY__
#define __IRCD_NOTIFY__
module mop
{
module hi 
{
module svc
{
module ircd 
{
    class ClassMemberChange
    {  
        int classId;    
        int memId;  
        int stat;
    };
    sequence<ClassMemberChange> ClassMemberChanges;
    dictionary<int , ClassMemberChange> ClassMemberChangeMap;
    class IrcdNotifyManager
    { 
        void ClassMemberChangeNotify(ClassMemberChanges changes);
    };
};
};
};
};


#endif//__IRCD_NOTIFY__
