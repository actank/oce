#include <iostream>

#include "socialgraph/recommendbyinfo/cpp/recommendbyinfo.h"
#include "socialgraph/socialgraphutil/clogging.h"

using namespace xce::socialgraph;

int main(int argc, char* argv[]) {
	InitLogger("RecommendByInfoNew");
	if (argc != 2) {
		CLOG(ERROR) << "Error input parameter!";
		return 0;
	}
	
	RecommendByInfo rbi = RecommendByInfo(std::string(argv[1]));
	rbi.Calc();
}
