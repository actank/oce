package com.xiaonei.xce.log;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.TimeUnit;

import xce.log.LogEntry;

public class XceLogger {
  private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
      .getLog("xceloggerlog");

  private class CheckInvoke extends Thread {
    private List<LogEntry> toInvokeCache = new ArrayList<LogEntry>();
    private long lastInvokeTime = 0;

    private boolean shouldInvoke() {
      return (null != prx_)
          && (toInvokeCache.size() > 128 || System.currentTimeMillis()
              - lastInvokeTime > 10L * 1000L);
    }

    private void invokeAndClearCache() {
      try {
        if (toInvokeCache.isEmpty())
          return;
        prx_.append(toInvokeCache);
      } catch (Throwable e) {
        _logger.warn("InvokeAndClearCache => ", e);
      } finally {
        lastInvokeTime = System.currentTimeMillis();
        toInvokeCache.clear();
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
          }
        } catch (InterruptedException e) {
          break;
        } catch (Exception e) {
          _logger.warn("CheckInvoke thread => ", e);
        }
      }
    }
  }

  private final Ice.Communicator ic_;
  private final xce.log.LoggerPrx prx_;
  private LinkedBlockingDeque<LogEntry> cachedEntry = new LinkedBlockingDeque<LogEntry>(
      100000);
  private CheckInvoke checkThread = new CheckInvoke();

  protected XceLogger(Ice.Communicator ic, xce.log.LoggerPrx prx) {
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
