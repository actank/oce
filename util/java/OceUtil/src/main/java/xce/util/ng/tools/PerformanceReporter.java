package xce.util.ng.tools;

import java.io.PrintStream;
import java.util.Date;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import xce.util.tools.DateFormatter;

public class PerformanceReporter {
  private final String _name;
  private final AtomicInteger _count;
  private final AtomicLong _time;
  private PrintRepeat _repeatThread;

  private class PrintRepeat extends Thread {
    private final PrintStream _out;
    private final long _interval;

    public PrintRepeat(PrintStream out, long interval) {
      _out = out;
      _interval = interval;
    }

    @Override
    public void run() {
      while (true) {
        try {
          print(_out);
          sleep(_interval);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    }
  }

  public PerformanceReporter(String name) {
    _name = name;
    _count = new AtomicInteger();
    _time = new AtomicLong();
  }

  public void record(int count, long time) {
    _count.addAndGet(count);
    _time.addAndGet(time);
  }

  public void print(PrintStream out) {
    long time = _time.get();
    int count = _count.get();
    out.println(DateFormatter.format(new Date()) + " " + _name + " " + count
        + " times cost " + time + " ms");
  }

  public void reset() {
    _count.set(0);
    _time.set(0);
  }

  public synchronized void printRepeat(PrintStream out, long interval) {
    if (_repeatThread != null) {
      return;
    }
    _repeatThread = new PrintRepeat(out, interval);
    _repeatThread.start();
  }
}
