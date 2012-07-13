#ifndef __USERDESC_MASTERTOSLAVE_ADAPTER_H__
#define __USERDESC_MASTERTOSLAVE_ADAPTER_H__

#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

#include <QueryRunner.h>

#include "GeneralMaster/cxxmemadapter/GeneralMasterMemAdapter.h"

namespace xce {
namespace adapter {
namespace userdescmastertoslave {

using namespace com::xiaonei::xce;
using namespace ::xce::userdesc;
using namespace ::xce::generalmastermemadapter;
using namespace ::callguaranteemanager;


const std::string DB_USER_TIME_APT = "user_time";
const std::string DB_USER_CONTACT_APT  = "user_contact";

class UserContactMasterToSalveAdapter : public GeneralMasterMemAdapterI<UserContactManagerPrx>, public MyUtil::Singleton<UserContactMasterToSalveAdapter>
{
protected:

	class UserContactResultHandlerI;
	
	class CallCreateHandle;
	typedef IceUtil::Handle<CallCreateHandle> CallCreateHandlePtr;
	
	class CallSetHandle;
	typedef IceUtil::Handle<CallSetHandle> CallSetHandlePtr;
	
	class CallReloadHandle;
	typedef IceUtil::Handle<CallReloadHandle> CallReloadHandlePtr;

public:

	UserContactMasterToSalveAdapter() : GeneralMasterMemAdapterI<UserContactManagerPrx>("ControllerUserDesc", 120, 300, true, "C")
	{
	}

	void create(const int userId);

	void set(const int userId, const MyUtil::Str2StrMap& props);

	void reload(const int userId);

protected:

	class CallCreateHandle : public GuaranteeHandle 
	{
		public:

			CallCreateHandle(const int userId, const std::vector<UserContactManagerPrx> prxs):
							_userId(userId), _prxs(prxs)
			{
				Statement sql;
				sql << "SELECT id, msn, homepage, mobilephone, fixphone, qq FROM user_contact WHERE id = '" << _userId << "'";
			
				MyUtil::Str2StrMap res;

				UserContactResultHandlerI handler(res);
			
				QueryRunner(DB_USER_CONTACT_APT , CDbWServer).query(sql, handler);

				if( res.size() > 0)
				{
					_props = res;
				}
			}
	
		public:
			
			virtual bool doHandle()
			{
				if( _props.size() < 0 )
				{
					return true;
				}
				
				try
				{
					for(std::vector<UserContactManagerPrx>::const_iterator it = _prxs.begin();
						it != _prxs.end(); ++it)
					{
						(*it)->createUserContact(_props);
					}
				}catch(...)
				{
					return false;
				}
				return true;
			}
		
			virtual bool reDoHandle()
			{
				return doHandle();
			}

			virtual void merge(const GuaranteeHandlePtr& newData)
			{
			}

		private:

			int _userId;
			MyUtil::Str2StrMap _props;
			std::vector<UserContactManagerPrx> _prxs;
	};

	class CallSetHandle: public GuaranteeHandle
	{
		public:

			CallSetHandle(const int userId, const MyUtil::Str2StrMap props, const std::vector<UserContactManagerPrx> prxs):
						  _userId(userId),_props(props), _prxs(prxs)
			{
			}
	
		public:
			
			virtual bool doHandle()
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				try
				{
					for(std::vector<UserContactManagerPrx>::const_iterator it = _prxs.begin();
						it != _prxs.end(); ++it)
					{
						(*it)->setUserContact(_userId, _props);
					}
				}catch(...)
				{
					return false;
				}
				return true;
			}
	
			virtual bool reDoHandle()
			{
				return doHandle();
			} 
			
			virtual void merge(const GuaranteeHandlePtr& newData)
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				
				CallSetHandlePtr sourceData = CallSetHandlePtr::dynamicCast(newData);
				for(MyUtil::Str2StrMap::const_iterator it = sourceData->_props.begin();
					it != sourceData->_props.end(); ++ it)
				{
					_props[it->first] = it->second;
				}
			}
		
		private:
			
			int _userId;
			MyUtil::Str2StrMap _props;
			std::vector<UserContactManagerPrx> _prxs;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
	};
	
	class CallReloadHandle: public GuaranteeHandle
	{
		public:

			CallReloadHandle(const int userId, const std::vector<UserContactManagerPrx> prxs):
						  _userId(userId),_prxs(prxs)
			{
			}
	
		public:
			
			virtual bool doHandle()
			{
				try
				{
					for(std::vector<UserContactManagerPrx>::const_iterator it = _prxs.begin();
						it != _prxs.end(); ++it)
					{
						(*it)->reload(_userId);
					}
				}catch(...)
				{
					return false;
				}
				return true;
			}
	
			virtual bool reDoHandle()
			{
				return doHandle();
			} 
			
			virtual void merge(const GuaranteeHandlePtr& newData)
			{
			}
		
		private:
			
			int _userId;
			std::vector<UserContactManagerPrx> _prxs;
	};
	
	class UserContactResultHandlerI: public com::xiaonei::xce::ResultHandler
	{
		public:
			
			UserContactResultHandlerI(MyUtil::Str2StrMap& props) : _props(props)
			{
			}
			
			virtual bool handle(mysqlpp::Row& row) const
			{
				MyUtil::Str2StrMap result;

				result["ID"] = (string) row["id"];
				result["MSN"] = (string) row["msn"];
				result["HOMEPAGE"] = (string) row["homepage"];
				result["MOBILEPHONE"] = (string) row["mobilephone"];
				result["FIXPHONE"] = (string) row["fixphone"];
				result["QQ"] = (string) row["qq"];

				_props = result;

				return true;
			}
		
		private:
			MyUtil::Str2StrMap& _props;
	};
};

class UserTimeMasterToSalveAdapter : public GeneralMasterMemAdapterI<UserTimeManagerPrx>, public MyUtil::Singleton<UserTimeMasterToSalveAdapter>
{
protected:

	class UserTimeResultHandlerI;
	
	class CallCreateHandle;
	typedef IceUtil::Handle<CallCreateHandle> CallCreateHandlePtr;
	
	class CallSetHandle;
	typedef IceUtil::Handle<CallSetHandle> CallSetHandlePtr;
	
	class CallReloadHandle;
	typedef IceUtil::Handle<CallReloadHandle> CallReloadHandlePtr;

public:

	UserTimeMasterToSalveAdapter() : GeneralMasterMemAdapterI<UserTimeManagerPrx>("ControllerUserDesc", 120, 300, true, "C")
	{
	}

	void create(const int userId);

	void set(const int userId, const MyUtil::Str2StrMap& props);

	void reload(const int userId);

protected:

	class CallCreateHandle : public GuaranteeHandle 
	{
		public:

			CallCreateHandle(const int userId, const std::vector<UserTimeManagerPrx> prxs):
							_userId(userId), _prxs(prxs)
			{
				Statement sql;
				
				sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM user_time WHERE id = '" << userId << "'";
			
				MyUtil::Str2StrMap res;

				UserTimeResultHandlerI handler(res);
				
				QueryRunner(DB_USER_TIME_APT , CDbRServer).query(sql, handler);
				
				if( res.size() > 0)
				{
					_props = res;
				}
			}
	
		public:
			
			virtual bool doHandle()
			{
				if( _props.size() < 0 )
				{
					return true;
				}
				
				try
				{
					for(std::vector<UserTimeManagerPrx>::const_iterator it = _prxs.begin();
						it != _prxs.end(); ++it)
					{
						(*it)->createUserTime(_props);
					}
				}catch(...)
				{
					return false;
				}
				return true;
			}
		
			virtual bool reDoHandle()
			{
				return doHandle();
			}

			virtual void merge(const GuaranteeHandlePtr& newData)
			{
			}

		private:

			int _userId;
			MyUtil::Str2StrMap _props;
			std::vector<UserTimeManagerPrx> _prxs;
	};

	class CallSetHandle: public GuaranteeHandle
	{
		public:

			CallSetHandle(const int userId, const MyUtil::Str2StrMap props, const std::vector<UserTimeManagerPrx> prxs):
						  _userId(userId),_props(props), _prxs(prxs)
			{
			}
	
		public:
			
			virtual bool doHandle()
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				try
				{
					for(std::vector<UserTimeManagerPrx>::const_iterator it = _prxs.begin();
						it != _prxs.end(); ++it)
					{
						(*it)->setUserTime(_userId, _props);
					}
				}catch(...)
				{
					return false;
				}
				return true;
			}
	
			virtual bool reDoHandle()
			{
				return doHandle();
			} 
			
			virtual void merge(const GuaranteeHandlePtr& newData)
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				
				CallSetHandlePtr sourceData = CallSetHandlePtr::dynamicCast(newData);
				for(MyUtil::Str2StrMap::const_iterator it = sourceData->_props.begin();
					it != sourceData->_props.end(); ++ it)
				{
					_props[it->first] = it->second;
				}
			}
		
		private:
			
			int _userId;
			MyUtil::Str2StrMap _props;
			std::vector<UserTimeManagerPrx> _prxs;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
	};
	
	class CallReloadHandle: public GuaranteeHandle
	{
		public:

			CallReloadHandle(const int userId, const std::vector<UserTimeManagerPrx> prxs):
						  _userId(userId),_prxs(prxs)
			{
			}
	
		public:
			
			virtual bool doHandle()
			{
				try
				{
					for(std::vector<UserTimeManagerPrx>::const_iterator it = _prxs.begin();
						it != _prxs.end(); ++it)
					{
						(*it)->reload(_userId);
					}
				}catch(...)
				{
					return false;
				}
				return true;
			}
	
			virtual bool reDoHandle()
			{
				return doHandle();
			} 
			
			virtual void merge(const GuaranteeHandlePtr& newData)
			{
			}
		
		private:
			
			int _userId;
			std::vector<UserTimeManagerPrx> _prxs;
	};
	
	class UserTimeResultHandlerI: public com::xiaonei::xce::ResultHandler
	{
		public:
			
			UserTimeResultHandlerI(MyUtil::Str2StrMap& props) : _props(props)
			{
			}
			
			virtual bool handle(mysqlpp::Row& row) const
			{
				MyUtil::Str2StrMap result;

				result["ID"] = (string) row["id"];
				result["REGISTERTIME"] = (string) row["registertime"];
				result["ACTIVATETIME"] = (string) row["activatetime"];
				result["LASTLOGINTIME"] = (string) row["lastLogintime"];
				result["LASTLOGOUTTIME"] = (string) row["lastLogouttime"];
				
				_props = result;

				return true;
			}
		
		private:
			MyUtil::Str2StrMap& _props;
	};
};

}
}
}

#endif

