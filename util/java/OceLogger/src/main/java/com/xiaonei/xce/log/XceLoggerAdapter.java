package com.xiaonei.xce.log;

import java.text.DateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import xce.log.LoggerPrxHelper;
import xce.util.channel.Channel;

public class XceLoggerAdapter {

  private static HashMap<String, XceLogger> _cachedLogger = new HashMap<String, XceLogger>();

  private static String hash(List<String> args, String loggerEndpoints) {
    StringBuffer buff = new StringBuffer();
    if (args == null || args.isEmpty()) {
      buff.append("DEFAULT_");
    } else {
      for (String arg : args) {
        buff.append(arg).append("_");
      }
    }
    buff.append(loggerEndpoints);
    return buff.toString();
  }

  public static XceLogger getLogger() {
    return getLogger(null, null);
  }

  public static XceLogger getLogger(String loggerEndpoints) {
    return getLogger(null, loggerEndpoints);
  }

  public synchronized static XceLogger getLogger(List<String> args,
      String loggerEndpoints) {
    loggerEndpoints = (null == loggerEndpoints) ? "M@XceLogger"
        : loggerEndpoints;
    String hash = hash(args, loggerEndpoints);
    XceLogger cached = _cachedLogger.get(hash);
    if (cached != null) {
      return cached;
    }

    Ice.Communicator ic;
    xce.log.LoggerPrx prx;
    if (args == null || args.isEmpty()) {
      System.out.println("XceLoggerAdapter uses default settings.");
      ic = Channel.newLogChannel("XceLogger").getCommunicator();
    } else {
      System.out.println("XceLoggerAdapter uses customized settings.");
      ic = Ice.Util.initialize(args.toArray(new String[args.size()]));
    }
    prx = LoggerPrxHelper.uncheckedCast(ic.stringToProxy(loggerEndpoints)
        .ice_locatorCacheTimeout(60).ice_compress(true).ice_datagram());
    cached = new XceLogger(ic, prx);
    _cachedLogger.put(hash, cached);
    return cached;
  }

  public static void main(String[] args) {
    
    DateFormat d = DateFormat.getDateTimeInstance();
    for ( int i = 0; i < 1000000; ++i ) {
      XceLoggerAdapter.getLogger("M@XceLogger").addLog(
          new String[] { "ll", "no-HA" }, "[" + String.valueOf(i) + "]\t" + d.format(new Date()));
      System.out.print(i);
      if ( i % 100 == 0) {
        System.out.print("\n[" + i + "] ");
      }
      
      try {        
        Thread.sleep(200);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
  }  
}
