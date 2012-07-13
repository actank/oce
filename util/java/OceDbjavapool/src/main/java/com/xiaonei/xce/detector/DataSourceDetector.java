package com.xiaonei.xce.detector;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.pool.DataSourceInstanceIF;

public class DataSourceDetector extends Thread {
    org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory.getLog(XceAdapter.class);
    private final long DETECTE_INTERVAL = 15000;
    
    private static DataSourceDetector _instance = new DataSourceDetector();
    
    public static DataSourceDetector getInstance() {
	return _instance;
    }
    
    private Lock _lock;
    private Map<String, DataSourceInstanceIF> _map;
    private PriorityQueue<DetectorEntry> _queue;
    private Condition _condition;
    
    private DataSourceDetector(){
	_lock=new ReentrantLock();
	_map=new HashMap<String, DataSourceInstanceIF>();
	_queue=new PriorityQueue<DetectorEntry>();
	_condition=_lock.newCondition();
	start();
    }
    
    public void redetect(String name, DataSourceInstanceIF instance,String strings){
	try{
	    _lock.lock();
	    if(!_map.containsKey(name)){
		_map.put(name, instance);
		_queue.offer(new DetectorEntry(name, 0, new Date().getTime()+DETECTE_INTERVAL,strings));
		if(_map.size()==1){
		    _condition.signal();
		}
	    }
	}finally{
	    _lock.unlock();
	}
    }
    
    public void run(){
	while(true){
	    try{
		try{
		    _lock.lock();
		    DetectorEntry entry = _queue.poll();
		    if(entry==null){
			_condition.await();
			continue;
		    }
		    long left = entry.getTime() - new Date().getTime();
		    if(left > 0){
			_condition.await(left, TimeUnit.MILLISECONDS);
		    }
		    if (isConnected(entry.getDescription())) {
			_map.remove(entry.getName());
			continue;
		    }
		    entry.incCount(1);
		    if(entry.getCount()>=5){
			_map.get(entry.getName()).cutReadServer(entry.getDescription());
			XceAdapter.getInstance().alert(entry.getName(), entry.getDescription());
			_map.remove(entry.getName());
		    }else{
			entry.setTime(new Date().getTime()+DETECTE_INTERVAL);
			_queue.offer(entry);
		    }
		}finally{
		    _lock.unlock();
		}
	    }catch(Throwable e){
		e.printStackTrace();
	    }
	}
    }
    
    private boolean isConnected(String description){
  Socket s=null;
	try {
	    String[] split = description.split("\\:");
	    int p = Integer.parseInt(split[2]);
	    s = new Socket();
	    s.connect(new InetSocketAddress(split[1], p), 300);
	    return true;
	} catch (UnknownHostException e) {
	    e.printStackTrace();
	} catch (IOException e) {
	    e.printStackTrace();
	} catch (Exception e){
	    e.printStackTrace();
	} finally {
    if (s!=null){
      try {
        s.close();
      } catch (Exception e){}
    }
  }
	_logger.warn(description +" isn't connected");
	return false;
    }
}
