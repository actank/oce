package com.renren.tripod.config.communicator;

import java.util.HashMap;
import java.util.Map;

import com.renren.tripod.util.TripodException;

public class CommunicatorFactory {

    static Map<String, Communicator> map_ = new HashMap<String, Communicator>();

    /**
     * @param locator ZooKeeperAddress + "/Tripod"
     * @return
     * @throws TripodException
     */
    public static synchronized Communicator create(String locator) throws TripodException {
        Communicator communicator = map_.get(locator);
        if (null != communicator) return communicator;
        communicator = new Communicator();

        try {
            communicator.initialize(locator);
        } catch (TripodException e) {
            throw e;
        }
        map_.put(locator, communicator);

        return communicator;
    }
}
