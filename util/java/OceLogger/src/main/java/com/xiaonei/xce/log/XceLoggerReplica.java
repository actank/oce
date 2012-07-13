package com.xiaonei.xce.log;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.TimeUnit;

import xce.blt.BulletinAdminPrx;
import xce.blt.BulletinAdminPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.log.LogEntry;
import xce.log.LoggerPrx;
import xce.log.LoggerPrxHelper;
import xce.util.channel.OceChannel;

public class XceLoggerReplica extends ReplicatedClusterAdapter {
  private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory.getLog("xceloggerlog");

  private class CheckInvoke extends Thread {
    private List<LogEntry> toInvokeCache = new ArrayList<LogEntry>();   
    private int messageLen = 0;
    private long lastInvokeTime = 0;

    private boolean shouldInvoke() {
      return ( messageLen >= 32767 || toInvokeCache.size() > 128 || System.currentTimeMillis() - lastInvokeTime > 10L * 1000L);
    }

    private void invokeAndClearCache() {
      try {
        if (toInvokeCache.isEmpty()) {
          return;
        }
        LoggerPrx prx = getProxy();
        
        if (prx != null) {
          prx.append(toInvokeCache);
        } else {
          _logger.warn("InvokeAndClearCache => XceLogger Proxy is null, can't append");
        }
      } catch (Throwable e) {
        e.printStackTrace();
        _logger.warn("InvokeAndClearCache => ", e);
      } finally {
        lastInvokeTime = System.currentTimeMillis();
        toInvokeCache.clear();
        messageLen = 0;
      }
    }

    private void invoke() {
      if (shouldInvoke())
        invokeAndClearCache();
    }

    @Override
    public void run() {
      while (true) {
        try {
          invoke();
          LogEntry entry = cachedEntry.pollLast(3L, TimeUnit.SECONDS);
          if (entry != null) {
            toInvokeCache.add(entry);
            messageLen += entry.message.length();
          }
        } catch (InterruptedException e) {
          break;
        } catch (Exception e) {
          _logger.warn("CheckInvoke thread => ", e);
        }
      }
    }
  }

  private Ice.Communicator ic_ = null;
  private LoggerPrx prx_ = null;
  private String ctrlEndpoints_ = null;
  private static int interval = 120;

  private LinkedBlockingDeque<LogEntry> cachedEntry = new LinkedBlockingDeque<LogEntry>(100000);
  private CheckInvoke checkThread = new CheckInvoke();

  protected XceLoggerReplica(String ctrlEndpoints) {  
    super(ctrlEndpoints, interval);
    ctrlEndpoints_ = ctrlEndpoints;
    checkThread.setDaemon(true);
    checkThread.start();
  }
  
  protected XceLoggerReplica(Ice.Communicator ic, xce.log.LoggerPrx prx) {
    ic_ = ic;
    prx_ = prx;
    checkThread.setDaemon(true);
    checkThread.start();
  }

  public void addLog(String[] categories, String message) {
    append(new LogEntry(categories, message));
  }

  private void append(LogEntry entry) {
    cachedEntry.offerFirst(entry);
  }

  private LoggerPrx getProxy() {
    Ice.ObjectPrx prx0 = null;
    
    // 提供两种寻找方式:HA方式和直接拼凑endpoints的方式
    if ( ctrlEndpoints_ != null ) {
       prx0 = getValidProxy(0);
       if (prx0 != null) {
         LoggerPrx prx = LoggerPrxHelper.uncheckedCast(prx0.ice_locatorCacheTimeout(60).ice_compress(true).ice_datagram());    
         return prx;
       } else {
         return null;
       }       
    } else if ( prx_ != null ) {
       return prx_;
    }
    return null;
  }

  public void close() {
    /*
     * checkThread.interrupt(); if (null != ic_) { ic_.destroy(); }
     */
  }

  @Override
  public void finalize() {
    /*
     * checkThread.interrupt(); if (null != ic_) { ic_.destroy(); }
     */
  }
}
