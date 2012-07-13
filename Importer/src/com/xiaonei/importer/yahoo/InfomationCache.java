package com.xiaonei.importer.yahoo;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Queue;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.OwnerInfo;

public class InfomationCache {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(InfomationCache.class);

	private static InfomationCache _instance = new InfomationCache();

	public static InfomationCache getInstance() {
		return _instance;
	}

	public InfomationCache() {
		_uuid = new LinkedList<UUID>();
		_uuuserId = new HashMap<UUID, Integer>();
		_uuidCache = new ConcurrentHashMap<Integer, Map<String, UUID>>();
		_tokenCache = new ConcurrentHashMap<UUID, String>();
		_contactsCache = new ConcurrentHashMap<UUID, BuddyList>();
		_thread = new CheckThread();
		_thread.start();
	}

	private Thread _thread;

	private Queue<UUID> _uuid;
	private Map<UUID, Integer> _uuuserId;
	private Map<Integer, Map<String, UUID>> _uuidCache;
	private Map<UUID, String> _tokenCache;
	private Map<UUID, BuddyList> _contactsCache;

	public void putUUID(OwnerInfo owner, UUID id) {
		_uuid.offer(id);
		_uuuserId.put(id, owner.userId);
		if (_logger.isDebugEnabled()) {
			_logger.debug("putUUID owner=" + owner.userId + " " + owner.email + " " + owner.password + " " + id);
		}
		if (_uuidCache.containsKey(owner.userId)) {
			if (_logger.isTraceEnabled()) {
				_logger.trace("owner.userId=" + owner.userId + " exists in _uuidCache");
			}
			Map<String, UUID> accounts = _uuidCache.get(owner.userId);
			if (accounts.containsKey(owner.email)) {
				accounts.remove(owner.email);
				if (_logger.isTraceEnabled()) {
					_logger.trace("remove exist request from cache. owner.userId=" + owner.userId + " owner.email="
							+ owner.email);
				}
			}
			if (_logger.isTraceEnabled()) {
				_logger.trace("put new request into cache. owner.userId=" + owner.userId + " owner.email="
						+ owner.email);
			}
			accounts.put(owner.email, id);
		} else {
			if (_logger.isTraceEnabled()) {
				_logger.trace("put this into cache: owner.userId=" + owner.userId + " owner.email=" + owner.email);
			}
			Map<String, UUID> accounts = new HashMap<String, UUID>();
			accounts.put(owner.email, id);
			_uuidCache.put(owner.userId, accounts);
		}
	}

	public UUID getUUID(OwnerInfo owner) {
		if (_logger.isTraceEnabled()) {
			_logger.trace("getUUID owner.userId=" + owner.userId + " owner.email=" + owner.email);
		}
		if (_uuidCache.containsKey(owner.userId)) {
			if (_logger.isTraceEnabled()) {
				_logger.trace("owner.userId=" + owner.userId);
			}
			Map<String, UUID> accounts = _uuidCache.get(owner.userId);
			if (accounts.containsKey(owner.email)) {
				if (_logger.isTraceEnabled()) {
					_logger.trace("owner.email=" + owner.email);
				}
				return accounts.get(owner.email);
			}
		}
		throw new NoSuchElementException("No such request for OwnerInfo[userId=" + owner.userId + ",email="
				+ owner.email + ",password=" + owner.password + "]");
	}
	
	public void putToken(UUID id, String token) {
		_tokenCache.put(id, token);
	}

	public String getToken(UUID id) {
		return _tokenCache.get(id);
	}

	public void putContacts(UUID id, BuddyList contacts) {
		if (_logger.isTraceEnabled()) {
			_logger.trace("putContacts UUID[" + id + "] => BuddyList[" + contacts.status.code + ", "
					+ contacts.status.message + "]");
		}
		_contactsCache.put(id, contacts);
	}

	public BuddyList getContacts(UUID id) {
		if (_logger.isTraceEnabled()) {
			_logger.trace("getContacts UUID[" + id + "]");
		}
		return _contactsCache.get(id);
	}

	private class CheckThread extends Thread {
		@Override
		public void run() {
			try {
				if (_uuid.size() > 500) {
					_logger.info("Reducing uuid");
					while (_uuid.size() > 200) {
						try {
							UUID id = _uuid.poll();
							int userId = _uuuserId.get(id);
							_uuidCache.remove(userId);
							_tokenCache.remove(id);
							_contactsCache.remove(id);
						} catch (Exception e) {
							_logger.warn("CheckThread reducing error.", e);
						}
					}
					if (_logger.isInfoEnabled()) {
						_logger.info("Reducing cache done. size=" + _uuid.size() + "|" + _uuuserId.size() + "|"
								+ _uuidCache.size() + "|" + _tokenCache.size() + "|" + _contactsCache.size());
					}
				}
				Thread.sleep(5 * 1000);
			} catch (Exception e) {
				_logger.warn("CheckThread error.", e);
			}
		}
	}
}
