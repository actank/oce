#ifndef CONVERTER
#define CONVERTER

#include "MceMonitor.h"
#include "Date.h"

namespace MceMonitor
{

static string
resource2str(const Resource& r)
{
	ostringstream o;
	o << "Name=" << r.name;
	o << "Type=" << r.type;
	for(MyUtil::StrSeq::const_iterator paraIt = r.parameters.begin();
			paraIt != r.parameters.end();
			++paraIt)
	{
		o << " Parameter: " << *paraIt;
	};
	o << " Interval=" << r.interval;
	o << " Min=" << r.min;
	o << " Max=" << r.max;
	o << " warningCount=" << r.warningCount;
	o << " resumeCount=" << r.resumeCount;
	o << " warningMessage=" << r.warningMessage;
	return o.str();
}

static string
log2str(const Log& l)
{
	ostringstream o;
	o << "ResourceGroup=" << l.resourcegroup;
	o << " Resource=" << l.resource;
	o << " Type=" << l.type;
	o << " Value=" << l.value;
	o << " Time=" << MyUtil::Date(l.time).str();
	return o.str();
}

static string
user2str(const User& u)
{
	ostringstream o;
	o << "Name=" << u.name;
	o << " Phone=" << u.phone;
	o << " Level=" << u.level;
	return o.str();
}

};

#endif

