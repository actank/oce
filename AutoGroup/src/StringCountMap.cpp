#include "StringCountMap.h"
StringCountMap::StringCountMap()
{
	countStringMost = 0;
}
void StringCountMap::insert(string &stringname)
{
	if (stringname.size() == 0)
			return;
	int cnt;
	map<string,int>::iterator it;
	it = stringMap.find(stringname);
	if (it != stringMap.end())
	{
		stringMap[stringname]++;
		cnt = stringMap[stringname];
	}
	else
	{
		stringMap.insert(pair<string, int>(stringname, 1));
		cnt = 1;
	}
	if ( cnt > countStringMost)
	{
		countStringMost = cnt;
		stringForMost = stringname;
	}
}
void StringCountMap::disp()
{
	cout<<"    StringCountMap"<<endl;
	map<string,int>::iterator it= stringMap.begin();
 	 for(;it!= stringMap.end();++it)  
	 {
	   	 cout<<"             (name: "<<it->first<<"  value: "<<it->second<<")"<<endl;
	} 
	cout<<"    max:("<< stringForMost<<"---"<< countStringMost<<")"<<endl;
}
