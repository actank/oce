#ifndef TEMPLATEDATACOLLECTOR_H_
#define TEMPLATEDATACOLLECTOR_H_

#include <string>
#include <IceUtil/Shared.h>

#include <ctemplate/template.h>
#include "Util.h"

namespace talk {
namespace http {

using namespace MyUtil;
using ctemplate::TemplateDictionary;

const int kLockSize = 1000000;
static IceUtil::Mutex mutex_[kLockSize];

class TemplateDataCollector : public IceUtil::Shared{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict){
		return true;
	};
};

typedef IceUtil::Handle<TemplateDataCollector> TemplateDataCollectorPtr;

class OnlineBuddyListCollector : public TemplateDataCollector{
public:

	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};

class OnlineBuddyCountCollector : public TemplateDataCollector{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};

class OnlineToolbarCountsCollector : public TemplateDataCollector{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};

class OnlineRoomUserCollector : public TemplateDataCollector{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};

class GroupHistoryCollector : public TemplateDataCollector{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};

class UserInfoCollector: public TemplateDataCollector{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};


};
};

#endif /*TEMPLATEDATACOLLECTOR_H_*/
