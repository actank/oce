#ifndef __IMPORTER_MSN_WORKER_ADAPTER_H__
#define __IMPORTER_MSN_WORKER_ADAPTER_H__

#include "ImporterMSNWorker.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace importer {
namespace adapter {

class ImporterMSNWorkerAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<
		MyUtil::ImporterChannel, ImporterMSNWorkerAdapter> {
public:
//	ImporterMSNWorkerAdapter() {
//	}
	xce::importer::ImporterMSNWorkerPrx getImporterMSNWorker(int userId);

private:
	virtual string name() {
		return "ImporterMSNWorker";
	}
	virtual string endpoints() {
		return "@ImporterMSNWorker";
	}
	virtual size_t cluster() {
		return 0;
	}

	std::vector<ImporterMSNWorkerPrx> _managers;
};

}
}
}

#endif

