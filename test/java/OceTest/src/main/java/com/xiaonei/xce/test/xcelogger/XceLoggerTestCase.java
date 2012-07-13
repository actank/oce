package com.xiaonei.xce.test.xcelogger;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.List;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.log.ActiveTrack;
import com.xiaonei.xce.log.XceLoggerReplicaAdapter;

@RunWith(Parameterized.class)
public class XceLoggerTestCase {

  private int _userId;
  private String _ctrlEndpoints;
  private String _args[];
  private String _loggerEndpoints;
  private String _serverHost;
  private String _logPath;
  private String _path;
  private String _action;

  public XceLoggerTestCase(int userId, String ctrlEndpoints, String args[], String loggerEndpoints, String serverHost,
      String logPath, String path, String action) {
    this._userId = userId;
    this._ctrlEndpoints = ctrlEndpoints;
    this._args = args;
    this._loggerEndpoints = loggerEndpoints;
    this._serverHost = serverHost;
    this._logPath = logPath;
    this._path = path;
    this._action = action;
  }

  @Parameters
  public static Collection<Object[]> setParameters() {
    return Arrays.asList(new Object[][] { { 222525090, "ControllerXceLogger",
        new String[] { "Ice.Default.Host=10.2.17.249" }, "M:udp -h 10.3.17.160 -p 9900", "10.3.17.160",
        "/data/xce/LogData/", "test", "ll" }, // 校内
        });
  }

  @BeforeClass
  public static void setUpBeforeClass() throws Exception {
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
  }

  @Before
  public void setUpBefore() throws Exception {
  }

  @After
  public void tearDownAfter() throws Exception {
  }

  @Test
  public void Test() {

    // 生成时间
    Date d = new Date();
    DateFormat dfHour = new SimpleDateFormat("yyyy-MM-dd-HH");
    DateFormat dfSec = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    String timeHour = dfHour.format(d);
    String timeSec = dfSec.format(d);
    System.out.println(" - Time - " + timeSec);
    int circle = 20;

    // 测试XceReplicaAdapter的3个接口
    String[] paths = new String[] { _path, _action };
    String logFilePath = _logPath + "/logger/" + _path + "/" + _action + "." + timeHour;
    String message1 = "[X1]" + timeSec;
    String message2 = "[X2]" + timeSec;
    String message3 = "[X3]" + timeSec;
    System.out.println(" - LOG FILE - " + logFilePath);
    System.out.println(" - MESSAGE 1 - " + message1);
    System.out.println(" - MESSAGE 2 - " + message2);
    System.out.println(" - MESSAGE 3 - " + message3);
    List<String> args = new ArrayList<String>();
    for (String arg : _args) {
      args.add(arg);
    }
    // 打log
    for (int i = 0; i < circle; ++i) {
      System.out.print(i + " ");
      XceLoggerReplicaAdapter.getLogger().addLog(paths, message1);
      XceLoggerReplicaAdapter.getLogger(_ctrlEndpoints).addLog(paths, message2);
      XceLoggerReplicaAdapter.getLoggerDirectly(args, _loggerEndpoints).addLog(paths, message3);
      try {
        Thread.sleep(1000);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    System.out.println();
    // 进行检验
    String shell[] = new String[] { "/bin/bash", "-c",
        "ssh xce@" + _serverHost + " \"tail -" + (circle * 3) + " " + logFilePath + "\"" };
    boolean flagX1 = false;
    boolean flagX2 = false;
    boolean flagX3 = false;
    try {
      Process p = Runtime.getRuntime().exec(shell);
      InputStreamReader ir0 = new InputStreamReader(p.getInputStream());
      LineNumberReader ir = new LineNumberReader(ir0);
      String line = "";
      while (line != null) {
        if (line.contains(message1)) {
          flagX1 = true;
        } else if (line.contains(message2)) {
          flagX2 = true;
        } else if (line.contains(message3)) {
          flagX3 = true;
        }
        if (flagX1 && flagX2 && flagX3) {
          break;
        }
        line = ir.readLine();
      }
      System.out.println("[Result - XceLoggerReplicaAdapter TestCase - 1] : " + flagX1);
      System.out.println("[Result - XceLoggerReplicaAdapter TestCase - 2] : " + flagX2);
      System.out.println("[Result - XceLoggerReplicaAdapter TestCase - 3] : " + flagX3);
      Assert.assertTrue(flagX1);
      Assert.assertTrue(flagX2);
      Assert.assertTrue(flagX3);
    } catch (IOException e) {
      e.printStackTrace();
    }

    // 测试ActiveTrack的接口
    String actionFilePath = _logPath + "/logger/ActiveTrackXN/action/bucket_" + (_userId % 10) + "." + timeHour;
    System.out.println(" - ActionFile - " + actionFilePath);
    String object = "[OBJECT] " + timeSec;
    for (int i = 0; i < circle; ++i) {
      System.out.print(i + " ");
      ActiveTrack.action(_userId, paths, _action, object);
      try {
        Thread.sleep(1000);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    System.out.println();
    // 进行检验
    shell = new String[] { "/bin/bash", "-c",
        "ssh xce@" + _serverHost + " \"tail -" + (circle * 10000) + " " + actionFilePath + "\"" };
    boolean flagA = false;
    try {
      Process p = Runtime.getRuntime().exec(shell);
      InputStreamReader ir0 = new InputStreamReader(p.getInputStream());
      LineNumberReader ir = new LineNumberReader(ir0);
      String line = "";
      while (line != null) {
        // System.out.println(line);
        if (line.contains(object)) {
          flagA = true;
          break;
        }
        line = ir.readLine();
      }
      System.out.print("[Result - ActiveTrack TestCase] : " + flagA);
      Assert.assertTrue(flagA);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}