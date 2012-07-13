package MyUtil;

import java.util.UUID;

import xce.util.channel.Channel;
import xce.util.tools.IPAddress;
import Ice.InitializationData;
import Ice.ObjectAdapter;

import com.xiaonei.xce.log.Oce;

public class SbjTopicI extends SbjTopic implements Runnable{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	protected Channel _channel;
	private SubjectObserverPrx _subjectObserverOneway;
	private SubjectObserverPrx _subjectObserverTwoway;
	private SbjSubscriberPrx _subscriber;
	static private String _topicName;
	private String _endpoints;
	private int _interval=120;	  
	
	public SbjTopicI(String topicName,
									 SbjSubscriber subscriber,
									 String endpoints,
									 int interval,
									 Channel channel) {
		_topicName=topicName;
		_endpoints="@"+endpoints;
		// _channel=Channel.newCoreChannel(endpoints);
		_channel=channel;
		
		if( interval > 0)
		{
			_interval = interval;
		}
		
		_subjectObserverOneway = getSbjTopicOneWay();
		_subjectObserverTwoway = getSbjTopicTwoWay();
		
		UUID myUUID = UUID.randomUUID();
		String myName="SubscriberRandom_"+myUUID.toString();
		
		_channel.getCommunicator().getProperties().setProperty(myName+".Endpoints", "tcp -h " + getLocalIP());
		ObjectAdapter _adapter = _channel.getCommunicator().createObjectAdapter(myName);
		_adapter.activate();
		_subscriber=SbjSubscriberPrxHelper.uncheckedCast(_adapter.addWithUUID(subscriber).ice_timeout(300));			
	}
	
	private SubjectObserverPrx getSbjTopicOneWay() {
		return _channel.locate("SO",_endpoints, Channel.Invoke.Oneway,
				SubjectObserverPrxHelper.class, 300);
	}
	
	private SubjectObserverPrx getSbjTopicTwoWay() {
		return _channel.locate("SO",_endpoints, Channel.Invoke.Twoway,
				SubjectObserverPrxHelper.class, 300);
	}

	 public void subscribe(SbjSubscriberPrx subscriber){
		 try{
			 _subjectObserverOneway.subscribe(_topicName, subscriber);
			 // System.out.println("Subscribe to controller");
		 }catch(Exception e){
			 Oce.getLogger().error(this.getClass().getName() + " subscribe " + e +" interval: "+_interval+" endpoint: "+_endpoints);
			 //e.printStackTrace();
		 }finally{
			 
		 }
	 }
	 
	 public void unsubscribe(SbjSubscriberPrx subscriber){
		 try{
			 _subjectObserverTwoway.unsubscribe(_topicName,subscriber);
		 }catch(Exception e){
			 Oce.getLogger().error(this.getClass().getName() + " unsubscribe " + e);
			 //e.printStackTrace();
		 }finally{
			 
		 }
		 
	 }

	public void run() {
		while(true){			
			try{				
				subscribe(_subscriber);				
			}catch(Exception e){
				Oce.getLogger().error(this.getClass().getName() + " run " + e);
				//System.out.println("[SjbTopic]: subscribe Thread Exception");
				//e.printStackTrace();
			}			
			try {				
				Thread.sleep(_interval*1000);
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + " run " + e);
				//e.printStackTrace();
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
	
	public  String  getLocalIP()  {  
        try  {  
        	
        	String ip = IPAddress.getLocalAddress();
        	// System.out.println(ip);         	
        	return ip;        	
        }  
        catch  (Exception  e)  {
        	Oce.getLogger().error(this.getClass().getName() + " getLocalIP " + e);
        	//e.printStackTrace();
        	return "127.0.0.1";
        } 
          
	}
}  
