package com.xiaonei.xce.log;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class CombinedLog {

  private String remote_host;
  private String remote_logname;
  private long remote_user;
  private long time;
  private String first_line_of_request;
  private int status;
  private int bytes_sent;
  private String refer;
  private String user_agent;
  static private DateFormat df = new SimpleDateFormat("dd/MM/yy:HH:mm:ss Z");

  /**
   * 构造apache httpd CombinedLog格式的log
   * 
   * @param remote_host
   *          用户ip
   * @param remote_logname
   *          远程logname，如无特殊设置可为"-"
   * @param remote_user
   *          用户id，未登录用户为0
   * @param time
   *          行为时间 (long格式，在写入log时自动转换为形如"[20/05/11:15:44:59 +0800]"的格式
   * @param request_url
   *          用户访问的url
   * @param status
   *          本次访问的status，如200/404/502
   * @param refer
   *          本次访问refer
   * @param user_agent
   *          本次访问的user_agent信息，如"Mozilla/4.08 [en] (Win98; I ;Nav)"
   **/
  public CombinedLog(String remote_host, String remote_logname, long remote_user, long time,
      String first_line_of_request, int status, int bytes_sent, String refer, String user_agent) {
    this.remote_host = remote_host;
    this.remote_logname = remote_logname;
    this.remote_user = remote_user;
    this.time = time;
    this.first_line_of_request = first_line_of_request;
    this.status = status;
    this.bytes_sent = bytes_sent;
    this.refer = refer;
    this.user_agent = user_agent;
  }

  public void setRemoteHost(String remote_host) {
    this.remote_host = remote_host;
  }

  public String getRemoteHost() {
    return remote_host;
  }

  public void setRemoteLogName(String remote_logname) {
    this.remote_logname = remote_logname;
  }

  public String getRemoteLogName() {
    return remote_logname;
  }

  public void setRemoteUser(long remote_user) {
    this.remote_user = remote_user;
  }

  public long getRemoteUser() {
    return remote_user;
  }

  public void setTime(long time) {
    this.time = time;
  }

  public long getTime() {
    return time;
  }

  public void setRequestUrl(String first_line_of_request) {
    this.first_line_of_request = first_line_of_request;
  }

  public String getFirstLineOfRequest() {
    return first_line_of_request;
  }

  public void setStatus(int status) {
    this.status = status;
  }

  public int getStatus() {
    return status;
  }

  public void setBytesSent(int bytes_sent) {
    this.bytes_sent = bytes_sent;
  }

  public int getByetsSent() {
    return bytes_sent;
  }

  public void setRefer(String refer) {
    this.refer = refer;
  }

  public String getRefer() {
    return refer;
  }

  public void setUserAgent(String user_agent) {
    this.user_agent = user_agent;
  }

  public String getUserAgent() {
    return user_agent;
  }

  public String toString() {
    StringBuffer buf = new StringBuffer();

    buf.append(remote_host).append(' ');
    buf.append(remote_logname).append(' ');
    buf.append(remote_user).append(' ');
    buf.append("[").append(df.format(new Date(time))).append("] ");
    buf.append(quote(first_line_of_request)).append(' ');
    buf.append(status).append(' ');
    buf.append(bytes_sent).append(' ');
    buf.append(quote(refer)).append(' ');
    buf.append(quote(user_agent)).append(' ');

    return buf.toString();
  }

  private static String quote(String field) {
    if (field == null) {
      return "\"" + null + "\"";
    } else {
      return "\"" + field.replaceAll("(\\\\)?\\\"", "\\\\\"") + "\"";
    }
  }

  public static void main(String args[]) {

    /*
     * CombinedLog log = new CombinedLog("10.2.17.249", "-", 222525090, new
     * Date().getTime(), "GET http://page.renren.com \"HTTP/1.0\"", 200, 60,
     * "baidu", "Mozilla/4.08 [en] \"(Win98; I ;Nav)\"");
     */
    CombinedLog log = new CombinedLog(null, null, 222525090, new Date().getTime(), null, 200, 60, null, null);
    System.out.println(log.toString());
    System.exit(0);
  }
}