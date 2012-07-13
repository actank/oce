#ifndef ANTISPAM_ICE
#define ANTISPAM_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module spam
{
    
	dictionary<string,int> ControlMap;
	struct CheckAns{
		int type;
		double  order;
	};  
    class AntispamManager2
    {
        int check(int fromId, int toid, int type, string text, int hashcode);
         CheckAns  check2(int fromId, int toid, int type, string text, int hashcode);
		void addDictWord(string word, int type);
        void addNewSpam(string text);
//        void stopCheck(int stat);//0为开启所有检查，1为停止Spam检查， 2为停止Spam和频率的所有检查	
//        void changeTimeslot(int slot);
        void control(ControlMap props);
    };


};
};
};
};
#endif

