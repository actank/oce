#ifndef __STEPGREET_H__
#define __STEPGREET_H__

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <cmath>
#include "IceLogger.h"
#include "Step.h"
namespace mop
{
namespace hi
{
namespace svc
{
namespace gf
{
    using namespace std;
    
	static const int MAX_COUNT_GREET = 20;
	static int OVER_COUNT_GREET = 12;
	
    static int MAX_MARK_GREET = 10000;
    static int DEC_MARK_TIMESLOT_GREET = 60*5;
    static int MAX_DEC_MARK_GREET = 15;
    
    static double ALPHA_GREET = 0.5;//加倍系数
    static double BETA_GREET = 15; //指数分母
    
	class StepGreet :public Step
	{
	protected:
		IceUtil::Mutex _lock;
	public:
		
		int steps[MAX_COUNT_GREET];
		int currentTime;
		int index;
		int avg;
		int likeAvgCount;
		int total;
		
		int mark;
		int decMarkCount;
        int mistakeCount;
        
	//int dayCount;

		StepGreet()
		{
			memset(&steps[0],0, sizeof(int)*20);
			currentTime = time(NULL);
			index = -1;
			avg = 0 ;
			likeAvgCount = 0;
			total = 0;
	//		dayCount = 0;
			
			mark = 0;
			decMarkCount = 0;
			mistakeCount = 0;
		};

		void add(int time)
		{
			IceUtil::Mutex::Lock lock(_lock);
	/*		if (++dayCount >= 300) {
				if (time - currentTime < 60*60*24) {
					++likeAvgCount;
					return;
				} else {
					likeAvgCount = 0;
					dayCount = 0;
				}
			}*/
			MCE_DEBUG("call add");
			index = (index+1)% MAX_COUNT_GREET;
			total -= steps[index];
			steps[index] = time - currentTime;
			total += steps[index];
			currentTime = time;
			avg = total / MAX_COUNT_GREET;
			if(abs(avg - steps[index]) <= 1)
			{
			    likeAvgCount ++;
			}
			MCE_DEBUG("TIME_STEP:"<< steps[index]<<"\tAVG:"<< avg <<"\tLIKE_AVG_COUNT:"<< likeAvgCount);	
		};

		int getAvgCount(int fromid, int toid)
		{
			IceUtil::Mutex::Lock lock(_lock);
		/*	if (dayCount >= 300)	
			{
				return likeAvgCount;
			}*/
			if(likeAvgCount >= OVER_COUNT_GREET)
			{
				likeAvgCount = 0;
				for(int i = 0; i< MAX_COUNT_GREET; ++i)
				{
					if(abs(avg - steps[i]) <= 1) likeAvgCount ++;
				}
			}
			MCE_DEBUG("call getAvgCount:"<<likeAvgCount);
			if((likeAvgCount >= OVER_COUNT_GREET) || (mark > 0)) 
			{
				MCE_INFO("FROMID::"<<fromid<<"\tTOID:"<<toid<<"\tTIME_STEP:"<< steps[index]<<"\tAVG:"<< avg << "\tMARK:"<<mark <<"\tMISTAKECOUNT:"<< mistakeCount << "\tDECMARKCOUNT:"<< decMarkCount << "\tLIKE_AVG_COUNT:"<< likeAvgCount);
			}
			return likeAvgCount;
		};
		
		void incMark()
		{
		    IceUtil::Mutex::Lock lock(_lock);
		    MCE_DEBUG("call incMark");
		    if(mark > MAX_MARK_GREET) return;
		    mark += (int)(ALPHA_GREET * pow(2,(double)mistakeCount/(double)BETA_GREET));
		    mistakeCount++;
		    decMarkCount = 0;
			MCE_DEBUG("MARK:"<<mark<<"\tMISTAKECOUNT:"<<mistakeCount);
		};
		
		void decMark()
		{
		    IceUtil::Mutex::Lock lock(_lock);
		    MCE_DEBUG("call decMark");
		    mistakeCount -= decMarkCount++/5;
		    if(mistakeCount<0) mistakeCount = 0;
		    
		   
		    mark = mark - (steps[index]/DEC_MARK_TIMESLOT_GREET)*MAX_DEC_MARK_GREET -1;
		    if(mark < 0) mark = 0;
			MCE_DEBUG("DEC_MARK_COUNT:"<<decMarkCount<<"\tMISTAKE_COUNT:"<<mistakeCount<<"\tMARK:"<<mark);
		   
		};
		
		int getMark()
		{
		    IceUtil::Mutex::Lock lock(_lock);
			MCE_DEBUG("call getMark:"<<mark);
		    return mark;
		};
		
	};
	
	//typedef IceUtil::Handle<Step> StepPtrLetter;
};
};
};
};
#endif

