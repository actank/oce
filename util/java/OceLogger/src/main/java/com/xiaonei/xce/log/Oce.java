package com.xiaonei.xce.log;

import java.util.Date;

import xce.util.tools.DateFormatter;
import xce.util.tools.IPAddress;

public class Oce {
  public static class FakeLogger {
    private String[] categories;
    private String localAddress;

    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
        .getLog("oce");

    public static org.apache.commons.logging.Log getLogger() {
      return _logger;
    }

    public FakeLogger(String[] categories) {
      try {
        this.categories = categories;
      } catch (Throwable e) {
        this.categories = new String[] { "xce", "unknown" };
        e.printStackTrace();
      }
      try {
        this.localAddress = IPAddress.getLocalAddress();
      } catch (Throwable e) {
        this.localAddress = "";
        e.printStackTrace();
      }
    }

    public void info(Object obj) {
      String message = obj.toString();
      XceLoggerAdapter.getLogger().addLog(categories, format(message));
      _logger.info(obj);
    }

    public void error(Object obj) {
      String message = obj.toString();
      XceLoggerAdapter.getLogger().addLog(categories, format(message));
      _logger.error(obj);
    }

    public void error(Object obj, Throwable e) {
        String message = obj.toString() + e;
        XceLoggerAdapter.getLogger().addLog(categories, format(message));
        _logger.error(obj,e);
    }
    
    private String format(String message) {
      return DateFormatter.format(new Date()) + " " + localAddress + " "
          + message;
    }
  }

  private static FakeLogger dummy = new FakeLogger(
      new String[] { "xce", "oce" });

  public static FakeLogger getLogger() {
    return dummy;
  }

}
