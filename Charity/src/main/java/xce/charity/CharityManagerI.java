package xce.charity;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.BitSet;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.atomic.AtomicInteger;

import Ice.Current;

public class CharityManagerI extends _CharityManagerDisp {
  private static final long serialVersionUID = -4832757535093984401L;
  private final String _path;
  private BitSet _bits = new BitSet(Integer.MAX_VALUE);
  private BlockingDeque<Integer> _to_be_seted = new LinkedBlockingDeque<Integer>();
  private AtomicInteger _count = new AtomicInteger(0);
  private Checking _checking_thread = new Checking();
  private Timer _saving_timer = new Timer();

  protected CharityManagerI(String path) {
    this._path = path;
  }

  protected void initialize() {
    ObjectInputStream is = null;
    try {
      is = new ObjectInputStream(new FileInputStream(_path));
      _bits = (BitSet) is.readObject();
      _count.set(_bits.cardinality());
    } catch (Throwable e) {
      e.printStackTrace();
    } finally {
      if (null != is) {
        try {
	  is.close();
        } catch (Throwable e) {
	  e.printStackTrace();
        }
      }
    }
    _checking_thread.setDaemon(true);
    _checking_thread.start();
    _saving_timer.schedule(new Saving(), 2 * 60 * 1000, 2 * 60 * 1000);
  }

  private class Checking extends Thread {

    @Override
    public void run() {
      while (true) {
        try {
          Integer id = _to_be_seted.takeLast();
          synchronized (_bits) {
            if (!_bits.get(id)) {
              _bits.set(id);
              _count.incrementAndGet();
            }
          }
        } catch (Throwable e) {
          e.printStackTrace();
        }
      }

    }

  }

  private class Saving extends TimerTask {

    @Override
    public void run() {
	ObjectOutputStream os = null;
      try {
        synchronized (_bits) {
          os = new ObjectOutputStream(new FileOutputStream(
              _path));
          os.writeObject(_bits);
        }
      } catch (Throwable e) {
        e.printStackTrace();
      } finally {
	if (null != os) {
	  try {
		os.close();
	  } catch (Throwable e) {
		e.printStackTrace();
	  }
	}
      }
    }

  }

  @Override
  public int count(Current arg0) {
    return _count.get();
  }

  @Override
  public int next(int arg0, Current arg1) {
    synchronized (_bits) {
      return _bits.nextSetBit(arg0);
    }
  }

  @Override
  public boolean set(int arg0, boolean arg1, Current arg2) {
    if (arg1) {
      _to_be_seted.offerFirst(arg0);
      return true;
    }
    return false;
  }

}
