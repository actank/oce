/*
 * AppendReceive.h
 *
 *  Created on: Jun 5, 2011
 *      Author: yejingwei
 */

#include "AppendReceive.h"


using namespace xce::feed;
int main(int argc, char ** argv){
	if(argc != 4){
		cout << "err argc:" << argc<< endl;
	}

	cout << string(argv[1]) << endl;
	Appender::instance().ScanDispatch(string(argv[1]));
	Appender::instance().ScanClickReply(string(argv[2]),string(argv[3]));
	return 1;
}
