/*
 * PreferStat.h
 *
 *  Created on: Dec 15, 2011
 *      Author: rong.tang
 */

#ifndef PreferStat_H_
#define PreferStat_H_
#include <map>
#include <iostream>
#include <fstream>
#include <ext/hash_map>
#include "Singleton.h"
#include <vector>
#include <IceUtil/Handle.h>
using namespace std;
using  __gnu_cxx::hash_map;

namespace xce {
namespace feed {

#define DoLog(msg) std::cout << msg << std::endl;;
//#define DoLog(msg) ;
static const int VECSIZE = 10000; 

class TimeStat {
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log = false) {
    _log = log;
    reset();
  }
  void reset() {
    gettimeofday(&_begin,NULL);
  }
  float getTime(bool clear = false) {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec
        - _begin.tv_usec;
    timeuse /= 1000;
    return timeuse;
  }
};

class PreferStat: public MyUtil::Singleton<PreferStat>{
public:
	
  void ProcessDispatch(const string & dispatchlog){
		TimeStat ts;
    ifstream infile;
    infile.open(dispatchlog.c_str(), ifstream::in);
		if (!infile) { 
	    DoLog("PreferStat: ProcessDispatch open file fail");	
		}
    Clear();

		int to,stype,count;
		int cnt = 0, old_id = -1;
		vector< pair<int,int> > vecStypeCnt;
		string line;
    while (getline(infile, line)) {
		  ++cnt;
      sscanf(line.c_str(), "%d\t%d\t%d", &to, &stype, &count);
      if(to != old_id){
        if(old_id != -1){
          from_map_[(long)old_id] = vecStypeCnt;
        }
        vecStypeCnt.clear();
        old_id = to;
      }
      vecStypeCnt.push_back( make_pair(stype,count) );
		}
    from_map_[(long)old_id] = vecStypeCnt;

		infile.close();
		DoLog("PreferStat::ProcessDispatch. from_ size:" << from_map_.size() << " linecnt:" << cnt);
	  DoLog("ProcessDispatch cost: " << ts.getTime());
  }

  void WriteFile4Temp(vector<string>& temp, map<int,int>& mapStypeCnt, ofstream& outfile) {
    int to, featureid, click, reply, receive;
    float score = 0;
    for(vector<string>::iterator temp_it = temp.begin(); temp_it != temp.end(); ++temp_it) {
      receive = 0;
      sscanf(temp_it->c_str(), "%d\t%d\t%d\t%d", &to, &featureid, &click, &reply);
      map<int,int>::iterator map_it = mapStypeCnt.find((-featureid));
      if(map_it != mapStypeCnt.end()){
        receive = map_it->second;
      }
      score = Score(click, reply, receive);
      outfile << to << '\t' << featureid << '\t' << score << '\n';
     // ++line_out_;
    }
  
  }

  void ProcessClickReply(const string & clickreply,const string & scorefile){
		TimeStat ts;
    ifstream infile;
		infile.open(clickreply.c_str(), ifstream::in);
	  ofstream outfile;
    outfile.open(scorefile.c_str(), ofstream::out);
    
    if (!infile) {
	    DoLog("PreferStat: ProcessClic click_file fial 1: did not exist?");
      return ;
		}
		if (!outfile.is_open()) {
	    DoLog("PreferStat: ProcessClic score_file fail 2: could not open " << scorefile);	
		  return ;
    }

		string line;
		int to, featureid, click,reply, receive;
    int old_id = -1, cnt=0;
    float score;
    vector<string> temp;
    map<int,int> mapStypeCnt;
		//line_in_ = line_out_ = delta = 0;
    while (getline(infile, line)) {
      //++line_in_;
      ++cnt;
      if(cnt%10000000==0) DoLog("line:" << cnt);

		  sscanf(line.c_str(), "%d\t%d\t%d\t%d", &to, &featureid,&click, &reply);
      if(to==-1) {
        DoLog("evial input!!!\nline:"<<cnt << "    data-->"<<line);
        continue;
      }
      //if(to == 263734453) {
      //  DoLog("263734453::" << to << " " << featureid <<" "<<click <<" "<<reply)
      //}
      if (old_id != to) {//new person!!
        //process older person
        if (old_id != -1 && !temp.empty()) {
          WriteFile4Temp(temp, mapStypeCnt, outfile);
        }
        //if(delta != line_in_ - line_out_) {
        //  delta = line_in_ - line_out_;
        //  DoLog("line:" << line_in_ <<" && "<<line_out_<<"  id:"<<old_id);
        //}
        old_id = to;
        mapStypeCnt.clear();
        temp.clear();
      }

      {
        //same person or first show
        if(featureid<0) {//stype 
          temp.push_back(line);
        } else {//friends
          receive = 0;
          hash_map<long,vector<pair<int,int> > >::iterator hi = from_map_.find( (long)featureid );
          if(hi != from_map_.end()){
            vector<pair<int,int> > res = hi->second;
            for(vector<pair<int,int> >::iterator res_it = res.begin(); res_it != res.end(); ++res_it){
              int stype = res_it->first;
              int cnt = res_it->second; 
                //1.addup
              receive += cnt;
              //add in mapStypeCnt
              mapStypeCnt[stype] += cnt;
            }
          } else {
            //DoLog("frined:  did not find friend:" << featureid);
          } 
          score = Score(click, reply, receive);
          outfile << to << '\t' << featureid << '\t' << score << '\n';
         // ++line_out_;
        }
      }//same person 
    }
    // process the last person
    //DoLog("left tempsize:" << temp.size());
    if (!temp.empty()) {
      WriteFile4Temp(temp, mapStypeCnt, outfile);
    }

    infile.close();
    outfile.close();
    DoLog("ProcessClick cost: " << ts.getTime() << "linecnt: " << cnt);
  }

  class Person {
  public:
    Person(int from=0, int to=0, float score=0.0):from_(from), to_(to), score_(score){
    }
    int from_, to_;
    float score_;
    bool operator < (const Person& B) const {
      return this->score_ < B.score_;
    } 
  };
  bool LessThan( const Person& A, const Person& B) {
    return A.score_ < B.score_;
  }
  
  void WriteFile(vector<Person>& vecPerson, int vecPersonSize,
          vector<Person>& vecType, int vecTypeSize, bool special, ofstream& outfile) {
    
    sort(vecPerson.begin(), vecPerson.begin()+vecPersonSize);
    float weight = 1.0, meta = 0.0, sum = 0.0;
    if (special) {
      weight = 0.5;
      sum = weight;
    } 
    meta = weight/vecPersonSize, sum += meta;
    for(vector<Person>::iterator it = vecPerson.begin(); 
        it != vecPerson.begin()+vecPersonSize; ++it) {
      outfile << it->from_ << '\t' << it->to_ << '\t' << sum << '\n';
      sum += meta;
    }

    sort(vecType.begin(), vecType.begin()+vecTypeSize);
    if (special) sum = weight;
    else sum = 0.0;
    meta = weight/vecTypeSize, sum += meta;
    for(vector<Person>::iterator it = vecType.begin(); 
        it != vecType.begin()+vecTypeSize; ++it) {
      outfile << it->from_ << '\t' << it->to_ << '\t' << sum << '\n';
      sum += meta;
    }

  }

  void Process(const string& filename) {
    TimeStat ts;
    DoLog("PreferStat::Process!!")
    string resfile = filename;
    resfile += ".result";
    
    ifstream infile (filename.c_str(), ifstream::in);
    ofstream outfile (resfile.c_str());
    if(!infile) {
      DoLog("Process open infile fail");
      return ;
    }
    if(!outfile.is_open()) { 
      DoLog("Process open outfile fail");
      return ;
    }
    cout << "Process: resultname " << resfile; 

    string line;
    int from, to, old_id = -111;
    float score;
    vector<Person> vecPerson(VECSIZE);
    vector<Person> vecType(VECSIZE);
    int vecPersonSize = 0, vecTypeSize = 0, cnt = 0;
    while(getline(infile, line)) {
      ++cnt;
      if(cnt%10000000 == 0) {
        //DoLog("line:" << linecnt);
        cout << "\nline: " << cnt;
      }
      sscanf(line.c_str(), "%d\t%d\t%f", &from, &to, &score);
      if(from == -1) {
        DoLog("evial data:!!  line:" << cnt <<" \n"<<line);
        continue;
      }
      if(old_id != from) {
        if(old_id != -111){
          int flag_bit = (old_id / 10) % 10; 
          if ( flag_bit == 0 || flag_bit == 9 ) {
            WriteFile(vecPerson, vecPersonSize, vecType, vecTypeSize, true, outfile);
          } else {
            WriteFile(vecPerson, vecPersonSize, vecType, vecTypeSize, false, outfile);
          }
        }
        vecPersonSize = 0;
        vecTypeSize = 0;
        old_id = from;
      } 
      if(to > 0) {//person
        if(vecPersonSize < VECSIZE) {
          vecPerson.at(vecPersonSize).from_ = from; 
          vecPerson.at(vecPersonSize).to_ = to;
          vecPerson.at(vecPersonSize).score_ = score;
          vecPersonSize++;
        } else {
          DoLog("Process::vecPerson cache is too small!!!");
          return ;
        }
      } else {//Type
        if(vecTypeSize < VECSIZE) {
          vecType.at(vecTypeSize).from_ = from;
          vecType.at(vecTypeSize).to_ = to;
          vecType.at(vecTypeSize).score_ = score;
          vecTypeSize++;
        } else {
          DoLog("Process::vecType cache is too small!!!");
          return ;
        }
      }
    }

    int flag_bit = (old_id / 10) % 10; 
    if ( flag_bit == 0 || flag_bit == 9 ) {
      WriteFile(vecPerson, vecPersonSize, vecType, vecTypeSize, true, outfile);
    } else {
      WriteFile(vecPerson, vecPersonSize, vecType, vecTypeSize, false, outfile);
    }

    infile.close();
    outfile.close();
    DoLog("Process cost: " << ts.getTime() << "linecnt: " << cnt);
  }

	void Clear(){
		from_map_.clear();
	}

  float Score(float click, float reply, float receice) {
    return (click + 3*reply) / (receice + 2); 
  }
private:
	hash_map<long, vector< pair<int,int> > > from_map_;
  //int line_in_, line_out_, delta;
};


}

}
#endif


