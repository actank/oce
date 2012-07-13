package xce.replicaclusterdemo;

import Ice.Communicator;
import Ice.ObjectPrx;
import xce.clusterstate.ClientInterface;
import xce.util.channel.Channel;

public class RepCluDemoAdapter implements Runnable {
	public RepCluDemoAdapter(int userId) {
		
		cliInter=new ClientInterface("ControllerFootprint",300);
		_userId=userId;
	}

	protected Channel _channel;
	private ClientInterface cliInter;
	private String[] availableServersArray;

	private int _userId;
	
	public String getObjPrx() {
		return objPrx;
	}

	public void setObjPrx(String objPrx) {
		this.objPrx = objPrx;
	}

	private String objPrx;

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		RepCluDemoAdapter rcda=new RepCluDemoAdapter(222677621);

		Thread thr=new Thread(rcda);

		thr.start();
		
		try {
			Thread.sleep(1000*5);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
//		rcda.printprx();
	}
	
	public String[] getAvailableServersArray() {
		return availableServersArray;
	}

	public void printprx(){
		int index=_userId%10;
		objPrx=availableServersArray[index];
		
		Channel channel=Channel.newOceChannel(objPrx);
		Communicator ic=channel.getCommunicator();
		ObjectPrx ob=ic.stringToProxy("Demo"+"@"+objPrx);
		
		repcludemoManagerPrx repPrx=repcludemoManagerPrxHelper.uncheckedCast(ob);

		repPrx.printprx(objPrx);
	}
	
	public void run() {
		
		while(true){
			try{
				availableServersArray=cliInter.getAvailableObjProxies();
				
				if(availableServersArray!=null){
					
					System.out.println("availableServers:....."+availableServersArray[0]+"    "+availableServersArray[1]+"    "+availableServersArray[2]);
					
					printprx();
					
				}else{
					System.out.println("availableServers:.....null");
				}
				
				
				
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

}