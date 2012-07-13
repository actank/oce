#include "StringInt.h"
StringInt::StringInt(string n, int c)
{
	name = n;
	cnt = c;
}
void StringInt::disp()
{
	cout <<"(" << name << "," << cnt << ")"<< endl;
}
