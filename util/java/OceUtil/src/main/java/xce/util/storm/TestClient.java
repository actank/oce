package xce.util.storm;

import java.util.HashMap;

import Ice.Current;
import MyUtil.SvcDescriptor;
import MyUtil.SvcInstance;
import MyUtil.SvcObserverPrx;

public class TestClient {
	public static void main(String[] args) throws Throwable {
		SvcDescriptorI obj = new SvcDescriptorI();

		Thread.sleep(1000);
	
		Topic client = new Topic(new TopicManager("TestStorm"), "Test");
		//SvcDescriptorPrxHelper.uncheckedCast(client.getPublisher()).reload();

		Thread.sleep(1000);
		
		System.exit(0);
	}
}

class SvcDescriptorI extends SvcDescriptor {
	public SvcDescriptorI() {
		_client.subscribe(new HashMap<String, String>(), this);
	}

	private Topic _client = new Topic(new TopicManager("TestStorm"), "Test");

	public void _notify(String name, Current __current) {
		// TODO Auto-generated method stub

	}

	public SvcInstance getSvcInstance(String name, Current __current) {
		// TODO Auto-generated method stub
		return null;
	}

	public SvcInstance query(String name, Current __current) {
		// TODO Auto-generated method stub
		return null;
	}

	public void reload(Current __current) {
		
	}

	public void subscribe(String name, SvcObserverPrx observer,
			Current __current) {
		// TODO Auto-generated method stub

	}

	public void unsubscribe(String name, SvcObserverPrx observer,
			Current __current) {
		// TODO Auto-generated method stub

	}
}
