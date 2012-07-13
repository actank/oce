#ifndef __ACTIVE_TRACK_H__
#define __ACTIVE_TRACK_H__

#include "Channel.h"
#include "Singleton.h"
#include "XceLoggerReplicaAdapter.h"
#include "XceLogger.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace log {
	
const int CLUSTER = 10;

enum LOG_TYPE{ XIAONEI, KAIXIN,OTHER };

class ActiveTrack : public MyUtil::Singleton<ActiveTrack> {
public:
	/* @brief 初始化(自动判断是否校内/开心节点)
	 * @param	loggerEndpoints	endpoints(默认为"")
	 */
	void initialize(string ctrlEndpoints="ControllerXceLogger");

	/* @brief 初始化
	 * @param	log_type	log类型(XIAONEI/KAIXIN/OTHER)
	 * @param	loggerEndpoints	endpoints(默认为"")
	 */
	void initialize(LOG_TYPE type, string ctrlEndpoints="ControllerXceLogger");

	/* @brief	记录一条AccessLog,和web的AccessLog结构、位置完全相同
	 * @param	userId		用户id
	 * @param	ip		用户ip
	 * @param	url		访问的url
	 * @param	decision	若该url有需要选择的项目，记录该项目	 *
	 */
	void access(int userId, const string& ip, const string& url, const string& decision);

	/* @biref	记录一个行为。 例如： ActiveTrack.action(202911262, vector<string>{"FEEDNEWS",
	 *  "PROFILE"}, "ADDFRIEND", "200353733"); 上述调用记录了一次加好友操作
	 * @param 	userId		用户id
	 * @param 	paths
	 * @param 	action		行为
	 * @param 	object  	行为对象
	 */
	void action(int userId, const vector<string>& paths, const string& action,
			const string& object);
	
	/* @biref	记录一个行为。 例如： ActiveTrack.action(202911262, vector<string>{"FEEDNEWS",
	 *  "PROFILE"}, "ADDFRIEND", "200353733"); 上述调用记录了一次加好友操作
	 * @param 	userId		用户id
	 * @param 	path		path
	 * @param 	action		行为
	 * @param 	object  	行为对象
	 */
	void action(int userId, const string& path, const string& action,
			const string& object);
	
private:
	friend class MyUtil::Singleton<ActiveTrack>;
	ActiveTrack():
       		_isXiaonei(true),
		_isKaixin(false)	{
	}

	void doAction(int userId, const string& path, const string& action,
			const string& object);

	com::xiaonei::service::XceLoggerReplicaPtr _logger;
	vector< vector<string> > _accessCategories;
	vector< vector<string> > _actionCategories;
	bool _isXiaonei;
	bool _isKaixin;
};

}
}

#endif

