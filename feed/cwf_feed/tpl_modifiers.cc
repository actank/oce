#include "tpl_modifiers.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "LogWrapper.h"
#include "feed/base_feed/logging.h"

namespace xce {
namespace notify {

using namespace ctemplate;
using namespace std;

static void html_split(const std::string& in, std::vector<std::string>& out) {
	std::string os;
	for(size_t i = 0; i < in.size(); ++i){
		if(in[i] == '<'){
			if(!os.empty()){
				out.push_back(os);
				os.clear();
			}
			os += in[i];
			while(++i < in.size()){
				os += in[i];
				if(in[i] == '>'){
	        if(!os.empty()){
					  out.push_back(os);
					  os.clear();
          }
          break;
				}
			}
		}else{
			os += in[i];
		}
	}
	if(!os.empty()){
		out.push_back(os);
	}
}

static void Append(const string & in, const size_t max_len, string & out, size_t & cur_len) {
	size_t last_size = 0; // 最有一个字符的字节数
	size_t i = 0;

	while(cur_len < max_len && i < in.size())
	{
		if((in[i] & 0x80) == 0) // 英文字母
		{
			out += in[i];
      last_size = 1;
			cur_len += 2; // 和中文一样，也作2个宽度
			++i;
		}
		else if((in[i] & 0xE0) == 0xC0) // 110, 宽度是1 ?
		{
			if(in[i+1] == 0)
				break;
			out += in[i];
			out += in[i+1];
      last_size = 2;
			cur_len += 1;
			i += 2;
		}
		else if((in[i] & 0xF0) == 0xE0) // 1110, 宽度是2
		{
			if(in[i+1] == 0 || in[i+2] == 0)
				break;
			out += in[i];
			out += in[i+1];
			out += in[i+2];
      last_size = 3;
			cur_len += 2;
			i += 3;
		}
		else if((in[i] & 0xF8) == 0xF0) // 11110, 宽度是2 ?
		{
			if(in[i+1] == 0 || in[i+2] == 0 || in[i+3] == 0)
				break;
			out += in[i];
			out += in[i+1];
			out += in[i+2];
			out += in[i+3];
      last_size = 4;
			cur_len += 2;
			i += 4;
		}
		else
		{
			break;
		}
	}

  if(i < in.size())
  {
    if(out.size() > last_size)
    {
      size_t sz = out.size() - last_size;
      out.resize(sz);
    }
    out += "...";
  }
}

string HtmlModifyLength(const string & in, size_t max_len)
{
  string out;
  size_t cur_len = 0;
  vector<string> v;
  html_split(in, v);

  size_t i = 0;
  for(; i<v.size(); ++i)
  {
    if(v[i][0] == '<')
    {
      if(boost::starts_with(v[i], "<img") || boost::starts_with(v[i], "<IMG"))
      {
        cur_len += 2; //图片算两个宽度
      }
      out += v[i];
    }
    else
    {
      Append(v[i], max_len, out, cur_len);
    }

    if(cur_len >= max_len)
    {
      ++i;
      break;
    }
  }

  if(i < v.size())
  {
    if(boost::starts_with(v[i], "</"))
    {
      out += v[i];
    }
  }

  return out;
}


void MaxLengthModifier::Modify(const char* in, size_t inlen,
		const ctemplate::PerExpandData* per_expand_data, ExpandEmitter* outbuf,
		const std::string& arg) const {
  size_t max_len = 32;
  if (arg.size() > 1) {
    try {
       max_len = boost::lexical_cast<size_t>(arg.c_str() + 1);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("MaxLengthModifier arg bad_lexical_cast : " << e.what());
    }
  }
	std::string out = HtmlModifyLength(string(in , inlen), max_len);
	outbuf->Emit(out);
}

// 为 app 业务返回特定格式的时间字符串
void AppTimeFormatModifier::Modify(const char* in, size_t inlen,
		const ctemplate::PerExpandData*, ExpandEmitter* outbuf,
		const std::string&) const {
  time_t t = time(NULL);
  try {
     t = boost::lexical_cast<time_t>(in);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN("MaxLengthModifier arg bad_lexical_cast : " << e.what());
  }

	struct tm lt;
	localtime_r(&t, &lt);

	const int MAX_LEN = 24;
	char res[MAX_LEN];
	res[MAX_LEN - 1] = 0;
	snprintf(res, MAX_LEN - 1, "%d-%02d-%02d %02d:%02d:%02d", lt.tm_year + 1900,
		 	lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
	outbuf->Emit(res);

}

}
}
