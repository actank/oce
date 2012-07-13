/*
 * PreferStat.h
 *
 *  Created on: Dec 15, 2011
 *      Author: rong.tang
 */

//./Prefer ../data/dispatch/dispatch.2011-12-15 ../data/clickreply/clickreply.2011-12-15 ../data/score/score.2001-12-15

#include "PreferStat.h"
using namespace xce::feed;

int main(int argc, char ** argv) { 
	if(argc != 4){
		cout << "err argc:" << argc<< endl;
	}

	cout << string(argv[1]) << endl;
	PreferStat::instance().ProcessDispatch(string(argv[1]));
  PreferStat::instance().ProcessClickReply(string(argv[2]),string(argv[3]));
	
  PreferStat::instance().Process(string(argv[3]));
	
  return 1;
}




