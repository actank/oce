/*
 * AppendReceive.h
 *
 *  Created on: Jun 5, 2011
 *      Author: yejingwei
 */

#ifndef MERGESCORE_H_
#define MERGESCORE_H_
#include <map>
#include <ext/hash_map>
#include <iostream>
#include <fstream>
#include "Singleton.h"
#include <cstdio>
#include "../CommonUtil/CommonUtil.h"
using namespace std;

#define DoLog(msg) std::cout << msg << std::endl;;
namespace xce {
namespace feed {
using  __gnu_cxx::hash_map;
//
//actor stype count

//99998920        -701    1       0       10      0.0833333333333

//uint64_t Join2(int a, int b) {
//	uint64_t r = 0;
//	r = r | (uint64_t(a) << 32);
//	r = r | (uint32_t) b;
//	return r;
//}
//
//void Extract2(uint64_t r, int & a, int &b) {
//	a = (int) ((r & 0xFFFFFFFF00000000) >> 32);
//	b = (int) (r & 0x00000000FFFFFFFF);
//}

int scorecnt = 0;
class Merger: public MyUtil::Singleton<Merger> {
public:
	void Init(const string & histlog, const string & lastlog,
			const string & outlog) {
		histlog_ = histlog;
		lastlog_ = lastlog;
		outlog_ = outlog;
	}
	void BeginScan() {
	 	ScanDispatch(histlog_,false);
		cout << "Scan hist done" << endl;
	 	ScanDispatch(lastlog_,false);
		cout << "Scan last done" << endl;

	}
	void ScanDispatch(const string & dispatchlog, bool last_or_history) {
		ifstream infile;
		infile.open(dispatchlog.c_str(), ifstream::in);
		if (!infile) {
			DoLog("LogCache::LoadClickAndReply. openfile fail");
		}
		string line;
		while (getline(infile, line)) {
			ScanLine(line,last_or_history);
		}
	}
	void ScanLine(const string & line, bool last_or_history) {
		int to, featureid,click,reply,rec;
		float score;
		sscanf(line.c_str(), "%d\t%d\t%d\t%d\t%d\t%f", &to, &featureid,&click,&reply,&rec,&score);
		//sscanf(line.c_str(), "%d\t%d\t%f", &to, &featureid, &score);
		uint64_t key = Join2(to, featureid);
		if (last_or_history) {
			last_[key] = score;
		} else {
			history_[key] = score;
		}
		++scorecnt;
	}
	void Merge() {
		DoLog("Merge. Begin");
	  ofstream myfile (outlog_.c_str());
	  if (!myfile.is_open())
	  {
	  	DoLog("Merge. Open file fail:" << outlog_);
	  	return;
	  }


		for (hash_map<uint64_t, float>::iterator li = last_.begin(); li
				!= last_.end();) {
			hash_map<uint64_t, float>::iterator hi = history_.find(li->first);
			int to, feature;
			Extract2(li->first, to, feature);
			if (hi == history_.end()) {
				myfile <<to << "\t" << feature << "\t" << li->second << "\n";
			} else {
				float score = (li->second + 0.95 * hi->second) / 1.95;
				myfile << to << "\t" << feature << "\t" << score << "\n";
				history_.erase(hi);
			}
			last_.erase(li++);
		}
		for (hash_map<uint64_t, float>::iterator hi = history_.begin(); hi
				!= history_.end(); ++hi) {
			int to, feature;
			Extract2(hi->first, to, feature);
			myfile <<  to << "\t" << feature << "\t" << hi->second << "\n";
		}
	}
private:
	hash_map<uint64_t, float> history_, last_;
	string histlog_, lastlog_, outlog_;
};

}
}
#endif /* APPENDRECEIVE_H_ */
