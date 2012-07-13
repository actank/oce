#include "ParseParam.h"
#include <time.h>

using namespace std;
using namespace xce::parseparam;

int main() {
  cout << "MAIN" << endl;
  time_t begin = time(0);
  string json = "[{\"t\":\"base\",\"gend\":\"男生\",\"prov\":\"云南\",\"city\":\"昆明市\"},{\"t\":\"age\",\"range\":\"2\"},   {\"t\":\"high\",\"name\":\"宜良一中\",\"year\":\"2011\"},{\"t\":\"juni\",\"name\":\"昆明二十四中\",\"year\":\"2010\"},{\"t\":   \"univ\",\"name\":\"东北大学\",\"id\":\"5002\",\"year\":\"2004\"},{\"t\":\"work\",\"name\":\"千橡互动\"},{\"t\":\"text\"}]";
  ParseParam a;
  map<string, string> result = a.json2map(json);
  for (map<string, string>::iterator itt = result.begin(); itt != result.end(); itt++) {
    cout << itt->first << " " << itt->second << endl;
  }
  sleep(5);
  time_t end = time(0);
  int cost = end - begin;
  cout << "BEGIN:" << begin << " END:" << end <<  " COST:" << cost << endl;
/*  string json = "[{\"t\":\"base\",\"gend\":\"男生\",\"prov\":\"云南\",\"city\":\"昆明市\"},{\"t\":\"age\",\"range\":\"2\"},{\"t\":\"high\",\"name\":\"宜良一中\",\"year\":\"2011\"},{\"t\":\"juni\",\"name\":\"昆明二十四中\",\"year\":\"2010\"},{\"t\":\"univ\",\"name\":\"东北大学\",\"id\":\"5002\",\"year\":\"2004\"},{\"t\":\"work\",\"name\":\"千橡互动\"},{\"t\":\"text\"}]";
  cout << json << " size:" << json.size() << endl;
  string subString = string(json,1,json.size() - 2);
  cout << subString << " size:" << subString.size() << endl;
  cout << string(subString, 5, 10) << endl;
  int first = 0;
  int count = 0;
  int num = 0;
  string key = "";
  string text = "";
  string value = "";
  string type = "";
  vector<string> results;
  map<string, map<string, string> > str2str2str;
  map<string, string> str2str;
  for (int i = 0; i < subString.size(); i++) {
    count++;
    if (subString[i] == '{') {
      first = i;
      count = 0;
    }
    if (subString[i] == '}') {
      results.push_back(string(subString, first + 1, count - 1));
    }
  }
  cout << "results.size():" << results.size() << endl;
//  for (int j = 0; j < 1; j++) {
  for (int j = 0; j < results.size(); j++) {
    count = 0;
    first = 0;
    cout << results[j] << endl;
    for (int k = 0; k < results[j].size(); k++) {
//      cout << "CHAR:" << results[j][k] << endl;
      count++;
      if (results[j][k] == '\"') {
        if (num % 2 != 0) {
          text = string(results[j], first + 1, count - 1);
//          cout << "FIRST:" << first << " COUNT:" << count << endl;
//          cout << "TEXT:" << text << endl;
        }
        if (num % 2 == 0) {
          count = 0;
          first = k;
        }
        num++;
      }
      if (results[j][k] == ':') {
        key = text;
  //      cout << "KEY:" << key << endl;
      }
      if (results[j][k] == ',') {
//        cout << "VALUE:" << text << endl;
        value = text;
        if (key == "t") {
          type = value;
    //      cout << "KEY:" << key << " VALUE:" << value << " TYPE:" << type << endl;
        } else {
          key = type + "|" + key;
          str2str[key] = value;
    //      cout << "KEY:" << key << " VALUE:" << value << endl;;
        }
      }
      if (k == results[j].size() - 1){
//        cout << "VALUE:" << text << endl;
        value = text;
        if (key == "t") {
          type = value;
        } else {
          key = type + "|" + key;
          str2str[key] = value;
 //         cout << "KEY:" << key << " VALUE: " << value << endl;;
        }
      }
    }
  }
  cout << "str2str.size():" << str2str.size() << endl;
  for (map<string,string>::iterator it = str2str.begin(); it != str2str.end(); it++) {
    cout << it->first << " " << it->second << endl;
  }
  */
}
