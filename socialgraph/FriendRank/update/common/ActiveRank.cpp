#include "ActiveRank.h"

ActiveRank::ActiveRank(USERID fid)
{
	this->fid = fid;
	this->pvCur = 0;
	this->pvOld = 0;
}
ActiveRank::~ActiveRank()
{
}

ActiveRank& ActiveRank::operator=(const ActiveRank& other)
{
	this->fid = other.fid;
	this->pvCur = other.pvCur;
	this->pvOld = other.pvOld;
	return *this;
}

void ActiveRank::Print(float factor) const
{
	printf("fid=%10d, score=%.3f, pvCur=%d, pvOld=%d\n", 
			fid, factor*GetScore(), pvCur, pvOld);
}

