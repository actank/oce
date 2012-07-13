#ifndef _CHECK_HANDLER_FACTORY_H__
#define _CHECK_HANDLER_FACTORY_H__

#include "ServiceI.h"
#include "CheckHandler.h"
#include "UserCacheCheckHandler.h"
#include "UserAdsCacheCheckHandler.h"
#include "UserApiCacheCheckHandler.h"
#include "UserDescCheckHandler.h"
#include "UserBaseCheckHandler.h"
#include "BuddyByNameCacheCheckHandler.h"
#include "BuddyCacheCheckHandler.h"
#include "UserNetworkCheckHandler.h"
#include "UserConfigCheckHandler.h"

namespace xce 
{
namespace generalcheck
{
	class CheckHandlerFactory : public Ice::Object
	{
		public:
			
			static CheckHandlerPtr getHandler( const std::string& identifier )
			{
				if( identifier == "UserCache" )
				{
					return new UserCacheCheckHandler( 100, 1 );
				} else if(identifier == "UserAdsCache") {
					return new UserAdsCacheCheckHandler( 100, 4 );
				} else if(identifier == "UserApiCache") {
					return new UserApiCacheCheckHandler( 100, 4 );
				} else if(identifier == "BuddyCache") {
					return new BuddyCacheCheckHandler( 10,10,6,300);
				} else if(identifier == "BuddyByNameCache") {
					return new BuddyByNameCacheCheckHandler( 10,10,4,300);
				}else if(identifier == "UserDesc") {
					return new UserDescCheckHandler( 100,10);
				}else if(identifier == "UserBase") {
					return new UserBaseCheckHandler( 100,100);
				}else if(identifier == "UserNetwork") {
					return new UserNetworkCheckHandler( 10, 10);
				}else if(identifier == "UserConfig") {
					return new UserConfigCheckHandler( 100, 100);
				}

				return NULL;
			}
	};
}
}

#endif
