/*
 * AppendReceive.h
 *
 *  Created on: Jun 5, 2011
 *      Author: yejingwei
 */

#ifndef FEED_REPUTATION_H_
#define FEED_REPUTATION_H_
#include <map>
#include <iostream>
#include <fstream>
#include <ext/hash_map>
#include "Singleton.h"
using namespace std;
using  __gnu_cxx::hash_map;
namespace xce {
namespace feed {

//
//actor stype count

#define DoLog(msg) std::cout << msg << std::endl;;
uint64_t Join2(int a, int b) {
	uint64_t r = 0;
	r = r | (uint64_t(a) << 32);
	r = r | (uint32_t) b;
	return r;
}
 int scorecnt = 0;
class Appender:public MyUtil::Singleton<Appender>{
public:
	void ScanDispatch(const string & dispatchlog){
		ifstream infile;
		infile.open(dispatchlog.c_str(), ifstream::in);
		if (!infile) {
			DoLog("LogCache::LoadClickAndReply. openfile fail");
		}
		string line;
		int cnt = 0;
		while (getline(infile, line)) {
			ScanDispatchLine(line);
			++cnt;
			if(cnt % 10000 == 0){
				DoLog("Scan :" << cnt);
			}
		}
		infile.clear();
		DoLog("Appender::ScanDispatch. from_ size:" << from_.size() << " linecnt:" << cnt);
//		while (1){
//			sleep(100);
//		}
	}
	void ScanDispatchLine(const string & line){
		int from,to,featureid, stype,cnt,click, reply,receive;
		float score;
		sscanf(line.c_str(), "%d\t%d\t%d", &to, &stype, &cnt);
		uint64_t key = Join2(to,stype);
		from_[key] = cnt;
		uint64_t key_u = Join2(to,0);
		from_[key_u] += cnt;
		++scorecnt;
	}

	void ScanClickReply(const string & clickreplylog,const string & outlog){
		ifstream infile;
		infile.open(clickreplylog.c_str(), ifstream::in);
	  ofstream outfile (outlog.c_str());
		if (!infile) {
			DoLog("LogCache::ScanClickReply. open clickreplylog fail");
		}
		if (!outfile) {
			DoLog("LogCache::ScanClickReply. open outlog fail");
		}
		string line;
		while (getline(infile, line)) {
			ScanClickReplyLine(line,outfile);
		}
		if(last_to_ !=0){
			Process(outfile);
		}

	}
	void ScanClickReplyLine(const string & line,ofstream & outfile){
		int to,featureid, click,reply;
		float score;
		sscanf(line.c_str(), "%d\t%d\t%d\t%d", &to, &featureid,&click, &reply);
		if( last_to_ != 0 && last_to_ != to){
			Process(outfile);
		}
		last_to_ = to;
		pair<int,int> p = make_pair(click,reply);
		if(featureid<0){
			stypes_[featureid] = p;
		}else{
			froms_[featureid] = p;
		}

		++scorecnt;
	}
	void Process(ofstream & outfile){
		for(map<int,pair<int,int> >::iterator si = stypes_.begin(); si != stypes_.end();++si){
			int stype_recv = 0;
			for(map<int,pair<int,int> >::iterator fi = froms_.begin(); fi != froms_.end();++fi){
				uint64_t key = Join2(fi->first,-(si->first));
				hash_map<uint64_t,int>::iterator hi = from_.find(key);
				if(hi != from_.end()){
					stype_recv += hi->second;
				}
			}
			//DoLog(last_to_ << "\t" << si->first << "\t" << si->second.first << "\t" << si->second.second << "\t" << stype_recv);
			ostringstream os;
			outfile <<last_to_ << "\t" << si->first << "\t" << si->second.first << "\t" << si->second.second << "\t" << stype_recv << "\n";
//			outfile.write(os.str().c_str());
		}

		for(map<int,pair<int,int> >::iterator fi = froms_.begin(); fi != froms_.end();++fi){
			uint64_t key = Join2(fi->first,0);
			int from_recv = 0;
			hash_map<uint64_t,int>::iterator hi = from_.find(key);
			if(hi != from_.end()){
				from_recv = hi->second;
			}

//			DoLog(last_to_ << "\t" << fi->first << "\t" << fi->second.first << "\t" << fi->second.second << "\t" << from_recv);
			ostringstream os;
			outfile << last_to_ << "\t" << fi->first << "\t" << fi->second.first << "\t" << fi->second.second << "\t" << from_recv << "\n";
//			outfile.write(os.str().c_str());
		}
		Clear();
	}

	void Clear(){
		last_to_ = 0;
		stypes_.clear();
		froms_.clear();
	}
private:
	hash_map<uint64_t,int> from_;
	int last_to_;
	map<int,pair<int,int> > stypes_;
	map<int,pair<int,int> > froms_;
	hash_map<int,hash_map<int,float> > clickmap_;
	hash_map<int,float> outmap_;
	hash_map<int,float> prmap_;
};



}
}
#endif /* APPENDRECEIVE_H_ */
