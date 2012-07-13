#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "DbCxxPool/src/QueryRunner.h"
#include "socialgraph/autogroup/algorithm/executer.h"
#include "socialgraph/autogroup/algorithm/methodfactory.h"
#include "base/logging.h"

#include "item.pb.h"

using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

int main(int argc, char* argv[]) {
	ConnectionPoolManager::instance().initialize();   //for main test
	//std::string file = "/data/xce/Project/AutoGroup/ids";
	//std::ifstream infile(file.c_str(), ifstream::in);

	//std::string line = "";
	//while (getline(infile, line)) {
	//	boost::trim(line);
	//	int userid = boost::lexical_cast<int>(line);
	//	Executer executer;
	//	executer.SetAutoGroupInstance(MethodFactory::GetInstance().Create(GroupingWithLabel));
	//	executer.Grouping(userid);

	//	executer.Display();
	//}
	
	int userid = 238111132;
	Executer executer;
	executer.SetAutoGroupInstance(MethodFactory::GetInstance().Create(NewGroupingWithLabel));
	executer.Grouping(userid);

	executer.Display();

	//Items result;
	//executer.GetGroupResult(result);

	//for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator citer = result.items().begin();
	//		citer != result.items().end(); ++citer) {
	//	std::ostringstream labels;
	//	for (::google::protobuf::RepeatedPtrField< ::std::string >::const_iterator miter = citer->messages().begin();
	//			miter != citer->messages().end(); ++miter) {
	//		//LOG(INFO) << *miter << " ";
	//		labels << *miter << " ";
	//	}
	//	LOG(INFO) << labels.str();

	//	std::ostringstream idlist;
	//	for (::google::protobuf::RepeatedField< ::google::protobuf::int32>::const_iterator fiter = citer->fields().begin();
	//			fiter != citer->fields().end(); ++fiter) {
	//		idlist << *fiter << " ";
	//	}
	//	LOG(INFO) << idlist.str();
	//	LOG(INFO) << "";
	//}

	//std::vector<unsigned char> binary_data;
	//int byte_size = result.ByteSize();	
	//LOG(INFO) << "byte_size : " << byte_size;
	//binary_data.resize(byte_size);
	//result.SerializeToArray(binary_data.data(), byte_size);
	//
	//LOG(INFO)	<< "binary_data size : " << binary_data.size();
}
