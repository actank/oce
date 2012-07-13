#include "GeneralFilterI.h"
#include "StepFootprint.h"
#include "StepMessage.h"
#include "StepLetter.h"
#include "FootprintAdapter.h"
#include "Step.h"

using namespace MyUtil;
using namespace	mop::hi::svc::gf;
using namespace mop::hi::svc::adapter;


extern "C"
{

//
// Factory function
//
GENERALFILTER_API	::IceBox::Service*
create(Ice::CommunicatorPtr	communicator)
{
	return &GeneralFilterServiceI::instance();
}

}

//---------------------------------------------------------------------------
/*
void
GeneralFilterServiceI::initialize()
{
	addServantCache<Ice::Int>(FOOTPRINT, "S", new SyncServantFactory<Ice::Int>(new FootprintFilterFactoryI));
	addServantCache<Ice::Int>(MESSAGE, "M", new SyncServantFactory<Ice::Int>(new MessageFilterFactoryI));
	addServantCache<Ice::Int>(LETTER, "L", new SyncServantFactory<Ice::Int>(new LetterFilterFactoryI));
	addServantCache<Ice::Int>(DIARYANDREPLY, "D", new SyncServantFactory<Ice::Int>(new DiaryAndReplyFilterFactoryI));
	addServantCache<Ice::Int>(INVITATION, "I", new SyncServantFactory<Ice::Int>(new InvitationFilterFactoryI));
	addServantCache<Ice::Int>(ADDAFRIEND, "A", new SyncServantFactory<Ice::Int>(new AddAFriendFilterFactoryI));
	addServantCache<Ice::Int>(JOINAGROUP, "J", new SyncServantFactory<Ice::Int>(new JoinAGroupFilterFactoryI));
	addServantCache<Ice::Int>(GREET, "G", new SyncServantFactory<Ice::Int>(new GreetFilterFactoryI));
		
	getAdapter()->add(new GeneralFilterManagerI, createIdentity("GFM", ""));

	//FootprintAdapter::instance().initializeWith(getCommunicator());
}
*/
/*

void
GeneralFilterServiceI::initialize()
{
	addServantCache<Ice::Int>(FOOTPRINT, "S", new FootprintFilterFactoryI);
	addServantCache<Ice::Int>(MESSAGE, "M", new MessageFilterFactoryI);
	addServantCache<Ice::Int>(LETTER, "L", new LetterFilterFactoryI);
	addServantCache<Ice::Int>(DIARYANDREPLY, "D", new DiaryAndReplyFilterFactoryI);
	addServantCache<Ice::Int>(INVITATION, "I", new InvitationFilterFactoryI);
	addServantCache<Ice::Int>(ADDAFRIEND, "A", new AddAFriendFilterFactoryI);
	addServantCache<Ice::Int>(JOINAGROUP, "J", new JoinAGroupFilterFactoryI);
	addServantCache<Ice::Int>(GREET, "G", new GreetFilterFactoryI);
		
	getAdapter()->add(new GeneralFilterManagerI, createIdentity("GFM", ""));
}
*/

void
GeneralFilterServiceI::initialize()
{
	addServantCache<Ice::Int>(FOOTPRINT, "S", new GeneralFilterFactoryI(FOOTPRINT));
	addServantCache<Ice::Int>(MESSAGE, "M", new GeneralFilterFactoryI(MESSAGE));
	addServantCache<Ice::Int>(LETTER, "L", new GeneralFilterFactoryI(LETTER));
	addServantCache<Ice::Int>(DIARYANDREPLY, "D", new GeneralFilterFactoryI(DIARYANDREPLY));
	addServantCache<Ice::Int>(INVITATION, "I", new GeneralFilterFactoryI(INVITATION));
	addServantCache<Ice::Int>(ADDAFRIEND, "A", new GeneralFilterFactoryI(ADDAFRIEND));
	addServantCache<Ice::Int>(JOINAGROUP, "J", new GeneralFilterFactoryI(JOINAGROUP));
	addServantCache<Ice::Int>(GREET, "G", new GeneralFilterFactoryI(GREET));
		
	getAdapter()->add(new GeneralFilterManagerI, createIdentity("GFM", ""));
}

StepPtr
GeneralFilterServiceI::getStep(Ice::Int	userId, Ice::Int type)
{
	switch(type){
		case FOOTPRINT:
			return StepPtr::dynamicCast(findObject(FOOTPRINT, userId));
		case MESSAGE:
			return StepPtr::dynamicCast(findObject(MESSAGE, userId));
		case LETTER:
			return StepPtr::dynamicCast(findObject(LETTER, userId));
		case DIARYANDREPLY:
			return StepPtr::dynamicCast(findObject(DIARYANDREPLY, userId));
		case INVITATION:
			return StepPtr::dynamicCast(findObject(INVITATION, userId));
		case ADDAFRIEND:
			return StepPtr::dynamicCast(findObject(ADDAFRIEND, userId));
		case JOINAGROUP:
			return StepPtr::dynamicCast(findObject(JOINAGROUP, userId));
		case GREET:
			return StepPtr::dynamicCast(findObject(GREET, userId));
		default:
			MCE_DEBUG("you have pass a non_exist value of type");
			return StepPtr::dynamicCast(findObject(FOOTPRINT, userId));	//never reach here
		}
}

//------------------------GeneralFilterFactoryI---------------------------------------
GeneralFilterFactoryI::GeneralFilterFactoryI(int type)
{
	this->type = type;
}
Ice::ObjectPtr
GeneralFilterFactoryI::create(const Ice::Int& id)
{
	StepPtr result = NULL;
	switch(type){
		case FOOTPRINT:
			result = new StepFootprint;
			break;
		case MESSAGE:
			result = new StepMessage;
			break;
		case LETTER:
			result = new StepLetter;
			break;
		case DIARYANDREPLY:
			result = new StepDiaryAndReply;
			break;
		case INVITATION:
			result = new StepInvitation;
			break;
		case ADDAFRIEND:
			result = new StepAddAFriend;
			break;
		case JOINAGROUP:
			result = new StepJoinAGroup;
			break;
		case GREET:
			result = new StepGreet;
			break;
		default:
			MCE_DEBUG("you have pass a non_exist value of type");
		}
		return result;
}
/*
Ice::ObjectPtr
FootprintFilterFactoryI::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepFootprint;

	return result;
}
Ice::ObjectPtr
MessageFilterFactoryI::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepMessage;

	return result;
}
Ice::ObjectPtr
LetterFilterFactoryI::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepLetter;

	return result;
}
Ice::ObjectPtr
DiaryAndReplyFilterFactoryI ::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepDiaryAndReply;

	return result;
}
Ice::ObjectPtr
InvitationFilterFactoryI ::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepInvitation;

	return result;
}
Ice::ObjectPtr
AddAFriendFilterFactoryI ::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepAddAFriend;

	return result;
}
Ice::ObjectPtr
JoinAGroupFilterFactoryI ::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepJoinAGroup;

	return result;
}
Ice::ObjectPtr
GreetFilterFactoryI ::create(const Ice::Int& id)
{
	//MCE_INFO("Loading " << id << " ... ");
	StepPtr	result = new StepGreet;

	return result;
}
*/
//---------------------------------------------------------------------------
int
GeneralFilterManagerI::passFilter(const GeneralInformationPtr &fpt,const Ice::Current&)
{
	MCE_DEBUG("call GeneralFilterManagerI::passFilter");
	int legal = 0; 
	switch(fpt->type){
		case FOOTPRINT:
			legal = visitHomepage(fpt);
			break;
		case MESSAGE:
			legal = sendMessage(fpt);
			break;
		case LETTER:
			legal = sendLetter(fpt);
			break;
		case DIARYANDREPLY:
			legal = diaryAndReply(fpt);
			break;
		case INVITATION:
			legal = sendInvitation(fpt);
			break;
		case ADDAFRIEND:
			legal = addAFriend(fpt);
			break;
		case JOINAGROUP:
			legal = joinAGroup(fpt);
			break;
		case GREET:
			legal = sendGreet(fpt);
			break; 
		default:
			legal = -1;
			MCE_DEBUG("you passed a non_exist value of type in passFilter");	
		}
	return legal;
}

int
GeneralFilterManagerI::filterControl(const MyUtil::Str2IntMap &cmd, const Ice::Int type,const Ice::Current&)
{
	int success = 0;
	switch(type){
		case FOOTPRINT:
			success = controlFootprint(cmd);
			break;
		case MESSAGE:
			success = controlMessage(cmd);
			break;
		case LETTER:
			success = controlLetter(cmd);
			break;
		case DIARYANDREPLY:
			success = controlDiaryAndReply(cmd);
			break;
		case INVITATION:
			success = controlInvitation(cmd);
			break;
		case ADDAFRIEND:
			success = controlFriend(cmd);
			break;
		case JOINAGROUP:
			success = controlGroup(cmd);
			break;
		case GREET:
			success = controlGreet(cmd);
			break;
		default:
			success = -1;
			MCE_DEBUG("you passed a non_exist value of type in passFilter");
		}
	return success;
}

int
GeneralFilterManagerI::visitHomepage(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::visitHomepage");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,info->type);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_FOOTPRINT)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's footprint: " << info->guest);
	}
	return legal;
	
}

int
GeneralFilterManagerI::controlFootprint(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_FOOTPRINT"== it->first) OVER_COUNT_FOOTPRINT = it->second;
		else if("MAX_MARK_FOOTPRINT" == it->first) MAX_MARK_FOOTPRINT = it->second;
		else if("DEC_MARK_TIMESLOT_FOOTPRINT" == it->first) DEC_MARK_TIMESLOT_FOOTPRINT = it->second;
		else if("MAX_DEC_MARK_FOOTPRINT" == it->first) MAX_DEC_MARK_FOOTPRINT = it->second;
		else if("ALPHA_FOOTPRINT" == it->first) ALPHA_FOOTPRINT = it->second;
		else if("BETA_FOOTPRINT" == it->first) BETA_FOOTPRINT = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::sendMessage(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::sendmessage");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,MESSAGE);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_MESSAGE)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's message: " << info->guest);
	}
	return legal;
}
		

int 
GeneralFilterManagerI::controlMessage(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_MESSAGE"== it->first) OVER_COUNT_MESSAGE = it->second;
		else if("MAX_MARK_MESSAGE" == it->first) MAX_MARK_MESSAGE = it->second;
		else if("DEC_MARK_TIMESLOT_MESSAGE" == it->first) DEC_MARK_TIMESLOT_MESSAGE = it->second;
		else if("MAX_DEC_MARK_MESSAGE" == it->first) MAX_DEC_MARK_MESSAGE = it->second;
		else if("ALPHA_MESSAGE" == it->first) ALPHA_MESSAGE = it->second;
		else if("BETA_MESSAGE" == it->first) BETA_MESSAGE = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::sendLetter(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::sendletter");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,LETTER);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_LETTER)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's letter: " << info->guest);
	}
	return legal;
}

int
GeneralFilterManagerI::controlLetter(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_LETTER"== it->first) OVER_COUNT_LETTER = it->second;
		else if("MAX_MARK_LETTER" == it->first) MAX_MARK_LETTER = it->second;
		else if("DEC_MARK_TIMESLOT_LETTER" == it->first) DEC_MARK_TIMESLOT_LETTER = it->second;
		else if("MAX_DEC_MARK_LETTER" == it->first) MAX_DEC_MARK_LETTER = it->second;
		else if("ALPHA_LETTER" == it->first) ALPHA_LETTER = it->second;
		else if("BETA_LETTER" == it->first) BETA_LETTER = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::diaryAndReply(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::diaryAndReply");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,DIARYANDREPLY);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_DIARYANDREPLY)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's diaryAndReply: " << info->guest);
	}
	return legal;
}

int
GeneralFilterManagerI::controlDiaryAndReply(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_LETTER"== it->first) OVER_COUNT_DIARYANDREPLY = it->second;
		else if("MAX_MARK_LETTER" == it->first) MAX_MARK_DIARYANDREPLY = it->second;
		else if("DEC_MARK_TIMESLOT_LETTER" == it->first) DEC_MARK_TIMESLOT_DIARYANDREPLY = it->second;
		else if("MAX_DEC_MARK_LETTER" == it->first) MAX_DEC_MARK_DIARYANDREPLY = it->second;
		else if("ALPHA_LETTER" == it->first) ALPHA_DIARYANDREPLY = it->second;
		else if("BETA_LETTER" == it->first) BETA_DIARYANDREPLY = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::sendInvitation(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::Invitation");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,INVITATION);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_INVITATION)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's Invitation: " << info->guest);
	}
	return legal;
}

int
GeneralFilterManagerI::controlInvitation(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_LETTER"== it->first) OVER_COUNT_INVITATION = it->second;
		else if("MAX_MARK_LETTER" == it->first) MAX_MARK_INVITATION = it->second;
		else if("DEC_MARK_TIMESLOT_LETTER" == it->first) DEC_MARK_TIMESLOT_INVITATION = it->second;
		else if("MAX_DEC_MARK_LETTER" == it->first) MAX_DEC_MARK_INVITATION = it->second;
		else if("ALPHA_LETTER" == it->first) ALPHA_INVITATION = it->second;
		else if("BETA_LETTER" == it->first) BETA_INVITATION = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::addAFriend(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::addAFriend");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,ADDAFRIEND);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_ADDAFRIEND)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's AddAFriend: " << info->guest);
	}
	return legal;
}

int
GeneralFilterManagerI::controlFriend(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_LETTER"== it->first) OVER_COUNT_ADDAFRIEND = it->second;
		else if("MAX_MARK_LETTER" == it->first) MAX_MARK_ADDAFRIEND = it->second;
		else if("DEC_MARK_TIMESLOT_LETTER" == it->first) DEC_MARK_TIMESLOT_ADDAFRIEND = it->second;
		else if("MAX_DEC_MARK_LETTER" == it->first) MAX_DEC_MARK_ADDAFRIEND = it->second;
		else if("ALPHA_LETTER" == it->first) ALPHA_ADDAFRIEND = it->second;
		else if("BETA_LETTER" == it->first) BETA_ADDAFRIEND = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::joinAGroup(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::joinAGroup");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,JOINAGROUP);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_JOINAGROUP)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's JoinAGroup: " << info->guest);
	}
	return legal;
}

int
GeneralFilterManagerI::controlGroup(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_LETTER"== it->first) OVER_COUNT_JOINAGROUP = it->second;
		else if("MAX_MARK_LETTER" == it->first) MAX_MARK_JOINAGROUP = it->second;
		else if("DEC_MARK_TIMESLOT_LETTER" == it->first) DEC_MARK_TIMESLOT_JOINAGROUP = it->second;
		else if("MAX_DEC_MARK_LETTER" == it->first) MAX_DEC_MARK_JOINAGROUP = it->second;
		else if("ALPHA_LETTER" == it->first) ALPHA_JOINAGROUP = it->second;
		else if("BETA_LETTER" == it->first) BETA_JOINAGROUP = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}

int
GeneralFilterManagerI::sendGreet(
	const GeneralInformationPtr&	info )
{
	int legal = 0;
	MCE_DEBUG("call GeneralFilterManagerI::sendGreet");	
	StepPtr step = GeneralFilterServiceI::instance().getStep(info->guest,GREET);
	//step->add(footprint->time/1000);
	step->add(time(NULL));
	int count = step->getAvgCount(info->guest, info->host);
	if(count >= OVER_COUNT_GREET)
	{
		step->incMark();	
	}else
	{
		step->decMark();
	}
	
	if(step->getMark() == 0)
	{
		
	} else {
		legal = -2;
		MCE_DEBUG("Spam refuse user's Greet: " << info->guest);
	}
	return legal;
}

int
GeneralFilterManagerI::controlGreet(const MyUtil::Str2IntMap& cmd)
{
	for(MyUtil::Str2IntMap::const_iterator it = cmd.begin(); it != cmd.end(); it++)
	{
		if("OVER_COUNT_LETTER"== it->first) OVER_COUNT_GREET = it->second;
		else if("MAX_MARK_LETTER" == it->first) MAX_MARK_GREET = it->second;
		else if("DEC_MARK_TIMESLOT_LETTER" == it->first) DEC_MARK_TIMESLOT_GREET = it->second;
		else if("MAX_DEC_MARK_LETTER" == it->first) MAX_DEC_MARK_GREET = it->second;
		else if("ALPHA_LETTER" == it->first) ALPHA_GREET = it->second;
		else if("BETA_LETTER" == it->first) BETA_GREET = it->second; 
		MCE_INFO("[CONTRAL]  KEY:"<< it->first<<"   VALUE:"<<it->second);
	}
}
