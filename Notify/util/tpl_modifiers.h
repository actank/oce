#include <ctemplate/template.h>
#include <string>
#include <vector>

namespace xce {
namespace notify {

using namespace ctemplate;

std::string HtmlModifyLength(const std::string & in, size_t max_len);

class MaxLengthModifier: public ctemplate::TemplateModifier {
public:
  MaxLengthModifier() {}
private:
	void Modify(const char* in, size_t inlen,
			const ctemplate::PerExpandData* per_expand_data,
			ExpandEmitter* outbuf, const std::string& arg) const;
};

class AppTimeFormatModifier: public ctemplate::TemplateModifier {
public:
  AppTimeFormatModifier() {}
private:
	void Modify(const char* in, size_t inlen,
			const ctemplate::PerExpandData* per_expand_data,
			ExpandEmitter* outbuf, const std::string& arg) const;
};

}
}

