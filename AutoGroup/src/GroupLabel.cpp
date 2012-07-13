#include "GroupLabel.h"
void GroupLabel::dispRes(vector<StringInt> &res, const char *s)
{
	cout<<s<<endl;
	for(int i = 0; i< res.size(); i++)
		res.at(i).disp();
}

/* return a digit chosen btw index i and j, depending on 
 * which index has bigger value
 * if equals, i is returned
 */
static int compare(const vector<vector<StringInt> > & countList, int i, int j)
{
	if (countList.at(i).at(0).cnt >= countList.at(j).at(0).cnt)
		return i;
	return j;
}
static int compare(const vector<StringInt> & countList, int i, int j)
{
	if (countList.at(i).cnt >= countList.at(j).cnt)
		return i;
	return j;
}
static int itemMaxValue(const vector<vector<StringInt> > & countList)
{
	int maxIndex = 0;
	for (int i = 1; i < countList.size(); i++)
		maxIndex = compare(countList, maxIndex, i);
	return maxIndex;
}
static int itemMaxValue(const vector<StringInt> & countList)
{
	int maxIndex = 0;
	for (int i = 1; i < countList.size(); i++)
		maxIndex = compare(countList, maxIndex, i);
	return maxIndex;
}
void GroupLabel::existingLabel(const vector<int> &userlist, \
		int host, StringInt & res)
{
	vector<StringInt> tmp;
	ExistingLabelCount e(host);
	e.countLabel(userlist,tmp);
	res = tmp.at(0);
}
void GroupLabel::userBasicLabel(const vector<int> &userlist,\
		       	StringInt & res)
{
	UserBasicCount userBasic(userlist);
	vector<StringInt> userBasicMax; 
	userBasic.countCity(userlist, userBasicMax);
//	dispRes(userBasicMax, "UserBasic");
	res.name = userBasicMax.at(0).name;
	res.cnt = userBasicMax.at(0).cnt;
	if (userBasicMax.at(0).cnt && \
			userBasicMax.at(1).cnt >= userBasicMax.at(0).cnt * 0.5)
		res.name.append(userBasicMax.at(1).name);
				
}
void GroupLabel::networkLabel(const vector<int> &userList, StringInt & res, int host)
{
	NetworkBigObjectNPtr p = UserNetworkAdapter::instance().getBigObjectN(host);
	UniversityInfoSeq &univList = p->universitys;
	vector<string> hostUnivName;
	vector<string> hostDepartmentName;
	vector<string> hostappendYear;
	for(int i=0; i<univList.size(); ++i)
	{
		UniversityInfoPtr &univInfo = univList.at(i);
		hostUnivName.push_back(univInfo->universityName);
		hostDepartmentName.push_back(univInfo->department);

    	char tmpstr[25];
        sprintf(tmpstr, "%d", univInfo->enrollYear);
		hostappendYear.push_back(string(tmpstr));
	}	

	NetworkCount netCount(userList);
	//cout<<"NetworkCount netCount(userList)"<<endl;

	vector<vector<StringInt> > countList(5);
	netCount.countUniv(userList, countList.at(0));
	//dispRes(countList.at(0), "University");

	netCount.countHighSchool(userList, countList.at(1));
	//dispRes(countList.at(1), "HighSchool");

	netCount.countElementarySchool(userList, countList.at(2));
	//dispRes(countList.at(2), "ElementarySchool");
	
	netCount.countJuniorHighSchool(userList, countList.at(3));
	//dispRes(countList.at(3), "JuniorHighSchool");
	
	netCount.countWorkplace(userList, countList.at(4));
	//dispRes(countList.at(4), "Workplace");

	int maxIndex;   // index of the item which has the biggest count value
	maxIndex = itemMaxValue(countList);	

	float threshold = 0;

    if(5 >= userList.size())
    {
        threshold = 0.7;
    }
    else
    {
        threshold = 0.5;
    }


	const vector<StringInt> &max = countList.at(maxIndex);
	res.name = max.at(0).name;
	int index;
	//bool appendYear = true;
	bool appendYear = false;
	switch (maxIndex){
	case 0:
		if (max.at(0).cnt == 0)
			break;
		if(find(hostUnivName.begin(), hostUnivName.end(), max.at(0).name) == hostUnivName.end())     //与用户不同学校，则粗化到学校
			break;
		//index = max.at(1).cnt < max.at(2).cnt? 2 : 1;
		index = 1;

		if(find(hostDepartmentName.begin(), hostDepartmentName.end(), max.at(index).name) == hostDepartmentName.end())    //与用户不同学院，则粗化到学校
			break;

		if (max.at(index).cnt >= max.at(0).cnt * threshold){

			res.name.append(max.at(index).name);

			if(find(hostappendYear.begin(), hostappendYear.end(), max.at(3).name) != hostappendYear.end())     //与用户不同界，则粗化到学院
				appendYear = true;
		}

		if (appendYear && max.at(3).cnt >= max.at(0).cnt * threshold && max.at(3).name.size() == 4)
			res.name = res.name + max.at(3).name.substr(2,2) ;
		break;
	case 1:
		if (max.at(0).cnt == 0)
			break;
		/*
		index = (max.at(1).cnt < max.at(2).cnt && max.at(2).name != "0" )? 2 : 1;
		index = (max.at(index).cnt < max.at(3).cnt && max.at(3).name != "0" )? 3 : index;
		if (max.at(index).cnt >= max.at(0).cnt * 0.7 && max.at(index).name != "0")
			res.name = res.name + max.at(index).name + "班";
		break;
		*/
	}
	res.cnt = max.at(0).cnt;
}

void GroupLabel::Label(const vector<int> &userlist, int host, vector<StringInt> & res)
{
	vector<StringInt> tmp;
	StringInt netLabel;
	networkLabel(userlist, netLabel, host);
	//cout<<"networkLabel(userlist, netLabel)"<<endl;

	StringInt basicLabel;
	userBasicLabel(userlist, basicLabel);
	//cout<<"userBasicLabel(userlist, basicLabel)"<<endl;

	StringInt currentLabel;
	existingLabel(userlist, host, currentLabel);
	//cout<<"existingLabel(userlist, host, currentLabel)"<<endl;

	tmp.push_back(netLabel);
	tmp.push_back(basicLabel);
	tmp.push_back(currentLabel);

	int maxIndex = itemMaxValue(tmp);

	switch(maxIndex){
	case 0:
		res.push_back(netLabel);
		break;
	case 1:
		if (netLabel.cnt >= basicLabel.cnt * 0.2)
			res.push_back(netLabel);
		if (netLabel.cnt <= basicLabel.cnt * 0.5)
			res.push_back(basicLabel);
		break;
	}
	if (currentLabel.cnt > tmp.at(maxIndex).cnt * 0.3 && currentLabel.cnt > 1 || currentLabel.cnt > 9)
		res.push_back(currentLabel);

//test
//	res.push_back(StringInt(string("******"), 0));
//	res.push_back(netLabel);
//	res.push_back(basicLabel);
//	res.push_back(currentLabel);

    //判断是否给标签
    if (tmp.at(maxIndex).cnt <= userlist.size() * 0.5 && userlist.size() < 20)
    {
        StringInt unNameElement("待命名", 0);

        vector<StringInt> unNameRes;
        unNameRes.push_back(unNameElement);

        for(int i=0; i<res.size(); ++i)
        {
            unNameRes.push_back(res[i]);
        }
        res.clear();
        res = unNameRes;
    }
}
/*
int main()
{
	vector<int> userList;
	for(int i= 700000000; i< 700000999; i++)
		userList.push_back(i);
	int host = 700000520;
	vector<StringInt> res;
	GroupLabel::Label(userList, host, res);
	GroupLabel::dispRes(res, "result");
}*/
