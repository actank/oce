#include <OfferFriendsAdapter.h>

using namespace std;
using namespace MyUtil;
using namespace xce::offerfriends::adapter;

static void split(std::string src, std::string token, vector<std::string>& vect)    
{    
    int nend=0;    
    int nbegin=0;    
    while(nend != -1)    
    {    
        nend = src.find_first_of(token, nbegin);    
        if(nend == -1){
            vect.push_back(src.substr(nbegin, src.length()-nbegin));    
		cout << "testing " << nend-nbegin;
	} else{
            vect.push_back(src.substr(nbegin, nend-nbegin));    
		cout << "testing " << nend-nbegin;
	}
        nbegin = nend + 1;    
    }    
}

vector<int> test_readfile(const char* filename)
{
        //cout << "opening file" << endl;
        vector<int> nodelist;
        ifstream ifs(filename);
        if( !ifs.is_open())
        {
                cout << "Error opening file" << endl;
                return nodelist;
        }
        string buffer;
        while (getline(ifs,buffer)){
                try{
			/*
                        string line=boost::lexical_cast<string>(buffer);
			vector<string> names;
			split(line,"|",names); 
			if( names.size() != 5 ){
				cout << " what?? " << line << endl;
			}
			LoginNode tmp;
			tmp.id=boost::lexical_cast<int>(line[0]);
			tmp.h=boost::lexical_cast<int>(line[1]);
			tmp.c=boost::lexical_cast<int>(line[2]);
			tmp.time=boost::lexical_cast<long>(line[3]);
			tmp.type=boost::lexical_cast<int>(line[4]);
			nodelist.push_back(tmp);
                        cout << "read user: " << tmp.id << " " << tmp.h << " " << tmp.c << " " << tmp.time << " " << tmp.type << endl;
                        nodelist.push_back(tmp);
			*/
                        string line = boost::lexical_cast<string>(buffer);
                        int id = boost::lexical_cast<int>(line);
                        nodelist.push_back(id);
                }catch(...){
                        //cout << "skipped bad ID: " << buffer << endl;
                }
        }
        ifs.close();
        return nodelist;
}

int main(int argc, char* argv[]) {
	vector<int> nodes = test_readfile("reload.ids");
	for( vector<int>::iterator it = nodes.begin(); it != nodes.end(); ++it ){
                try{
			cout << *it << " " << OfferFriendsCacheAdapter::instance().getFastOffer(*it,-1).size() << endl;
                }catch(...){
                }
	}
/*
	cout << OfferFriendsCacheAdapter::instance().getBestOffer(24496255,10)[1].userId << endl;
	cout << OfferFriendsCacheAdapter::instance().getFastOffer(24496255,10)[1].userId << endl;
	cout << OfferFriendsCacheAdapter::instance().getRandomOffer(24496255,10)[1].userId << endl;
	OfferFriendsLoaderAdapter::instance().apply(24496255,284647784);
	OfferFriendsLoaderAdapter::instance().accept(24496255,270116734);
	OfferFriendsLoaderAdapter::instance().denyOffer(24496255,49536);
	MyUtil::IntSeq ids;
	ids.push_back(316932644);
	ids.push_back(68126);
	OfferFriendsLoaderAdapter::instance().ignoreOffers(24496255,ids);
*/

/*
	vector<LoginNode> nodes = test_readfile("reload.ids");
	MyUtil::IntSeq tmp;
	for( MyUtil::IntSeq::iterator it = userIds.begin(); it != userIds.end(); ++it ){
		tmp.push_back( *it );
		if( tmp.size() >= 1000 ){
			cout << "reloadBatch size=" << tmp.size() << endl;
			ScoreLoaderAdapter::instance().reloadBatch(tmp);
			cout << "reloadBatch done" << endl;
			tmp = (MyUtil::IntSeq());
		}
	}
	if( tmp.size() >= 1 ){
		cout << "reloadBatch size=" << tmp.size() << endl;
		ScoreLoaderAdapter::instance().reloadBatch(tmp);
		cout << "reloadBatch done" << endl;
		tmp = (MyUtil::IntSeq());
	}
	return 0;
*/
/*
	vector<LoginNode> nodes = test_readfile("reload.ids");
	for( vector<LoginNode>::iterator it = nodes.begin(); it != nodes.end(); ++it ){
		cout << "setLoginDays user: " << it->id << " " << it->h << " " << it->c << " " << it->time << " " << it->type << endl;
		//ScoreLoaderAdapter::instance().setLoginDays(it->id,it->h,it->c,it->time,it->type);
	}
	return 0;
*/
/*
	long time = 1273939318231;
	ScoreLoaderAdapter::instance().setLoginDays(243532523,88,73,time,4);
*/
}

