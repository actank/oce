package com.renren.tripod.configclient;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class HashHolder<T> {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(HashHolder.class);

	private int _virtualNodes = 100;
	private TreeMap<Long, T> _config;

	private ReadWriteLock _lock = new ReentrantReadWriteLock();

	public HashHolder(int virtualNodes) {
		_logger.info("HashHolder.HashHolder virtualNodes " + virtualNodes);
		if (virtualNodes >= 1) {
			_virtualNodes = virtualNodes;
		}
	}

	public void renew(Map<String, T> newConfig) {
		_logger.info("HashHolder.renew");
		TreeMap<Long, T> config = hashConfig(newConfig);
		try {
			_lock.writeLock().lock();
			_config = config;
		} finally {
			_lock.writeLock().unlock();
		}
	}

	public T get(String key) {
		try {
			_lock.readLock().lock();
			SortedMap<Long, T> tMap = _config.tailMap(hashKey(key));
			return tMap.isEmpty() ? _config.firstEntry().getValue() : tMap
					.get(tMap.firstKey());
		} finally {
			_lock.readLock().unlock();
		}
	}

	public Map<T, List<String>> get(List<String> keys) {
		Map<T, List<String>> res = new HashMap<T, List<String>>();
		try {
			_lock.readLock().lock();
			for (String key : keys) {
				SortedMap<Long, T> tMap = _config.tailMap(hashKey(key));
				T t = tMap.isEmpty() ? _config.firstEntry().getValue() : tMap
						.get(tMap.firstKey());
				if (res.containsKey(t)) {
					res.get(t).add(key);
				} else {
					List<String> newKeys = new ArrayList<String>();
					newKeys.add(key);
					res.put(t, newKeys);
				}
			}
		} finally {
			_lock.readLock().unlock();
		}
		return res;
	}

	private TreeMap<Long, T> hashConfig(Map<String, T> config) {
		_logger.info("HashHolder.hashConfig");
		TreeMap<Long, T> res = new TreeMap<Long, T>();
		MessageDigest md5 = MD5.get();
		for (Entry<String, T> ent : config.entrySet()) {
			for (int i = 0; i < _virtualNodes / 4; ++i) {
				byte[] d = md5.digest((ent.getKey() + String.valueOf(i))
						.getBytes());
				for (int h = 0; h < 4; ++h) {
					Long k = ((long) (d[3 + h] & 0xFF) << 24)
							| ((long) (d[2 + h] & 0xFF) << 16)
							| ((long) (d[1 + h] & 0xFF) << 8)
							| ((long) (d[0 + h] & 0xFF));

					res.put(k, ent.getValue());
				}
			}
		}
		return res;
	}

	private Long hashKey(String key) {
		MessageDigest md5 = MD5.get();
		md5.reset();
		//md5.update(key.getBytes());
		byte[] bKey = md5.digest(key.getBytes());
		long res = ((long) (bKey[3] & 0xFF) << 24)
				| ((long) (bKey[2] & 0xFF) << 16)
				| ((long) (bKey[1] & 0xFF) << 8) | (long) (bKey[0] & 0xFF);
		return res;

	}

	private static ThreadLocal<MessageDigest> MD5 = new ThreadLocal<MessageDigest>() {
		@Override
		protected MessageDigest initialValue() {
			try {
				return MessageDigest.getInstance("MD5");
			} catch (NoSuchAlgorithmException e) {
				_logger.error("HashHolder.ThreadLocal<MessageDigest> no md5 algorithm found");
				throw new IllegalStateException(
						"HashHolder.ThreadLocal<MessageDigest> no md5 algorythm found");
			}
		}
	};
}
