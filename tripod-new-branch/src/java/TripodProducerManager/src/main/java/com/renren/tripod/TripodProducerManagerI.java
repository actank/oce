package com.renren.tripod;

import java.util.Map;

import Ice.Current;

public class TripodProducerManagerI extends _ProducerManagerDisp {
	/**
	 * 
	 */
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(TripodProducerManagerI.class);
	private static final long serialVersionUID = 1L;
	
	private static CallFilter _callFilter = new CallFilter();

  private TripodProducerFactory _factory = new TripodProducerFactory();

	@Override
	public Map<String, String> produce(String[] keys, String business,
			boolean isMaster, Current current)
			throws TripodProducerNotSupportedException {
		_logger.info("TripodProducerManagerI.produce key size" + keys.length
				+ " business " + business + " isMaster " + isMaster);

		if(_callFilter.filter(business) == false) {
			return null;
		}
		
		try {
			TripodProducer producer = _factory.get(business);
			if (null == producer) {
				throw new TripodProducerNotSupportedException();
			}

			return producer.create(keys, isMaster);			
		}finally {
			_callFilter.exitFilter(business);
		}
	}

	@Override
	public String[] produceKeys(String beginKey, int limit, String business,
			boolean hotKey, Current current)
			throws TripodProducerNotSupportedException {
		_logger.info("TripodProducerManagerI.produceKeys beginKey " + beginKey
				+ " limit " + limit + " business " + business + " hotKey "
				+ hotKey);

		if(_callFilter.filter(business) == false) {
			return null;
		}
		
		try {
			TripodProducer producer = _factory.get(business);
			if (null == producer) {
				throw new TripodProducerNotSupportedException();
			}
			if (hotKey) {
				return producer.createHotKeys(beginKey, limit);
			} else {
				return producer.createKeys(beginKey, limit);
			}		
		}finally {
			_callFilter.exitFilter(business);
		}
		
	}

}
