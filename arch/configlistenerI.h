#ifndef XCE_ARCH_CONFIGLISTENERI_H__
#define XCE_ARCH_CONFIGLISTENERI_H__

#include <boost/thread/mutex.hpp>
//#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "arch/configserver_adapter.h"

// 服务器响应配置变更的接口封装
// TODO: 重构 storm svcdescriptor

namespace xce {

template<typename ObjectT>
class ConfigListenerI : public xce::ConfigListener {
public:
  ConfigListenerI(ObjectT& t) 
		: t_(t)
	{
	}

  virtual bool Update(const std::string& key, const std::string& value, const Ice::Current& = ::Ice::Current())
	{
		LOG(LS_VERBOSE) << "config " << key << " set to " << value;
		// if changed
		// Ice::PropertiesPtr p = detail::MakeProperties(context);
		// t_.ApplyConfig(p); // TODO: add key

    t_.ApplyConfig(key, value);
    return true;
  }

private:
  ObjectT& t_;
	//std::vector<>
};

}
#endif // XCE_ARCH_CONFIGLISTENERI_H__
