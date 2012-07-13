#include "WindowsLiveImporterAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::importer;
using namespace xce::importer::adapter;
using namespace std;

//---------------------------------------------------------------------------
WindowsLiveImporterManagerPrx WindowsLiveImporterAdapter::getWindowsLiveImporterManager(int id) {
	return locate<WindowsLiveImporterManagerPrx>(_managers, "M", id, MyUtil::TWO_WAY);
}

