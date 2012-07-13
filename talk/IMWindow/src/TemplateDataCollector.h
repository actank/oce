#ifndef TEMPLATEDATACOLLECTOR_H_
#define TEMPLATEDATACOLLECTOR_H_

#include <string>
#include <IceUtil/Shared.h>

#include <ctemplate/template.h>
#include "Util.h"

namespace talk {
namespace window{

using namespace MyUtil;
using ctemplate::TemplateDictionary;

class TemplateDataCollector : public IceUtil::Shared{
public:
	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict){
		return true;
	};
};

typedef IceUtil::Handle<TemplateDataCollector> TemplateDataCollectorPtr;

class UserInfoCollector : public TemplateDataCollector{
public:

	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};
class StaticAdInfoCollector : public TemplateDataCollector{
public:

	virtual bool fillData(const std::string& path, const Str2StrMap& cookies, const Str2StrMap& parameter, TemplateDictionary& dict);
};

};
};

#endif /*TEMPLATEDATACOLLECTOR_H_*/
