#include "ServiceI.h"
#include "VipIconDataReaderAdapter.h"

using namespace xce::adapter::vipicondata;
using namespace xce::vipinfo;

int main(int argc, char * argv[])
{
	const int OLD_SERVER = 0;
	const int NEW_SERVER = 1;
	int id = 23;
	int server = NEW_SERVER;
	if(argc > 1){
		id = atoi(argv[1]);
	}
	if(argc == 3){
                server = atoi(argv[2]);
        }
	std::vector<int> ids;
	ids.push_back(id);
//	ids.push_back(0);
//	ids.push_back(23);
//	ids.push_back(1002);
//	ids.push_back(1015);
//	ids.push_back(1018);
//	ids.push_back(1032);
//	ids.push_back(1038);
//	ids.push_back(1045);
//	ids.push_back(1050);
//	ids.push_back(1063);
//	ids.push_back(1075);
//	ids.push_back(1084);
//	ids.push_back(1089);
//	ids.push_back(1090);
//	ids.push_back(1091);
//	ids.push_back(1093);
//	ids.push_back(1094);
//	ids.push_back(1107);
//	ids.push_back(1115);
//	ids.push_back(1121);
	std::vector<int> types;
	types.push_back(0);
	//while(true)
	{
		try{
			if(server == OLD_SERVER)
			{
				VipIconDataSeqMap iconData = VipIconDataReaderAdapter::instance().getVipIconDataSeqMap(ids, types);
        	                for(VipIconDataSeqMap::const_iterator mIt = iconData.begin(); mIt != iconData.end(); ++mIt){
        	                        cout << "\nIconData:" << mIt->first << endl;
        	                        for(VipIconDataSeq::const_iterator sIt = mIt->second.begin(); sIt != mIt->second.end(); sIt ++){
        	                                cout << "[id:" << (*sIt)->id;
        	                                cout << ", type:" << (*sIt)->type;
        	                                cout << ", level:" << (*sIt)->level;
        	                                cout << ", status:" << (*sIt)->status;
        	                                cout << ", iconId:" << (*sIt)->iconId;
        	                                cout << ", hideUrl:" << (*sIt)->hideUrl;
        	                                cout << "]" << endl;
        	                        }
        	                }
			}else{
				VipFeedDataSeqMap feedData = VipIconDataReaderAdapter::instance().getVipFeedDataSeqMap(ids, types);
				for(VipFeedDataSeqMap::const_iterator mIt = feedData.begin(); mIt != feedData.end(); ++mIt){
					cout << "\nFeedData:" << mIt->first << endl;
					for(VipFeedDataSeq::const_iterator sIt = mIt->second.begin(); sIt != mIt->second.end(); sIt ++){
						cout << "metaData=[id:" << (*sIt)->member->id;
						cout << ", type:" << (*sIt)->member->type;
						cout << ", level:" << (*sIt)->member->level;
						cout << ", status:" << (*sIt)->member->status;
						cout << "]" << endl;
        	                                cout << "iconData=[iconId:" << (*sIt)->icon->iconId;
        	                                cout << ", hideUrl:" << (*sIt)->icon->hideUrl;
        	                                cout << "]" << endl;
        	                                cout << "hatData=[hatId:" << (*sIt)->hat->hatId;
        	                                cout << ", hideUrl:" << (*sIt)->hat->hideUrl;
        	                                cout << ", status:" << (*sIt)->hat->status;
        	                                cout << "]" << endl;
					}
				}
			}
		}catch(Ice::Exception& e)
		{
			std::cout<<e.what()<<std::endl;
		}

	}
	return 0;
}
