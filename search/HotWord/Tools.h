#include <string>
#include <cstdlib>
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"


using namespace std;
namespace xce{
namespace hotwords{

class SplitWord {

public:
	
	static bool valid(wchar_t ch){
		return (int)ch >=0x4e00 && (int)ch<=0x9fa5;
	}
	static wstring str2wstr(const string& str){
		setlocale(LC_CTYPE,"");
		size_t maxSize = mbstowcs(NULL,str.c_str(),0);
		wchar_t* wcs = new wchar_t[maxSize + 1];
		mbstowcs(wcs,str.c_str(),maxSize);
		wcs[maxSize] = '\0';
		return wstring(wcs);
	}
	static string wstr2str(const wstring& wstr){
		setlocale(LC_CTYPE,"");
		size_t maxSize =  wcstombs(NULL, wstr.c_str(), 0);
		char* buffer = new char[maxSize + 1];
		buffer[maxSize] = '\0';
		wcstombs(buffer, wstr.c_str(), maxSize);
		return string(buffer);
	}

	static void splitHZ(const string& content,size_t len,vector<string>& hzs){
		wstring wcontent = str2wstr(content);
		for(size_t i=0;(i+len)<=wcontent.size();++i){
			size_t j=0;
			while(j<len){
				if(!valid(wcontent[i+j])){
					break;
				}	
				++j;
			}
			if(j==len){
				wostringstream woss;
				for(size_t k=i;k<i+len;++k){
					woss<<wcontent[k];	
				}
				hzs.push_back(wstr2str(woss.str()));
			}
		}
	}
	static string getAlias(const string& detail){
		string alias = detail.substr(1,detail.size()-2);
		vector<string> arrs;
		boost::algorithm::split(arrs,alias,boost::algorithm::is_any_of(";")); 
		set<wstring> uniq;
		size_t maxSize = 0;
		for (size_t i=0;i<arrs.size();++i) {
			vector<string> tmp;
			boost::algorithm::split(tmp,arrs[i],boost::algorithm::is_any_of(":"));
			if(tmp.size()!=2){
				continue;
			}
			string val = tmp[1];
			vector<string> vals;
			boost::algorithm::split(vals,val,boost::algorithm::is_any_of(","));
			vector<wstring> wvals;
			for(size_t i=0;i<vals.size();++i){
				wvals.push_back(str2wstr(vals[i]));
			}
			wstring s;
			if (wvals.size() == 3) {
				if (boost::starts_with(wvals[1],wvals[0])
						 && boost::ends_with(wvals[2],wvals[0])) {
					s = wvals[2] + wvals[1][wvals[1].size() - 1];
				}else if (boost::ends_with(wvals[1],wvals[0])
						 && boost::starts_with(wvals[2],wvals[0])) {
					s = wvals[1] + wvals[2][wvals[2].size() - 1];
				}
			}else if (wvals.size() == 2){
				if(boost::starts_with(wvals[1],wvals[0])){
					s = wvals[1];
				}else if(boost::ends_with(wvals[1],wvals[0])){
					s = wvals[1];
				}
			}else if(wvals.size() == 1){
				s = wvals[0];
			}	
			if(!s.empty()){
				uniq.insert(s);
				if (s.size() > maxSize) {
					maxSize = s.size();
				}
			}
		}
		vector<wstring> res;
		for (set<wstring>::iterator it=uniq.begin();it!=uniq.end();++it) 
			if (it->size() == maxSize){
				res.push_back(*it);
			}
		if (res.size() == 1) 
			return wstr2str(res[0]);
		int edge[res.size()][res.size()] ;
		int in[res.size()] ;
		memset(edge,0,sizeof(edge));
		memset(in,0,sizeof(in));
		for (size_t i = 0; i < res.size(); ++i) {
			wstring sub = res[i].substr(1);
			for (size_t j = 0; j < res.size(); ++j) {
				if (boost::starts_with(res[j],sub)) {
					edge[i][j] = 1;
					++in[j];
				}
			}
		}
		vector<wstring> show;
		int total = res.size();
		while (total-- > 0) {
			for (size_t i = 0; i < res.size(); ++i) {
				if (in[i] == 0) {
					show.push_back(res[i]);
					for (size_t j = 0; j < res.size(); ++j) {
						if (edge[i][j] == 1) {
							--in[j];
							edge[i][j] = 0;
						}
					}
					in[i] = -1;
					break;
				}
			}
		}
		if (show.empty()) {
			return alias;
		}
		wostringstream oss;
		oss<<show[0];
		wstring pre = show[0].substr(1);
		for (size_t i = 1; i < show.size(); ++i) {
			if (boost::starts_with(show[i],pre)) {
				oss<< show[i][show[i].size()-1];
			} else {
				oss<<","<<show[i];
			}
			pre = show[i].substr(1);
		}
		return wstr2str(oss.str());
	}
};
};
};
