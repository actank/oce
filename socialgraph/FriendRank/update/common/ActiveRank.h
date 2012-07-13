#ifndef _ACTIVE_RANK_H_
#define _ACTIVE_RANK_H_

#include "Common.h"

class ActiveRankHead
{
	public:
		USERID uid;
		time_t date;  // second
		short num;
};

class ActiveRank
{
	public:
		ActiveRank(USERID fid=0);
		~ActiveRank();
		void Print(float factor) const;
		ActiveRank& operator=(const ActiveRank& other);

		void Attenuate() {
			pvOld = pvCur + short(pvOld/1.3);
			pvCur = 0;
		}
		
		void IncreasePv(short pvInc) {
			pvCur += pvInc;
		}

		short GetScore(string& reason) const {
			char temp[128];
			sprintf(temp, "当前%d,历史%d", pvCur, pvOld);
			reason.append(temp);
			return pvCur + pvOld;
		}
		short GetScore() const {
			return pvCur + pvOld;
		}
		short GetCurUnit() const {
			return pvCur;
		}
		short GetOldUnit() const {
			return pvOld;
		}
		USERID GetFid() const {
			return fid;
		}

	private:
		USERID         fid;         // friend id
		short pvCur;       // 当前周期新的PV
		short pvOld;       // 上个周期的分值
};

struct Comp_ByActiveRank
{
	bool operator()(const ActiveRank& p1, const ActiveRank& p2) {
		return p1.GetScore() > p2.GetScore();
	}
};

#endif

