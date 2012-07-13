package com.xiaonei.xce.log.log4j;

import java.util.ArrayList;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.Layout;
import org.apache.log4j.MDC;
import org.apache.log4j.spi.LoggingEvent;

import xce.util.tools.IPAddress;

import com.xiaonei.xce.log.XceLoggerReplica;
import com.xiaonei.xce.log.XceLoggerReplicaAdapter;

public class XceAppender extends AppenderSkeleton {
  private XceLoggerReplica logger;
  private ArrayList<String> args = new ArrayList<String>();
  private String loggerEndpoints;
  private String ctrlEndpoints;
  private static String localAddress = "";
  static {
    try {
      localAddress = IPAddress.getLocalAddress();
    } catch (Throwable e) {
      e.printStackTrace();
      localAddress = "";
    }
  }

  public void setCommunicatorInitArgs(String value) {
    args.add(value);
  }

  public void setLoggerEndpoints(String value) {
    loggerEndpoints = value;
  }

  @Override
  public void activateOptions() {
    super.activateOptions();
    if (null != args) {
      if (args.size() > 0) {
        logger = XceLoggerReplicaAdapter.getLoggerDirectly(args, loggerEndpoints);
        return;
      } else if ((loggerEndpoints != null)
          && (loggerEndpoints.matches("\\s*\\S+:.*((tcp)|(udp)).*\\-h\\s.*\\-p.*\\d+.*"))) {
        logger = XceLoggerReplicaAdapter.getLoggerDirectly(args, loggerEndpoints);
        return;
      }
    }

    if (loggerEndpoints != null) {
      if (loggerEndpoints.equals("M@XceLogger")) {
        ctrlEndpoints = "ControllerXceLogger";
      } else if (loggerEndpoints.startsWith("M@")) {
        ctrlEndpoints = loggerEndpoints.replace("M@", "Controller");
      } else if (loggerEndpoints.equals("")) {
        ctrlEndpoints = null;
      } else {
        ctrlEndpoints = loggerEndpoints;
      }
      logger = XceLoggerReplicaAdapter.getLogger(ctrlEndpoints);
    } else {
      logger = XceLoggerReplicaAdapter.getLogger();
    }
  }

  public XceAppender() {

  }

  public XceAppender(Layout layout) {
    this.layout = layout;
  }

  public boolean requiresLayout() {
    return true;
  }

  @Override
  protected void append(LoggingEvent event) {
    MDC.put("localAddress", localAddress);
    if (event.getThrowableInformation() == null) {
      if (logger != null) {
        try {
          logger.addLog(event.getLoggerName().split("\\."), layout.format(event));
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    } else {
      StringBuffer info = new StringBuffer();
      info.append(layout.format(event));
      for (String o : event.getThrowableStrRep()) {
        info.append(o).append("~");
      }
      logger.addLog(event.getLoggerName().split(" "), info.toString());
    }
  }

  public void close() {
    logger.close();
  }
}
