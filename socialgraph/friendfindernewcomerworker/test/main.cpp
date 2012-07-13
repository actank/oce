#include "socialgraph/socialgraphutil/mcclient.h"
#include "item.pb.h"
#include <iostream.h>
#include "FriendFinderNewComerWorkerAdapter.h"

using namespace std;
using namespace xce;
using namespace xce::socialgraph;
using namespace xce::socialgraph::adapter;

/*
void write(int userId,const map<int,vector<int> >& data){
	xce::socialgraph::Items items;
	for( map<int,vector<int> >::const_iterator itMap = data.begin(); itMap != data.end(); ++itMap ){
		for( vector<int>::const_iterator itVec = itMap->second.begin(); itVec != itMap->second.end(); ++itVec ){
			xce::socialgraph::Item* item_newcomer = items.add_items();
			item_newcomer->set_id(*itVec);
			item_newcomer->set_field(itMap->first);
			item_newcomer->set_message("朝阳二外");
		}
	}

	char* chs = new char[items.ByteSize()];
	items.SerializeToArray(chs,items.ByteSize());
	vector<char> binary_data;
	for (int i = 0; i < items.ByteSize(); ++i) {
		binary_data.push_back(chs[i]);
	}
	delete[] chs;
        ostringstream key;
        key << "FFNCW_" << userId;
        MemcachedClient::GetInstance().Set(key.str(), binary_data);
}
*/

int main(int argc, char* argv[]) {
	int new_comer_id = boost::lexical_cast<int>( argv[1] );
	try {
                FriendFinderNewComerWorkerAdapter::instance().addNewComer( new_comer_id );
                cout << "[deployByDays] addNewComer(" << new_comer_id << ")" << endl;
        } catch( Ice::Exception e ){
                cout << "[deployByDays] addNewComer(" << new_comer_id << ") found Ice::Exception: " << e.what() << endl;
        } catch( ... ){
                cout << "[deployByDays] addNewComer(" << new_comer_id << ") found unknown exception: " << endl;
        }

}

