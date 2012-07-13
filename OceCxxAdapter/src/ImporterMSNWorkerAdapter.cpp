
#include "ImporterMSNWorkerAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::importer;
using namespace xce::importer::adapter;
using namespace std;

//---------------------------------------------------------------------------
ImporterMSNWorkerPrx ImporterMSNWorkerAdapter::getImporterMSNWorker(int id) {

	return ImporterMSNWorkerPrx::uncheckedCast(locate<ImporterMSNWorkerPrx>(_managers, "M", id, MyUtil::TWO_WAY, 60*2*1000)->ice_connectionCached(false));
}

