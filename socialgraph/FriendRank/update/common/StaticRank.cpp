#include "StaticRank.h"

// constructor & destructor {{{
StaticRank::StaticRank(USERID fid)
{
	Init(fid);
}
StaticRank::~StaticRank()
{
}// }}}

// Init {{{
void StaticRank::Init(USERID fid)
{
	this->fid = fid;
	this->diffBirth = -1;
	this->diffGender = false;

	this->friends[0] = this->friends[1] = this->friends[2] = 0;
	this->commonInfors = 0;
	this->scoreGroup = 0;
	this->isInvited = false;
	this->isSpecial = false;
	this->isGiftReceiver = false;
	this->feedconfig = 0;
}// }}}

// operator= {{{
StaticRank& StaticRank::operator=(const StaticRank& other)
{
	this->fid = other.fid;
	this->diffBirth = other.diffBirth;
	this->diffGender = other.diffGender;
	this->friends[0] = other.friends[0];
	this->friends[1] = other.friends[1];
	this->friends[2] = other.friends[2];
	this->commonInfors = other.commonInfors;
	this->scoreGroup = other.scoreGroup;
	this->isInvited = other.isInvited;
	this->isSpecial = other.isSpecial;
	this->isGiftReceiver = other.isGiftReceiver;
	this->feedconfig = other.feedconfig;
	return *this;
}// }}}

// Print {{{
void StaticRank::Print() const
{
	printf("fid=%10d ", fid);
	if (diffBirth==-1)
		printf("[年龄相差?岁]");
	else
		printf("[年龄相差%d岁]", (int)diffBirth);

	float mi = 0;
	if (friends[2]>0)
		mi = log(60000000.0*friends[2]/(friends[0]*friends[1]));

	printf(",%s,[共同好友%2.f(%d,%d,%d)],[资料匹配%.2f],[分组匹配%.2f],%s,%s,%s",
			diffGender?"异性":"",
			mi, friends[0], friends[1], friends[2],
		 	commonInfors, scoreGroup,
			isInvited?"被邀请":"", 
			isSpecial?"特别好友":"",
			isGiftReceiver?"礼物接收人":"");

	if (feedconfig>0)
		printf(",新鲜事白名单");
	else if (feedconfig<0)
		printf(",新鲜事黑名单");
	//else "不在黑白名单中"
}// }}}

//// GetScore() {{{
//float StaticRank::GetScore() const
//{
//	float score = 0;
//	if (diffBirth<10 && diffGender)
//		score += 2;
//	if (friends[2]>0)
//		score += log(60000000.0*friends[2]/(friends[0]*friends[1]));
//	if (commonInfors>0)
//		score += commonInfors;
//	if (scoreGroup>0)
//		score += scoreGroup;
//	if (isInvited)
//		score += 2;
//	if (isSpecial)
//		score += 20;
//	if (isGiftReceiver)
//		score += 5;
//	if (feedconfig>0)
//		score += 15;
//	else if (feedconfig<0)
//		score -= 15;
//
//	return score;
//}// }}}

// GetScore(string& reason) {{{
float StaticRank::GetScore(string& reason) const
{
	char temp[1024];
	float score = 0;

	if (diffBirth<10 && diffGender)
	{
		int s_gender = 0;   // 1;
		score += s_gender;
		if ((int)diffBirth==-1)
			sprintf(temp, "异性,相差?岁+%d,", s_gender);
		else
			sprintf(temp, "异性,相差%d岁+%d,", (int)diffBirth, s_gender);
		reason.append(temp);
	}
	
	short a = friends[0] + 1;
	short b = friends[1] + 1;
	short c = friends[2] + 2;
	//float mi = log(60000000.0*friends[2]/(friends[0]*friends[1]));
	float mi = 10*sqrt((float)c/(a+b-c));
	score += mi;
	sprintf(temp, "共同好友+%.2f(%d,%d,%d),", mi, friends[0], friends[1], friends[2]);
	reason.append(temp);
	
	if (commonInfors>0)
	{
		score += commonInfors;
		sprintf(temp, "位置+%.2f,", commonInfors);
		reason.append(temp);
	}
	if (scoreGroup>0)
	{
		score += scoreGroup;
		sprintf(temp, "分组+%.2f,", scoreGroup);
		reason.append(temp);
	}
	if (isInvited)
	{
		score += 2;
		reason.append("被邀请+2,");
	}
	if (isSpecial)
	{
		score += 20;
		reason.append("特别好友+20,");
	}
	if (isGiftReceiver)
	{
		score += 3;
		reason.append("礼物+3,");
	} 
	if (feedconfig>0)
	{
		score += 15;
		reason.append("新鲜事白名单+15");
	}
	else if (feedconfig<0)
	{
		score -= 15;
		reason.append("新鲜事黑名单-15");
	}
	return score;
}// }}}

