#include "sysinfo.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace MceMonitor::Item;

string sysinfo::readfile(string filename)
{
	ostringstream o;
	while(true)
	{
		usleep(500);
		{
			FILE *fp;
			int status;
			char tmp[PATH_MAX];
			fp = fopen( filename.c_str(), "r" );
			if (fp == NULL)
			{
				cout << "open failed." << endl;
				usleep(1000);
				continue;
			}
			while (fgets(tmp, PATH_MAX, fp) != NULL )
			{
				o << tmp;
			}
			status = pclose(fp);
			//if (status == -1) continue;else break;
			break;
		}
	}
	return o.str();
}

