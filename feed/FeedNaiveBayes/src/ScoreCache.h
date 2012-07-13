#ifndef FEEDNAIVEBAYESI_SCORECACHE_H_
#define FEEDNAIVEBAYESI_SCORECACHE_H_
#include "./FeedNaiveBayesI.h"
#include "QueryRunner.h"
#include "IceLogger.h"
#include <string>
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include "Date.h"
#include "CommonUtil.h"
namespace xce {
namespace feed {

using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace std;
using MyUtil::Date;
using namespace boost;

//"/data/xce/StatUtil/data/score/score_history.2011-05-21"
const static string kHistPath = "/data/xce/StatUtil/data/score_history/";


int scorecnt = 0;

string GetScoreFileName(int i) {
	MyUtil::Date DeltaDays(i* 24 * 60 * 60);
	MyUtil::Date today = Date(time(NULL));
	string name;
	MyUtil::Date d = today - DeltaDays;
	ostringstream oss;
	oss << "score_history." << d.year() << "-" << AddZero(d.month()) << "-"	<< AddZero(d.day());
	return oss.str();
}



class ScoreCache: public Singleton<ScoreCache> {
public:
	ScoreCache() :
		ready_(true) {
	}

	bool Reload() {
		if (ready_ == false) {
			MCE_INFO("ScoreCache::Reload. already in loading");
			return false;
		}
		ready_ = false;
		score_map_.clear();
		scorecnt = 0;
		ready_ = LoadScoreFile();
//		ready_ = LoadScoreFileX();
		MCE_INFO("ScoreCache::LoadScore done!!! size:" << score_map_.size()
				<< " scorecnt:" << scorecnt << " ready:" << ready_);
		//PrintScores();
		return ready_;
	}
	void PrintScores(){
		for(hash_map<uint64_t, float>::iterator it= score_map_.begin(); it != score_map_.end();++it){
			int to,featureid,score;
			Extract2(it->first, to,featureid);
			MCE_INFO("ScoreCache::PrintScores. to:" << to << " feature:" << featureid << " score:" << it->second );
		}
	}

	virtual bool MergeNew(const string & new_score,const string & dumpfile,bool dump = false){
		ifstream infile;

		infile.open(new_score.c_str(), ifstream::in);
		if (!infile) {
			MCE_INFO("ScoreCache::MergeNew. openfile fail:" << new_score );
			return false;
		} else {
			MCE_INFO("ScoreCache::MergeNew. openfile OK:" << new_score );
		}

		if (!infile) {
			MCE_INFO("ScoreCache::MergeNew. openfile all fail");
			return false;
		}

		string line;
		hash_map<uint64_t, float> score_map;
		while (getline(infile, line)) {
			HandleScoreLine(line,score_map);
		}
		MCE_INFO("ScoreCache::MergeNew. new map size:" << score_map.size() << " old score_map_ size:" << score_map_.size());
		for(hash_map<uint64_t, float>::iterator new_it= score_map.begin(); new_it != score_map.end();++new_it){
			hash_map<uint64_t, float>::iterator old_it = score_map_.find(new_it->first);
			if(old_it != score_map_.end()){
				old_it->second = (old_it->second * 1.5 + new_it->second) / 2.5;
			}else{
				score_map_[new_it->first] = new_it->second;
			}
		}
		score_map.clear();
		MCE_INFO("ScoreCache::MergeNew. After merge score_map_ size:" << score_map_.size());
		if(dump){
			Dump(dumpfile);
		}
		return true;
	}

	void Dump(const string & dumpfile){

//	  ofstream myfile (outlog_.c_str());
//	  if (!myfile.is_open())
//	  {
//	  	DoLog("Merge. Open file fail:" << outlog_);
//	  	return;
//	  }

		ofstream outfile;
		outfile.open(dumpfile.c_str(), ifstream::out);
		if (!outfile) {
			MCE_INFO("ScoreCache::Dump. openfile fail:" << dumpfile );
			return;
		} else {
			MCE_INFO("ScoreCache::Dump. openfile OK:" << dumpfile );
		}
		for(hash_map<uint64_t, float>::iterator it= score_map_.begin(); it != score_map_.end();++it){
			int to,featureid,score;
			Extract2(it->first, to,featureid);
			outfile << to << "\t" << featureid << "\t0\t0\t" << it->second << "\n";
		}
	}

	float GetScore(int to, int featureid) {
		if(!ready()){
			return 0.25;
		}
		uint64_t key = Join2(to, featureid);

		hash_map<uint64_t, float>::iterator it = score_map_.find(key);
		if (it != score_map_.end()) {
			return it->second;
		} else {
			return 0.001;
		}
	}
	bool ready() {
		return ready_;
	}
private:
	bool LoadScoreFileX(){
		const static int kBufsize =  1024 * 1024 * 1024;
		ifstream infile;
		int fd = -1;
		for (unsigned i = 0; i < 100; ++i) {
			string file = kHistPath + GetScoreFileName(i);
//			infile.open(file.c_str(), ifstream::in);
			fd = ::open(file.c_str(), O_RDONLY);
			if (fd < 0) {
				MCE_INFO("ScoreCache::LoadScoreFileX. openfile fail:" << file );
			} else {
				MCE_INFO("ScoreCache::LoadScoreFileX. openfile OK:" << file );
				break;
			}
		}
		if (fd < 0) {
			MCE_INFO("ScoreCache::LoadClickAndReply. openfile all fail");
			return false;
		}

		char * buf = new char[kBufsize];
		while (1) {
			memset(&buf[0], 0, kBufsize);
			MCE_INFO("ScoreCache::LoadScoreFileX.begin read.");
			int size = read(fd, buf, kBufsize);
			MCE_INFO("ScoreCache::LoadScoreFileX.end read. size:" << size);
			//		MCE_INFO("ScoreCache::LoadClickAndReply. buf:" << buf);
			string line;
			int start = 0;
			while (1) {
				//			MCE_INFO("ScoreCache::LoadClickAndReply. start:" << start);
				line = GetLine(buf, start, size);
				if (line.empty()) {
					//				MCE_INFO("ScoreCache::LoadClickAndReply. line emtpy. start:" << start);
					break;
				}
//				HandleScoreLine(line, score_map_);
				//			MCE_INFO("ScoreCache::LoadClickAndReply.line:" << line);

			}
			if (size < kBufsize) {
				break;
			}
		}
		delete [] buf;
		return true;
	}

	string GetLine(char * buf, int & start,int total){
		int tmpstart = start;
		char * begin = buf + start;
		char tmp[100];
		int tmpidx = 0;
		for(; buf[start] != '\n'; ++start){
//			MCE_INFO("GetLine: *idx:" << buf[start] << " total:" << total);
			if(start >= total){
				break;
			}
		}

		if(start >= total){
			return string("");
		}
		string res(begin, start-tmpstart);
		if(buf[start] == '\n'){
			++start;
		}
//		MCE_INFO("GetLine. res string" << res);
		return res;
	}
	bool LoadScoreFile() {
		ifstream infile;
		for (unsigned i = 0; i < 100; ++i) {
			string file = kHistPath + GetScoreFileName(i);
			infile.open(file.c_str(), ifstream::in);
			if (!infile) {
				MCE_INFO("ScoreCache::LoadClickAndReply. openfile fail:" << file );
			} else {
				MCE_INFO("ScoreCache::LoadClickAndReply. openfile OK:" << file );
				break;
			}
		}
		if (!infile) {
			MCE_INFO("ScoreCache::LoadClickAndReply. openfile all fail");
			return false;
		}

		string line;
		while (getline(infile, line)) {
			HandleScoreLine(line,score_map_);
		}
		return true;
	}
	void HandleScoreLine(const string & line,hash_map<uint64_t, float> & score_map) {
		int to, featureid, click, reply, receive;
		float score;
//		sscanf(line.c_str(), "%d\t%d\t%d\t%d\t%d\t%f", &to, &featureid, &click,
//				&reply, &receive, &score);
		sscanf(line.c_str(), "%d\t%d\t%f", &to, &featureid,  &score);

		uint64_t key = Join2(to, featureid);
		score_map[key] = score;
		++scorecnt;
	}

	hash_map<uint64_t, float> score_map_;
	bool ready_;

};



}
}
#endif
