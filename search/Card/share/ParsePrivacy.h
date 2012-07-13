#include <iostream>
#include <map>
#include <string>

namespace xce{
namespace cardcache{

class ParsePrivacy {
	public:
		ParsePrivacy(){
				pattern.push_back("h");
				pattern.push_back("m");
				pattern.push_back("o");
				pattern.push_back("q");
				pattern.push_back("r");
				pattern.push_back("u");
		}
		std::map<std::string,std::string> getJsonMap(const std::string& line){
			std::string key;
			std::string value;
			std::map<std::string,std::string> json;
			int mid = 1;
			for(size_t i=1,begin=1;i<line.size()-1;++i){
				if(line[i]==':'){
					key = std::string(line,begin,i-begin);			
					mid = i+1;
				}
				if(line[i]==','){
					value = std::string(line,mid,i-mid);
					begin = i+1;
					json[key] = value;
				}
			}	
			json[key]=std::string(line,mid,line.size()-mid);	
			return json;
		}
		std::string getJsonStr(const std::string& privacy){
                std::map<std::string,std::string> json = getJsonMap(privacy);
                ostringstream oss; 
                bool first = true;
                oss<<"{";
                for(std::vector<std::string>::iterator it=pattern.begin();it!=pattern.end();++it){
                        if(json.find(*it)!=json.end()){
                                if(!first)
                                    oss<<",";
                                oss<<*it<<":"<<json[*it];
                                first = false;
                        }
                }
                oss<<"}";
				return oss.str();
		}
	private:
		std::vector<std::string> pattern;
};
};
};
