package com.renren.tripod.node.impl.ice.server;

import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import Ice.Current;
import com.renren.tripod.TripodData;
import com.renren.tripod._WriteServerDisp;
import com.renren.tripod.config.communicator.Communicator;
import com.renren.tripod.config.communicator.CommunicatorFactory;
import com.renren.tripod.util.TripodException;

public class WriteServerI extends _WriteServerDisp{
    private static org.apache.commons.logging.Log logger_ = org.apache.commons.logging.LogFactory
    .getLog(WriteServerI.class);
    private static final long serialVersionUID = 3270642203159936469L;
    
    private BlockingQueue<Runnable> queue_;
    private ThreadPoolExecutor exec_;
    private String zkaddr_;
    private Communicator communicator_;
    
    public void initialize(String addr) throws TripodException{
        zkaddr_ = addr;
        queue_ = new LinkedBlockingDeque<Runnable>();
        exec_ = new ThreadPoolExecutor(40, 100, 10, TimeUnit.SECONDS, queue_);
       
        communicator_ = CommunicatorFactory.create(addr);      
    }
    
    public void close(){
        
    }
    
    @Override
    public boolean add(String name, TripodData[] data, boolean isSync, Current current) {
        // TODO Auto-generated method stub
        return false;
    }
    @Override
    public List<Long> set(String name, TripodData[] data, boolean isSync, Current current) {
        // TODO Auto-generated method stub
        return null;
    }
    @Override
    public void setLogLevel(String level, Current current) {
        // TODO Auto-generated method stub
        
    }
    
}
