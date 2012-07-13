#ifndef _SESSION_H_
#define _SESSION_H_

#include "../common/Common.h"

class Session
{
	public:
		Session() {}
		~Session() {}
		void Init(long date, int weight, USERID sender, USERID accepter1, USERID accepter2)
		{
			this->date = date;
			this->weight = weight;
			this->sender = sender;
			this->accepter1 = accepter1;
			this->accepter2 = accepter2;
		}
	public:
		long date;
		int weight;
		USERID sender;
		USERID accepter1;
		USERID accepter2;
};

#endif

