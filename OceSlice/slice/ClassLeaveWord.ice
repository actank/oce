#ifndef __CLASSLEAVEWORD_ICE__
#define __CLASSLEAVEWORD_ICE__

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module classleaveword
{
	exception DispatchException extends MyUtil::MceException{};
    
    class Note
    {
        long    memberId;
        string 	memberName;
        string 	tinyUrl;
        string 	body;
        string  subjectTime;
        int     subjectType;
        string  subjectLink;
    };
    sequence<Note> NoteSeq;
    
    
    class NoteWrapper
    {
        Note   getNote();
        
        Note   theNote;
    };      
            
    class Dispatcher
    {
    	void dispatch(Note theNote);
    };
    
   
};
};
};
};
#endif
