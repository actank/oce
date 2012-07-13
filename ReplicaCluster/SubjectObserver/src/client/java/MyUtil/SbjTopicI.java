package MyUtil;

import java.util.UUID;

import xce.util.channel.Channel;

import Ice.InitializationData;

import MyUtil.SbjSubscriber;
import MyUtil.SbjSubscriberPrxHelper;
import MyUtil.SbjTopic;
import MyUtil.SubjectObserverPrx;
import MyUtil.SubjectObserverPrxHelper;
import MyUtil.SbjSubscriberPrx;

public class SbjTopicI extends SbjTopic implements Runnable{
	
	protected Channel _channel;
	private SubjectObserverPrx _subjectObserverOneway;
	private SubjectObserverPrx _subjectObserverTwoway;
	private SbjSubscriberPrx _subscriber;
	static private String _topicName;
	private String _endpoints;
	private static int _interval=300;
	
	public SbjTopicI(String topicName,SbjSubscriber subscriber,String endpoints,long interval) {
		_topicName=topicName;
		_endpoints="@"+endpoints;
		_channel=Channel.newOceChannel(endpoints);
		
		_subjectObserverOneway = getSbjTopicOneWay();
		_subjectObserverTwoway = getSbjTopicTwoWay();
		
		_subscriber=SbjSubscriberPrxHelper.uncheckedCast(changeObjToObjPrx(subscriber));
	}
	
	private SubjectObserverPrx getSbjTopicOneWay() {
		return _channel.locate("SO",_endpoints, Channel.Invoke.Oneway,
				SubjectObserverPrxHelper.class,_interval);
	}
	
	private SubjectObserverPrx getSbjTopicTwoWay() {
		return _channel.locate("SO",_endpoints, Channel.Invoke.Twoway,
				SubjectObserverPrxHelper.class, _interval);
	}

	 public void subscribe(SbjSubscriberPrx subscriber){
		 try{
			 _subjectObserverOneway.subscribe(_topicName, subscriber);
		 }catch(Exception e){
			 e.printStackTrace();
		 }finally{
			 
		 }
	 }
	 
	 public void unsubscribe(SbjSubscriberPrx subscriber){
		 try{
			 _subjectObserverTwoway.unsubscribe(_topicName,subscriber);
		 }catch(Exception e){
			 e.printStackTrace();
		 }finally{
			 
		 }
		 
	 }

	public void run() {
		while(true){
			try{
				
				subscribe(_subscriber);				
			}catch(Exception e){
				System.out.println("[SjbTopic]: subscribe Thread Exception");
				e.printStackTrace();
			}	
			
			try {
				Thread.sleep(10*1000);
			} catch (Exception e) {
				e.printStackTrace();
			}
//			break;
		}
	}
	
	protected Ice.ObjectPrx changeObjToObjPrx(Ice.Object obj){
		
		UUID myUUID = UUID.randomUUID();
		
		String myName=myUUID.toString();
		
		Ice.Communicator _ic = createIceCommunicator(myName);
		
		Ice.ObjectAdapter objectAdapter = _ic.createObjectAdapter(myName);
		objectAdapter.activate();
		
		return objectAdapter.addWithUUID(obj);
		
	}
	
	private Ice.Communicator createIceCommunicator(String myName){
		Ice.Properties properties = Ice.Util.createProperties();
		properties.setProperty(myName + ".Endpoints", "tcp -h "
				+ xce.util.tools.IPAddress.getLocalAddress());
		
		Ice.InitializationData initData = new InitializationData();
		initData.properties = properties;
		Ice.Communicator ic = Ice.Util.initialize(initData);
		return ic;
	}
}