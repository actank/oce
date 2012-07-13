#include "VipMemberDataWriterAdapter.h"

namespace xce
{
namespace adapter
{
namespace vipmemberdata
{

VipMemberDataWriterAdapter::VipMemberDataWriterAdapter() :  MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::VipMemberDataWriterPrx>
														  ("ControllerVipInfoWriter", 120,300,new MyUtil::OceChannel,true,"VM")
{
	start();
}

xce::vipinfo::VipMemberDataWriterPrx VipMemberDataWriterAdapter::getVipMemberDataWriter(int id)
{
	return getProxy(id);
}

void VipMemberDataWriterAdapter::reload(int id)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	if(_idSet.size() >= MAX_RELOAD_ID_SIZE)
	{
		return;
	}
	_idSet.insert(id);
	_mutex.notify();
}

void VipMemberDataWriterAdapter::reload(const MyUtil::LongSeq& ids)
{
	if(ids.empty())
	{
		MCE_WARN("VipMemberDataWriterAdapter::reload Exception id size = 0 ");
		return;
	}

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	if(_idSet.size() + ids.size() >= MAX_RELOAD_ID_SIZE)
	{
		return;
	}
	_idSet.insert(ids.begin(), ids.end());
	_mutex.notify();
}

void VipMemberDataWriterAdapter::run()
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
		
		xce::vipinfo::VipMemberDataWriterPrx prx = getVipMemberDataWriter(ids.at(0));
		if(prx)
		{
			try
			{
				prx->reload(ids);
			}catch(Ice::Exception& e)
			{
				MCE_WARN("VipMemberDataWriterAdapter::run reload Exception id size = " << ids.size() <<" " << e.what());
			}
		}
	}
}


}
}
}
