#include "HatDataWriterAdapter.h"

namespace xce
{
namespace adapter
{
namespace hatdata
{

HatDataWriterAdapter::HatDataWriterAdapter() :  MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::HatDataWriterPrx>
														  ("ControllerVipInfoWriter", 120,300,new MyUtil::OceChannel,true,"IH")
{
	start();
}

xce::vipinfo::HatDataWriterPrx HatDataWriterAdapter::getHatDataWriter(int id)
{
	return getProxy(id);
}

void HatDataWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void HatDataWriterAdapter::reload(const MyUtil::LongSeq& ids)
{
	if(ids.empty())
	{
		MCE_WARN("HatDataWriterAdapter::reload Exception id size = 0");
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(ids.begin(), ids.end());
	_mutex.notify();
}

void HatDataWriterAdapter::run()
{
	while(true)
	{
		MyUtil::IntSeq ids;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
			if(_idSet.empty())
			{
				_mutex.wait();
			}
			ids.insert(ids.end(), _idSet.begin(), _idSet.end());
			_idSet.clear();
		}
		
		xce::vipinfo::HatDataWriterPrx prx = getHatDataWriter(0);
		if(prx)
		{
			try
			{
				prx->reload(ids);
			}catch(Ice::Exception& e)
			{
				MCE_WARN("HatDataWriterAdapter::run reload Exception id size = " << ids.size() <<" " << e.what());
			}
		}
	}
}


}
}
}
