package com.renren.tripod.node.impl.ice.server;

import com.renren.tripod.util.Constant;

import Ice.Communicator;
import Ice.ObjectAdapter;

public class WriteService implements IceBox.Service {

    private ObjectAdapter adapter_;

    private WriteServerI server_;

    private static final String ZK_ADDRESS = System
            .getProperty(
                    "ZK_ADDRESS",
                    Constant.ZOOKEEPER_ADDR);

    @Override
    public void start(String name, Communicator ic, String[] args) {
        this.adapter_ = ic.createObjectAdapter(name);
        this.server_ = new WriteServerI();
      
        try {
            this.server_.initialize(ZK_ADDRESS);
            this.adapter_.add(this.server_, ic.stringToIdentity("M"));
            this.adapter_.activate();
        } catch (Throwable e) {
            e.printStackTrace();
            System.exit(-1);
        }
        System.out.println("LogicService started");
    }

    @Override
    public void stop() {
        this.server_.close();
        this.adapter_.deactivate();
        this.adapter_.destroy();
    }
}