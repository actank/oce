
#include "ActivitiesI.h"
#include "ServiceI.h"

using namespace com::xiaonei::svc::activities;
void
MyUtil::initialize()
{
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(new ActivitiesManagerI(), service.getCommunicator()->stringToIdentity(MANAGER));
}

void
ActivitiesManagerI::occur(const ::com::xiaonei::svc::activities::ActivityRepresent& from,
        const ::com::xiaonei::svc::activities::ActivityRepresent& to,
        const Ice::Current& current)
{
    ostringstream oss;
    oss << "From: " << from.type << " " << from.id << " To:" << to.type << " " << to.id << " ";
    for(map<string, string>::const_iterator it = current.ctx.begin();
            it != current.ctx.end();
            ++it)
    {
        oss << it->first << " " << it->second << " ";
    }
    MCE_INFO(oss.str());
}
