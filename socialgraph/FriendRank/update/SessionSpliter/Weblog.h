#ifndef _WEBLOG_H_
#define _WEBLOG_H_

#include "../common/Common.h"
#define ITEM_SIZE 20

enum ITEM_NAME
{
	  TIME=0, UID, PATH, ACTION, OBJECT, ITEM_MAX_NUM
};

class Weblog
{
	public:
		Weblog();
		~Weblog();
		int ParseNextLog(FILE* fp);
		int Print() const;
	private:
		int Parse(char* line);
		int GetWeight(char* type, char* obj);

	public:
		long date;   // 单位秒
		USERID sender;
		int weight;
		USERID accepter1;
		USERID accepter2;
	private:
		char* line;
		char* items[ITEM_SIZE];
};

#endif

