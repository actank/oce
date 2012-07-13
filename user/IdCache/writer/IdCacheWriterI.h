#ifndef ID_CACHE_WRITER_I_H
#define ID_CACHE_WRITER_I_H

#include <ServiceI.h>
#include <IdCache.h>

namespace idcache
{
	class IdCacheWriterI : public IdCacheWriter, public MyUtil::Singleton<IdCacheWriterI>
	{
		public:
			void createIdRelation(::Ice::Int id, ::Ice::Int transId, ::Ice::Int virtualId, const ::Ice::Current& current);
			void deleteIdRelation(::Ice::Int id, ::Ice::Int virtualId, const ::Ice::Current& current);
			void reload(::Ice::Int id, const ::Ice::Current& current);
			void setValid(bool flag, const ::Ice::Current& current);
	};
}

#endif
