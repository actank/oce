/*
 * AppendReceive.h
 *
 *  Created on: Jun 5, 2011
 *      Author: yejingwei
 */


#include "Merger.h"
using namespace xce::feed;
int main(int argc, char ** argv){
	if(argc != 4){
		cout << "err argc:" << argc<< endl;
	}
	cout << string(argv[1]) << " " << string(argv[2]) << " " << string(argv[3]) << endl;
	Merger::instance().Init(argv[1],argv[2],argv[3]);
	Merger::instance().BeginScan();
	Merger::instance().Merge();
	return 1;
}

