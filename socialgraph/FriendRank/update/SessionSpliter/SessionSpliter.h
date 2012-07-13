#ifndef _SESSION_SPLITER_H_
#define _SESSION_SPLITER_H_

#include "Weblog.h"
#include "Session.h"

const int SESSION_NUM = 96;  // 96 * 15 = 24, 15分钟一个Session
int GetSessionIndex(const time_t ltime);

class SessionSpliter
{
	public:
		SessionSpliter() {}
		~SessionSpliter() {}
		static void SplitSession(string dir, string fname)
		{
			if (dir.substr(dir.length()-1)=="/")
				dir = dir.substr(0, dir.length()-1);
			SplitFile(dir, fname);
			MergeFile(dir, fname);
		}
	private:
		static int SplitFile(const string& dir, const string& fname);
		static int MergeFile(const string& dir, const string& fname);
		static int MergeEachFile(FILE* fp, map<USERID,map<USERID,short> >& pvDay);
		static int UpdateSession(map<USERID,map<USERID,short> >& pvSession, const Session& ss);
};

#endif

