#include "GroupCountElement.h"

GroupCountElement::GroupCountElement(vector<string> &nameList)
{
	for (int i = 0; i<nameList.size(); i++)
	{
		StringCountMap a;
		stringCountMapList.push_back(a);
		stringCountMapList.at(i).insert(nameList.at(i));			}
	count = 1;
}
void GroupCountElement::disp()
{
	for (int i = 0; i < stringCountMapList.size(); i ++)
	{
		cout<<"   List: "<< i << endl;
		stringCountMapList.at(i).disp();
	}
}
void GroupCountElement::update(vector<string> &nameList, int increasement)
{
	for (int i = 0; i< nameList.size(); i++)
	{
		stringCountMapList.at(i).insert(nameList.at(i));
	}
		count += increasement;
}
