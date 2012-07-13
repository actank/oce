package com.renren.xce.service.ad;


import org.apache.log4j.Logger;

import Ice.Communicator;
import com.renren.xce.impl.ad.AdNotifierI;



public class AdHotPushService implements IceBox.Service {

    private Ice.ObjectAdapter _adapter;
    private final Logger log = Logger.getLogger(AdHotPushService.class.getName());
    
    @Override
    public void start(String name, Communicator communicator, String[] args) {
        _adapter = communicator.createObjectAdapter(name);

        log.info("add object to adapter");
        System.out.println("add object to adapter");
        
        _adapter.add(new AdNotifierI(), Ice.Util.stringToIdentity("adhotnotifier"));
        _adapter.activate();
    }

    @Override
    public void stop() {
        _adapter.destroy();
    }   
}
