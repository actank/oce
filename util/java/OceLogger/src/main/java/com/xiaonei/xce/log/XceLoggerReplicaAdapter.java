package com.xiaonei.xce.log;

import java.text.DateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.log.LoggerPrxHelper;
import xce.util.channel.Channel;

public class XceLoggerReplicaAdapter extends ReplicatedClusterAdapter {

  private static HashMap<String, XceLoggerReplica> _cachedLogger = new HashMap<String, XceLoggerReplica>();  

  private static String hash(List<String> args, String ctrlEndpoints) {
    StringBuffer buff = new StringBuffer();
    if (args == null || args.isEmpty()) {
      buff.append("DEFAULT_");
    } else {
      for (String arg : args) {
        buff.append(arg).append("_");
      }
    }
    buff.append(ctrlEndpoints);
    return buff.toString();
  }

  public static XceLoggerReplica getLogger() {
    return getLogger(null);
  }

  public static XceLoggerReplica getLogger(String ctrlEndpoints) {
    
    ctrlEndpoints = (null == ctrlEndpoints) ? "ControllerXceLogger"
        : ctrlEndpoints;
    String hash = hash(null, ctrlEndpoints);
    XceLoggerReplica cached = _cachedLogger.get(hash);
    if (cached != null) {
      return cached;
    }

    cached = new XceLoggerReplica(ctrlEndpoints);
    _cachedLogger.put(hash, cached);
    return cached;
  }
  
  

  public synchronized static XceLoggerReplica getLoggerDirectly(List<String> args,
      String loggerEndpoints) {
    
    if ( null == args ) {
      return null;
    }    
    if ( (loggerEndpoints == null) || (loggerEndpoints.equals("")) ) {
      loggerEndpoints = "M@XceLogger";
    }
    String hash = hash(args, loggerEndpoints);
    XceLoggerReplica cached = _cachedLogger.get(hash);
    if (cached != null) {
      return cached;
    }

    System.out.println("XceLoggerAdapter uses customized settings.");
    Ice.Communicator ic = Ice.Util.initialize(args.toArray(new String[args.size()]));
    xce.log.LoggerPrx prx = LoggerPrxHelper.uncheckedCast(ic.stringToProxy(loggerEndpoints)
        .ice_locatorCacheTimeout(60).ice_compress(true).ice_datagram());
    cached = new XceLoggerReplica(ic, prx);
    _cachedLogger.put(hash, cached);
    return cached;
  }

  public static void main(String[] args) {
    
    DateFormat d = DateFormat.getDateTimeInstance();
    for ( int i = 0; i < 100000; ++i ) {
      XceLoggerReplicaAdapter.getLogger().addLog(
          new String[] { "ll", "HA" }, "[" + String.valueOf(i) + "]\t" + d.format(new Date()));
      System.out.print(i);
      
      try {        
        Thread.sleep(500);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    
    System.exit(0);

  }
}
