package xce.util.storm;

import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

import xce.util.channel.Channel;
import xce.util.tools.IPAddress;

import IceStorm.TopicManagerPrx;
import IceStorm.TopicManagerPrxHelper;


public class TopicManager {

	private static ConcurrentHashMap<String, TopicManager> _instances = new ConcurrentHashMap<String, TopicManager>();

	public static TopicManager getInstance(String instanceName) {
		if (!_instances.containsKey(instanceName)) {
			/*** TODO: This may be executed more than once. ***/
			_instances
					.putIfAbsent(instanceName, new TopicManager(instanceName));
		}
		return _instances.get(instanceName);
	}

	protected TopicManager(String instanceName) {
		_instanceName = instanceName;

		_channel = Channel.newOceChannel("Storm_" + _instanceName);

		StringBuffer idBuff = new StringBuffer();
		idBuff.append(_instanceName).append("/TopicManager@").append(
				_instanceName).append(".TopicManager");
		_topicManager = TopicManagerPrxHelper.uncheckedCast(_channel
				.getCommunicator().stringToProxy(idBuff.toString()));
	}

	private final String _instanceName;
	private final Channel _channel;
	private Ice.ObjectAdapter _adapter = null;
	private final TopicManagerPrx _topicManager;

	protected TopicManagerPrx getTopicManager() {
		return _topicManager;
	}

	protected synchronized Ice.ObjectPrx addObjectWithUUID(Ice.Object obj) {
		if (_adapter == null) {
			String uuid = UUID.randomUUID().toString();
			_adapter = _channel.getCommunicator()
					.createObjectAdapterWithEndpoints(uuid,
							"tcp -h " + IPAddress.getLocalAddress());
			_adapter.activate();
		}
		return _adapter.addWithUUID(obj);
	}
}
