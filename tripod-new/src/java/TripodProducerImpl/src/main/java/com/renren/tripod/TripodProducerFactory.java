package com.renren.tripod;

import java.util.Map;
import java.util.HashMap;
import com.renren.tripod.impl.TestTripodProducer;

public class TripodProducerFactory {
	private Map<String, TripodProducer> _map = new HashMap<String, TripodProducer>();;
	
  public TripodProducerFactory() {
		_map.put("TEST", new TestTripodProducer());
	}

  TripodProducer get(String business) {
    return _map.get(business);
  }
}
