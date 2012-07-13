#ifndef __HAT_DATA_WRITER_ADAPTER_H__
#define __HAT_DATA_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "VipInfo.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace hatdata{

class HatDataWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::HatDataWriterPrx>,
	public MyUtil::Singleton<HatDataWriterAdapter>, IceUtil::Thread
{
public:
	HatDataWriterAdapter();

	void reload(int id);
	
	void reload(const MyUtil::LongSeq& ids);

protected:

	void run();

private:

	xce::vipinfo::HatDataWriterPrx getHatDataWriter(int id);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	__gnu_cxx::hash_set<int> _idSet;
};

}
}
}

#endif

