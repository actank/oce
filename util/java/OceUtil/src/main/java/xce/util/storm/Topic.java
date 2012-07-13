package xce.util.storm;

import java.util.Map;

import IceStorm.AlreadySubscribed;
import IceStorm.BadQoS;
import IceStorm.NoSuchTopic;
import IceStorm.TopicExists;
import IceStorm.TopicPrx;

public class Topic {
	public Topic(String topicName) {
		this(new TopicManager("IceStorm"), topicName);
	}

	public Topic(String topicManagerName, String topicName) {
		this(new TopicManager(topicManagerName), topicName);
	}

	public Topic(TopicManager topicManager, String topicName) {
		_topicManager = topicManager;
		_topicName = topicName;
	}

	private final TopicManager _topicManager;
	private final String _topicName;

	public TopicPrx getTopic() {
		try {
			return _topicManager.getTopicManager().retrieve(_topicName);
		} catch (NoSuchTopic e) {
			try {
				return _topicManager.getTopicManager().create(_topicName);
			} catch (TopicExists e1) {
				// insane exception
				return null;
			}
		}
	}

	private Ice.ObjectPrx _publisher = null;

	public Ice.ObjectPrx getPublisher() {
		if (_publisher == null) {
			synchronized (this) {
				if (_publisher == null) {
					_publisher = getTopic().getPublisher();
				}
			}
		}
		return _publisher;
		// return getTopic().getPublisher();
	}

	public Ice.ObjectPrx subscribe(Map<String, String> qos, Ice.Object obj) {
		Ice.ObjectPrx result = addObject(obj);
		try {
			Ice.ObjectPrx publisher = getTopic().subscribeAndGetPublisher(qos,
					result);
			if (_publisher == null) {
				synchronized (this) {
					if (_publisher == null) {
						_publisher = publisher;
					}
				}
			}
		} catch (AlreadySubscribed e) {
			e.printStackTrace();
		} catch (BadQoS e) {
			e.printStackTrace();
		}
		return result;
	}

	public void unsubscribe(Ice.ObjectPrx prx) {
		getTopic().unsubscribe(prx);
	}

	private Ice.ObjectPrx addObject(Ice.Object obj) {
		return _topicManager.addObjectWithUUID(obj);
	}
}
