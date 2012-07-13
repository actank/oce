#include "ScoreCheckWorker.h"
using namespace com::xiaonei::xce;

void help()
{
	cout << "Usage: ScoreCheckWorker [OPTIONS]" << endl;
	cout << "-h, --help\tDisplay this help and exit." << endl;
	cout << "-u, --userid\tCheck users. (follow int userid)" << endl;
	cout << "-d, --days\tCheck users who login in how many days before now. (follow int days, 0 for default,means only today)" << endl;
	cout << "-t, --to\tTo do what. Find wrong sign setting. (follow \"verify\" to print only,\"correct\" to correct them)" << endl;
}

int main (int argc, char * const argv[]) {
	if (argc<2)
	{
		help();
		return 1;
	}

	int u_ = 0;
	bool t_ = false;

	int d_ = 0;

	string sign_check_type = "";
	bool tc_ = false;

#define OPT(_a1, _a2)   else if ( !strcmp(argv[i], _a1) || !strcmp(argv[i], _a2) )
	int i;
	for (i=1; i<argc; i++)
	{
		if ( i == 0);
		OPT("-h", "--help")     	{help(); return 0;}
		else if (i + 1 == argc) break;
		OPT("-u", "--userid")		{u_ = boost::lexical_cast<int>(argv[++i]); }
		OPT("-d", "--days")		{d_ = boost::lexical_cast<int>(argv[++i]); }
		OPT("-t", "--to")		{t_ = true; sign_check_type = argv[++i];}

		else break;
	}
#undef OPT

	if (i!=argc)
	{
		cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
		help();
		return 1;
	}

	if( sign_check_type=="verify" ){
		cout << "Check only." << endl;
		tc_ = false;
	} else if( sign_check_type=="correct" ){
		cout << "Check and Correct." << endl;
		tc_ = true;
	} else{
		cout << "Need check type(\"verify\" or \"correct\")." << endl;
		return 1;
	}

	if( u_ != 0 ){
		cout << "[main] starting task." << endl;
		ScoreCheckWorker worker;
		worker.check(u_,tc_);
		cout << "[main] finished task." << endl;
	}else if ( t_ ){
		if( d_ <= 0 ){
			d_ = 0;
		}
		cout << "[main] starting task." << endl;
		ScoreCheckWorker worker;
		worker.work(d_,tc_);
		cout << "[main] finished task." << endl;
	}
	return 0;
}

