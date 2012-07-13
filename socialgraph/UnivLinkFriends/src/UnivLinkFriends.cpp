#include "UnivLinkFriends.h"
#include "BuddyByAddTimeReplicaAdapter.h"
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;

static int cluster = 100;

UnivLinkFriends::UnivLinkFriends( string fileNamePatt) {
	cout<<"[UnivLinkFriends::UnivLinkFriends] ConnectionPoolManager initializing" << endl;
	ConnectionPoolManager::instance().initialize();
	cout<<"[UnivLinkFriends::UnivLinkFriends] ConnectionPoolManager initialize done" << endl;

	_succ = true;
	for( int i=0;i<cluster;++i ){
		ostringstream filename;
		filename << fileNamePatt << i;
		//filename << "/mnt/dsk11/sg/PREFIX_" << i;
		ofstream* tmp = new ofstream( filename.str().c_str() );
		_ofss.push_back( tmp );
        	if( _ofss[i]->is_open() ){
        		cout << "opening file: " << filename.str() << endl;
        	} else{
			_succ = false;
                	cout << "Error opening file" << filename.str() << endl;
		}
	}
}

UnivLinkFriends::~UnivLinkFriends(){
	for( int i=0;i<cluster;++i ){
		if( _ofss[i]->is_open() ){
			_ofss[i]->close();
		}
		delete _ofss[i];
	}
}

void UnivLinkFriends::collectUnivData(){
	if( !_succ ){
		cout << "[UnivLinkFriends::collectUnivData] File Not Open!" << endl;
		return;
	}
	int beginTime = time(NULL);
	cout << "[UnivLinkFriends::collectUnivData] start in university_info at time: " << beginTime << endl;
	int countTotal = 0;

	int max_id = 0;
	int last_id = 0;
	do {
		map<int, map<int,UnivData> > uds;
		Statement sql;
		sql	<< "SELECT userid,university_name,department,enroll_year FROM university_info "
			<< " WHERE userid >= " << max_id
			<< " LIMIT 100000";
		size_t count = QueryRunner("university_info_friendfinder", CDbRServer).query(sql,
				UnivDataByIDHandler(last_id,uds));
		countTotal += count;

		map<int, map<int,UnivData> > uds_linked = linkFriends( uds );

		writeFile( uds_linked );

		if (last_id > max_id) {
			max_id = last_id;
		} else if( count > 0) {
			++max_id;
			++last_id;
		} else{
			break;
		}
		int thisTime = time(NULL);
		cout << "[UnivLinkFriends::collectUnivData] collecting to " << last_id << " count: " << countTotal << " timecost: " << (thisTime-beginTime)/60/60 << "h" << ((thisTime-beginTime)/60)%60 << "m" << endl;
	} while (true);

	int endTime = time(NULL);
	cout << "[UnivLinkFriends::collectUnivData] ended at time: " << endTime << ". cost : " << (endTime-beginTime)/60/60 << "h" << ((endTime-beginTime)/60)%60 << "m" << endl;
}

map<int, map<int,UnivData> > UnivLinkFriends::linkFriends(map<int, map<int,UnivData> >& uds){
	map<int, map<int,UnivData> > res;
	int bTotal = 0;
	int aTotal = 0;
	for (map<int, map<int,UnivData> >::iterator itMap1 = uds.begin(); itMap1
			!= uds.end(); ++itMap1) {
		IntSeq ids;
		for ( map<int,UnivData>::iterator itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); ++itMap2) {
			ids.push_back( itMap2->first );
		}
		bTotal += itMap1->second.size();

		map<int,string> friends;

		MyUtil::Int2IntSeqMap buddies = xce::buddy::adapter::BuddyByAddTimeCacheAdapter::instance().getFriendLists(ids);
		for( MyUtil::Int2IntSeqMap::iterator it1=buddies.begin(); it1!=buddies.end(); ++it1 ){
			if( it1->second.size() <= 0 ){
				continue;
			}
			string& guestStr = friends[it1->first];
			for( MyUtil::IntSeq::iterator it2=it1->second.begin(); it2!=it1->second.end(); ++it2 ){
				guestStr = guestStr + "" + boost::lexical_cast<string>(*it2) + ",";
			}
		} 

		aTotal += friends.size();

		cout << "[UnivLinkFriends::linkFriends] linking mod " << itMap1->first << " size/fSize: " << itMap1->second.size() << "/" << friends.size() << endl;

		for ( map<int,string>::iterator itMap3 = friends.begin(); itMap3 != friends.end(); ++itMap3) {
			map<int,UnivData>::iterator itMap4 = itMap1->second.find( itMap3->first );
			UnivData ud = itMap4->second;
			ud.has_friends = true;
			ud.friend_list = itMap3->second;

			res[itMap1->first].insert( make_pair(itMap4->first,ud) );
		}
	}

	cout << "[UnivLinkFriends::linkFriends] size/linkedSize: " << bTotal << "/" << aTotal << endl;

	return res;
}

/*
map<int, map<int,UnivData> > UnivLinkFriends::linkFriends(map<int, map<int,UnivData> >& uds){
	map<int, map<int,UnivData> > res;
	int bTotal = 0;
	int aTotal = 0;
	for (map<int, map<int,UnivData> >::iterator itMap1 = uds.begin(); itMap1
			!= uds.end(); ++itMap1) {
		int min = 0;
		int max = 0;
		map<int,UnivData>::iterator itMap2 = itMap1->second.begin();
		if( itMap2 != itMap1->second.end() ){
			min = itMap2->first;
			max = min+1;
		} else{
			continue;
		}
		for ( ++itMap2; itMap2 != itMap1->second.end(); ++itMap2) {
			if( itMap2->first < min ){
				min = itMap2->first;
			}
			if( itMap2->first >= max ){
				max = itMap2->first + 1;
			}
		}
		bTotal += itMap1->second.size();

		map<int,string> friends;

		Statement sql;
		sql     << "SELECT host,guest"
			<< " FROM relation_" << itMap1->first
			<< " WHERE host >= " << min << " AND host < " << max;
		ostringstream pattern;
		pattern << "relation_" << itMap1->first;
		//QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
		QueryRunner("friendfinder_friend", CDbRServer, pattern.str()).query(
				sql, FriendsHandler( friends ));

		for ( map<int,string>::iterator itMap3 = friends.begin(); itMap3 != friends.end(); ++itMap3) {
			map<int,UnivData>::iterator itMap4 = itMap1->second.find( itMap3->first );
			if( itMap4 == itMap1->second.end() ){
				continue;
			}
			UnivData ud = itMap4->second;
			ud.has_friends = true;
			ud.friend_list = itMap3->second;

			res[itMap1->first].insert( make_pair(itMap4->first,ud) );
			aTotal++;
		}

		cout << "[UnivLinkFriends::linkFriends] linking mod " << itMap1->first << " size/fSize: " << itMap1->second.size() << "/" << friends.size() << endl;

	}

	cout << "[UnivLinkFriends::linkFriends] size/linkedSize: " << bTotal << "/" << aTotal << endl;

	return res;
}
*/

/*
map<int, map<int,UnivData> > UnivLinkFriends::linkFriends(map<int, map<int,UnivData> >& uds){
	map<int, map<int,UnivData> > res;
	int bTotal = 0;
	int aTotal = 0;
	for (map<int, map<int,UnivData> >::iterator itMap1 = uds.begin(); itMap1
			!= uds.end(); ++itMap1) {
		ostringstream ids;
		map<int,UnivData>::iterator itMap2 = itMap1->second.begin();
		if( itMap2 != itMap1->second.end() ){
			ids << itMap2->first;
		} else{
			continue;
		}
		for ( ++itMap2; itMap2 != itMap1->second.end(); ++itMap2) {
			ids << "," << itMap2->first;
		}
		bTotal += itMap1->second.size();

		map<int,string> friends;

		Statement sql;
		sql     << "SELECT host,guest"
			<< " FROM relation_" << itMap1->first
			<< " WHERE host IN (" << ids.str() << ")";
		ostringstream pattern;
		pattern << "relation_" << itMap1->first;
		QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
				sql, FriendsHandler( friends ));

		aTotal += friends.size();

		cout << "[UnivLinkFriends::linkFriends] linking mod " << itMap1->first << " size/fSize: " << itMap1->second.size() << "/" << friends.size() << endl;

		for ( map<int,string>::iterator itMap3 = friends.begin(); itMap3 != friends.end(); ++itMap3) {
			map<int,UnivData>::iterator itMap4 = itMap1->second.find( itMap3->first );
			UnivData ud = itMap4->second;
			ud.has_friends = true;
			ud.friend_list = itMap3->second;

			res[itMap1->first].insert( make_pair(itMap4->first,ud) );
		}
	}

	cout << "[UnivLinkFriends::linkFriends] size/linkedSize: " << bTotal << "/" << aTotal << endl;

	return res;
}
*/

void UnivLinkFriends::writeFile(std::map<int, std::map<int,UnivData> >& uds){
	for (map<int, map<int,UnivData> >::iterator itMap1 = uds.begin(); itMap1
			!= uds.end(); ++itMap1) {
		cout << "[UnivLinkFriends::writeFile] ------ writing at file: " << itMap1->first << " size: " << itMap1->second.size() << endl;
		for (map<int,UnivData>::iterator itMap2 = itMap1->second.begin(); itMap2
				!= itMap1->second.end(); ++itMap2) {
			ostringstream ctx;
			ctx	<< itMap2->second.userid << "\t"
				<< itMap2->second.university_name << "\t"
				<< itMap2->second.department << "\t"
				<< itMap2->second.enroll_year << "\t"
				<< itMap2->second.friend_list << "\t"
				<< "\n";
        		*(_ofss[itMap1->first]) << ctx.str();
		}
	}
}


bool UnivDataByIDHandler::handle(mysqlpp::Row& row) const{

        int userid = row["userid"];
        string university_name = boost::lexical_cast<string>(row["university_name"]);
        string department = boost::lexical_cast<string>(row["department"]);
        int enroll_year = row["enroll_year"];

	UnivData ud;
	ud.userid = userid;

	string pat = " ";
	bool un_invalid =  (university_name=="") || (university_name.compare(0, pat.size(), pat)==0);
	bool d_invalid =  (department=="") || (department.compare(0, pat.size(), pat)==0);

	ud.university_name = un_invalid ? "uNull" : university_name;
	ud.department = d_invalid ? "dNull" : department;

	ud.enroll_year = enroll_year;

	ud.has_friends = false;
	ud.friend_list = "fNull";

	_results[userid%cluster].insert( make_pair(userid,ud) );

	_last_id = userid > _last_id ? userid : _last_id;

	return true;
}

bool FriendsHandler::handle(mysqlpp::Row& row) const{

        int host = row["host"];
        int guest = row["guest"];
	string& guestStr = _results[host];
	guestStr = guestStr + "" + boost::lexical_cast<string>(guest) + ",";

	return true;
}

