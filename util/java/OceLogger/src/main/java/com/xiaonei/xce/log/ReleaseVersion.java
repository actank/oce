package com.xiaonei.xce.log;

import java.io.IOException;
import java.util.Date;

import xce.util.tools.DateFormatter;
import xce.util.tools.IPAddress;

public class ReleaseVersion {
  private static final String[] CLAZZS = new String[] {
      "com.xiaonei.xce.log.XceLogger", "xce.util.tools.Version",
      "com.xiaonei.xce.init.Init", "com.xiaonei.xce.pool.DataSourceInstanceIF",
      "com.xiaonei.xce.buddy.BuddyAdapter", "com.xiaonei.xce.passport.Login" };

  /** 应该在一个一定被使用的地方触发，目前放在ActiveTrack类中 */
  public static final void reportToXceLogger() {
    Thread t = new Thread(new Runnable() {

      @Override
      public void run() {
        String hostname = IPAddress.getLocalHostName();
        String address = IPAddress.getLocalAddress();
        for (String name : CLAZZS) {
          String txt = xce.util.tools.Version.getVersion(name);
          String log = DateFormatter.format(new Date()) + "\t" + hostname + "("
              + address + ")\t" + name + "\t" + txt;
          Oce.getLogger().info(log);
          XceLoggerAdapter.getLogger().addLog(
              new String[] { "xcejars", "version" }, log);
        }
      }
    });
    t.setDaemon(true);
    t.start();
  }

  /**
   * @param args
   * @throws IOException
   */
  public static void main(String[] args) {
    String hostname = IPAddress.getLocalHostName();
    String address = IPAddress.getLocalAddress();
    for (String name : CLAZZS) {
      String txt = xce.util.tools.Version.getVersion(name);
      String log = DateFormatter.format(new Date()) + " " + hostname + "("
          + address + ")\t" + name + "\t" + txt;
      System.out.println(log);
    }
  }
}
