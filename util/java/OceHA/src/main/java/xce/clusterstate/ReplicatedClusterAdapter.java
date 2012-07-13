package xce.clusterstate;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import xce.serverstate.ServerStateInfo;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

public class ReplicatedClusterAdapter {
  protected ClientInterface _csClient;

  public ReplicatedClusterAdapter(String endpoint, int interval) {
    _csClient = new ClientInterface(endpoint, interval,Channel.newOceChannel(endpoint));
  }
  
  public ReplicatedClusterAdapter(String endpoint, int interval,Channel c) {
      _csClient = new ClientInterface(endpoint, interval,c);
    }

  protected ReplicatedClusterAdapter() {
  }

  public ObjectPrx getValidProxy(int userid) {
    return _csClient.getValidProxy(userid);
  }

  public ObjectPrx getValidProxy(int userid, int timeout) {
    ObjectPrx prx = _csClient.getValidProxy(userid);
    return (prx == null) ? null : prx.ice_timeout(timeout);
  }

  public ObjectPrx getValidProxy(int userid, String serviceName) {
    return _csClient.getValidProxy(userid, serviceName);
  }

  public ObjectPrx getValidProxy(int userid, String serviceName, int timeout) {
    ObjectPrx prx = _csClient.getValidProxy(userid, serviceName);
    return (prx == null) ? null : prx.ice_timeout(timeout);
  }

  public List getAllProxy(int userId, String serviceName) {
    return _csClient.getAllProxy(userId, serviceName);
  }

  public ObjectPrx getProxy(int userid) {
    return _csClient.getProxy(userid);
  }

  public ObjectPrx getProxy(int userid, int timeout) {
    ObjectPrx prx = _csClient.getProxy(userid);
    return (prx == null) ? null : prx.ice_timeout(timeout);
  }

  public int getCluster() {
    return _csClient.getCluster();
  }

  public ArrayList getAllProxy(int userid) {
    int mod = userid % getCluster();
    ArrayList<ObjectPrx> prxs = new ArrayList<ObjectPrx>();
    Map<Integer, ServerStateInfo[]> serverStateInfoMap = _csClient.getAvailableObjProxies();
    ServerStateInfo[] serverInfo = serverStateInfoMap.get(mod);
    if (serverInfo == null) {
      return prxs;
    }
    for (int i = 0; i < serverInfo.length; ++i) {
      prxs.add(serverInfo[i].prx);
    }
    return prxs;
  }

  public static void main(String args[]) {
    ReplicatedClusterAdapter repli = new ReplicatedClusterAdapter("ControllerBuddyCore", 120);
    ArrayList prxs = repli.getAllProxy(0);
    for (int i = 0; i < prxs.size(); ++i) {
      System.out.println(prxs.get(i).toString());
    }
    // Map<Integer, ServerStateInfo[]> serverStateInfoMap =
    // repli._csClient.getAvailableObjProxies();
    // Set<Integer> keys = serverStateInfoMap.keySet();
    // Iterator<Integer> it = keys.iterator();
    // while(it.hasNext()) {
    // Integer integer = it.next();
    // System.out.println(integer);
    // ServerStateInfo[] serverInfo = serverStateInfoMap.get(integer);
    // for(int i = 0; i < serverInfo.length; ++i) {
    // System.out.println("name = " + serverInfo[i].servername + " identity = "
    // + serverInfo[i].identity + " cluster = "
    // + serverInfo[i].cluster + " mod = " + serverInfo[i].mod);
    // }
    // System.out.println("\n\n");
    // }
    // Ice.ObjectPrx prx =
    // Channel.newBuddyChannel("").getCommunicator().stringToProxy("TMA@BuddyByIdLoader0"
    // );
    // xce.svcapi.TaskManagerAPIPrx tma =
    // xce.svcapi.TaskManagerAPIPrxHelper.checkedCast(prx);
    // System.out.println("Status update queue size is " + tma.size(4));
  }
}
