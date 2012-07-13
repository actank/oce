#ifndef FEEDNAIVEBAYESI_FREQCACHE_H_
#define FEEDNAIVEBAYESI_FREQCACHE_H_
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
const static string kFreqPath = "/data/xce/StatUtil/data/freq/";


int freqcnt = 0;
//static void Extract4(uint64_t r, int &a, int &b, int& c, int &d);
//uint64_t Join2(int a, int b) {
//	uint64_t r = 0;
//	r = r | (uint64_t(a) << 32);
//	r = r | (uint32_t) b;
//
//	return r;
//}
//static uint64_t Join4(int a, int b, int c, int d) {
//	MCE_INFO("Join4 a:" << a << " b:" << b << " c:" << c << " d:" << d);
//	uint64_t r = 0;
//	r = (uint64_t(a) << 48) | (uint64_t(b) << 32) | (uint64_t(c) << 16)
//			| (uint64_t(d));
//
//	Extract4(r, a, b, c, d);
//	return r;
//}
//
//static void Extract2(uint64_t r, int & a, int &b) {
//	a = (int) ((r & 0xFFFFFFFF00000000) >> 32);
//	b = (int) (r & 0x00000000FFFFFFFF);
//}
//
//static void Extract4(uint64_t r, int &a, int &b, int& c, int &d) {
//	a = (int) ((r & 0xFFFF000000000000) >> 48);
//	b = (int) ((r & 0x0000FFFF00000000) >> 32);
//	c = (int) ((r & 0x00000000FFFF0000) >> 16);
//	d = (int) ((r & 0x000000000000FFFF));
//	MCE_INFO("Extract4 a:" << a << " b:" << b << " c:" << c << " d:" << d);
//}
//
//
//static string AddZero(int i) {
//  string s;
//  if (i < 10) {
//    s = "0";
//  }
//  s += lexical_cast<string> (i);
//  return s;
//}

static string GetFrequencyFileName(int i) {
	MyUtil::Date DeltaDays(i* 24 * 60 * 60);
	MyUtil::Date today = Date(time(NULL));
	string name;
	MyUtil::Date d = today - DeltaDays;
	ostringstream oss;
	oss << "freq." << d.year() << "-" << AddZero(d.month()) << "-"	<< AddZero(d.day());
	return oss.str();
}


class FrequencyCache: public Singleton<FrequencyCache> {
public:
	FrequencyCache() :
		ready_(true) {
	}
	bool Reload() {
		if (ready_ == false) {
			MCE_INFO("FrequencyCache::Reload. already in loading");
			return false;
		}
		ready_ = false;
		freq_map_.clear();
		scorecnt = 0;
		ready_ = LoadFreqFile();
		MCE_INFO("FrequencyCache::LoadScore done!!! size:" << freq_map_.size()
				<< " freqcnt:" << freqcnt << " ready:" << ready_);
		return ready_;
	}

	int GetFrequency(int fromid) {
		hash_map<int, int>::const_iterator it = freq_map_.find(fromid);
		if(it != freq_map_.end()){
			return it->second;
		}else{
			return 0;
		}
	}
	bool ready() {
		return ready_;
	}
private:
	bool LoadFreqFile() {
		hash_map<uint64_t, float> score_map;

		ifstream infile;
		for (unsigned i = 0; i < 20; ++i) {
			string file = kFreqPath + GetFrequencyFileName(i);
			infile.open(file.c_str(), ifstream::in);
			if (!infile) {
				MCE_INFO("FrequencyCache::LoadFreqFile. openfile fail:" << file );
			} else {
				MCE_INFO("FrequencyCache::LoadFreqFile. openfile OK:" << file );
				break;
			}
		}
		if (!infile) {
			MCE_INFO("FrequencyCache::LoadFreqFile. openfile all fail");
			return false;
		}

		string line;
		while (getline(infile, line)) {
			HandleFreqLine(line);
		}
		return true;
	}
	void HandleFreqLine(const string & line) {
		int fromid, freq;
		sscanf(line.c_str(), "%d\t%d", &fromid, &freq);
		freq_map_[fromid] = freq;
		++freqcnt;
	}

	hash_map<int, int> freq_map_;
	bool ready_;
};


}
}
#endif
