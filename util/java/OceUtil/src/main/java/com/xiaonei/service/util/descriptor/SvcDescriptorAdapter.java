package com.xiaonei.service.util.descriptor;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.Map.Entry;

import xce.util.channel.Channel;
import xce.util.tools.IPAddress;
import Ice.ObjectAdapter;
import MyUtil.SvcDescriptorPrx;
import MyUtil.SvcDescriptorPrxHelper;
import MyUtil.SvcInstance;
import MyUtil.SvcObserverPrx;
import MyUtil.SvcObserverPrxHelper;

public class SvcDescriptorAdapter implements Runnable {

  protected Channel channel;

  protected ObjectAdapter adapter;

  private Thread thread;

  private SvcDescriptorPrx descriptor;
  private SvcDescriptorPrx descriptorOneway;

  private Map<String, ArrayList<SvcDescriptorListener>> listeners = new HashMap<String, ArrayList<SvcDescriptorListener>>();

  private static SvcDescriptorAdapter instance;

  public static SvcDescriptorAdapter getInstance() {
    if (instance == null) {
      synchronized (SvcDescriptorAdapter.class) {
        if (instance == null) {
          instance = new SvcDescriptorAdapter();
        }
      }
    }
    return instance;
  }

  private SvcDescriptorAdapter() {
    channel = Channel.newOceChannel("@SvcDescriptor");
    descriptor = channel.locate("SD", "@SvcDescriptor", Channel.Invoke.Twoway,
        SvcDescriptorPrxHelper.class, 300);
    descriptorOneway = channel.locate("SD", "@SvcDescriptor",
        Channel.Invoke.Oneway, SvcDescriptorPrxHelper.class, 300);
    String uuid = UUID.randomUUID().toString();
    channel.getProperties().setProperty(uuid + ".Endpoints",
        "tcp -h " + IPAddress.getLocalAddress());
    adapter = channel.getCommunicator().createObjectAdapter(uuid);
    adapter.activate();

    thread = new Thread(this);
    thread.start();
  }

  public synchronized void subscribe(String name, SvcDescriptorListener listener) {
    ArrayList<SvcDescriptorListener> list = listeners.get(name);
    if (list == null) {
      list = new ArrayList<SvcDescriptorListener>();
      listeners.put(name, list);
    }
    if (!list.contains(listener)) {
      list.add(listener);
      SvcObserverPrx observer = SvcObserverPrxHelper.uncheckedCast(adapter
          .addWithUUID(listener));
      descriptorOneway.subscribe(name, observer);
      listener.setSvcObserver(observer);
    }
  }

  public synchronized void unsubscribe(String name,
      SvcDescriptorListener listener) {
    ArrayList<SvcDescriptorListener> list = listeners.get(name);
    if (list != null && list.contains(listener)) {
      list.remove(listener);
      descriptorOneway.unsubscribe(name, listener.getSvcObserver());
    }
  }

  public void notify(String name, Map<String, String> props) {
    if (props == null) {
      descriptorOneway._notify(name);
    } else {
      descriptorOneway._notify(name, props);
    }
  }

  public void run() {
    while (true) {
      try {
        Thread.sleep(5 * 60 * 1000);
      } catch (Exception e) {
        e.printStackTrace();
      }
      for (Entry<String, ArrayList<SvcDescriptorListener>> entry : listeners
          .entrySet()) {
        try {
          SvcInstance svci = descriptor.query(entry.getKey());
          for (SvcDescriptorListener listener : entry.getValue()) {
            descriptorOneway.subscribe(entry.getKey(), listener
                .getSvcObserver());
            if (svci != null) {
              listener.update(svci, null);
            }
          }
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    }
  }

  public static void main(String[] args) throws Exception {
    System.out.println("start testing ...");
    // System.out.println(SvcDescriptorAdapter.getInstance().descriptor.query("com.xiaonei.platform.core.opt.base.AppMenuItemDescriptorListener").properties);
    SvcDescriptorListener listener = new SvcDescriptorListenerExample();
    SvcDescriptorAdapter.getInstance().subscribe(
        "com.xiaonei.platform.core.opt.base.AppMenuItemDescriptorListener",
        listener);
    while (true) {
      Map properties = new HashMap();
      properties.put("key", new Date().toString());
      SvcDescriptorAdapter.getInstance().notify(
          "com.xiaonei.platform.core.opt.base.AppMenuItemDescriptorListener",
          properties);
      Thread.sleep(1 * 1000);
    }
  }
}

class SvcDescriptorListenerExample extends SvcDescriptorListener {
  public boolean update(Map<String, String> properties) {

    return true;
  }
}
