#include "ServiceI.h"
#include "UserTimeAdapter.h"

#include "UserContactAdapter.h"

#include "boost/lexical_cast.hpp"
using namespace std;
int main()
{
	ifstream ifile;
	ifile.open("tt", ios::in);
	if (!ifile){
		cout<<"open fild error"<<endl;
	}
	MyUtil::IntSeq ids;
	ostringstream content;
	char buf[1024];
	while(ifile.good()){
		ifile.getline(buf,1024);
		cout<<buf<<endl;
		if( strcmp(buf,"") == 0 )
		{
			break;
		}
		ids.push_back( boost::lexical_cast<int>( buf ) );
	}
	ifile.close();
	ifile.clear();
	cout<<"fuck size "<<ids.size()<<endl;
	int count = 0;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
	{
		if(++count == 100)
		{
			count = 0;
			sleep(1);
		}
		try
		{
		xce::adapter::usertime::UserTimeAdapter::instance().reload(*it);
		xce::adapter::usercontact::UserContactAdapter::instance().reload(*it);
		}catch( ... )
		{
			cout<<"fuck id "<<*it<<endl;
		}

	}
	return 1;
}

