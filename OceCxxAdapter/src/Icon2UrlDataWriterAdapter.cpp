#include "Icon2UrlDataWriterAdapter.h"

namespace xce
{
namespace adapter
{
namespace icon2urldata
{

Icon2UrlDataWriterAdapter::Icon2UrlDataWriterAdapter() :  MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::Icon2UrlDataWriterPrx>
														  ("ControllerVipInfoWriter", 120,300,new MyUtil::OceChannel,true,"IU")
{
	start();
}

xce::vipinfo::Icon2UrlDataWriterPrx Icon2UrlDataWriterAdapter::getIcon2UrlDataWriter(int id)
{
	return getProxy(id);
}

void Icon2UrlDataWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void Icon2UrlDataWriterAdapter::reload(const MyUtil::LongSeq& ids)
{
	if(ids.empty())
	{
		MCE_WARN("Icon2UrlDataWriterAdapter::reload Exception id size = 0");
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(ids.begin(), ids.end());
	_mutex.notify();
}

void Icon2UrlDataWriterAdapter::run()
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
		
		xce::vipinfo::Icon2UrlDataWriterPrx prx = getIcon2UrlDataWriter(0);
		if(prx)
		{
			try
			{
				prx->reload(ids);
			}catch(Ice::Exception& e)
			{
				MCE_WARN("Icon2UrlDataWriterAdapter::run reload Exception id size = " << ids.size() <<" " << e.what());
			}
		}
	}
}


}
}
}
