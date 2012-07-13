#ifndef __WINDOWS_LIVE_ADAPTER_H__
#define __WINDOWS_LIVE_ADAPTER_H__

#include "WindowsLiveImporter.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace importer {
namespace adapter {

class WindowsLiveImporterAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<
		MyUtil::ImporterChannel, WindowsLiveImporterAdapter> {
public:
	WindowsLiveImporterAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}
	xce::importer::WindowsLiveImporterManagerPrx getWindowsLiveImporterManager(
			int userId);

private:
	virtual string name() {
		return "WindowsLiveImporter";
	}
	virtual string endpoints() {
		return "@WindowsLiveImporter";
	}
	virtual size_t cluster() {
		return 10;
	}

	std::vector<WindowsLiveImporterManagerPrx> _managers;
};

}
}
}

#endif

