#ifndef __ICON2URL_DATA_WRITER_ADAPTER_H__
#define __ICON2URL_DATA_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "VipInfo.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace icon2urldata{

class Icon2UrlDataWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::Icon2UrlDataWriterPrx>,
	public MyUtil::Singleton<Icon2UrlDataWriterAdapter>, IceUtil::Thread
{
public:
	Icon2UrlDataWriterAdapter();

	void reload(int id);
	
	void reload(const MyUtil::LongSeq& ids);

protected:

	void run();

private:

	xce::vipinfo::Icon2UrlDataWriterPrx getIcon2UrlDataWriter(int id);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	__gnu_cxx::hash_set<int> _idSet;
};

}
}
}

#endif

