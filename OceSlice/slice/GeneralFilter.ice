#ifndef GENERAL_FILTER_ICE
#define GENERAL_FILTER_ICE

#include <Util.ice>	//MyUtil::Str2IntMap in Util.ice is needed

module mop
{
module hi
{
module svc
{
module gf
{
	const int FOOTPRINT = 1;
	const int MESSAGE = 2;
	const int LETTER = 3;
	const int DIARYANDREPLY = 4;
	const int INVITATION = 5;
	const int ADDAFRIEND = 6;
	const int JOINAGROUP = 7;
	const int GREET = 8;
	
	const int SUCCESS = 0;
	const int ERRORUNDEFINEDTYPE = -1;
	const int ERRORVISITIONDENIED = -2;
	
	//generalinformation has seven fields, but only three of them are necessary: type, host, guest
	class GeneralInformation
	{
		long id;			
		int type;				//type of events
		int host;				//the event's source
		int guest;				//the event's destnation 
		string hostName;
		string guestName;
		long time;				//the time of the event
	};
	
	//generalfiltermanager includes two interface, but the filterControl is not proper
	class GeneralFilterManager
    	{
       	int passFilter(GeneralInformation fpt);		//test if the event is legal
		int filterControl(MyUtil::Str2IntMap cmd, int type);	//set the argments of the filter
	}; 
};
};
};
};

#endif	//GENERAL_FILTER_ICE
	
