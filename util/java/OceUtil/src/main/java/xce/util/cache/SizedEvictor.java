/*
 * Created on 2005-10-25
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
package xce.util.cache;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import Freeze.LinkedList;

/**
 * @author Administrator
 * 
 * TODO To change the template for this generated type comment go to Window -
 * Preferences - Java - Code Style - Code Templates
 */
public class SizedEvictor {

	public SizedEvictor(Factory factory) {
		this(factory, 10000);
	}

	public SizedEvictor(Factory factory, int size) {
		this.factory = factory;
		if (size > 0) {
			this.size = size;
		} else {
			this.size = 10000;
		}
	}

	public void setSize(int size) {
		if (size > 0) {
			this.size = size;
		}
	}

	public synchronized void add(Object obj, Object id) {
		if (map.containsKey(id)) {
			EvictorEntry entry = map.get(id);
			entry.pos.remove();
			map.remove(id);
		}
		EvictorEntry entry = new EvictorEntry();
		entry.obj = obj;
		map.put(id, entry);

		hook(id, entry);
	}

	public synchronized void remove(Object id) {
		EvictorEntry entry = map.get(id);
		if (entry != null) {
			entry.pos.remove();
			map.remove(id);
		}
	}

	public synchronized boolean hasObject(Object id) {
		return map.containsKey(id);
	}

	public synchronized Object locate(Object id) {
		evict();
		// Check if we have a object in the map already
		EvictorEntry entry = map.get(id);
		if (entry != null) {
			// Got the entry already, dequeue the entry from
			// its current position.
			entry.pos.remove();
		} else {
			// We do not have a entry. Ask the factory to instantiate a
			// object and create a new entry to the map.
			if (factory == null) {
				return null;
			}
			entry = new EvictorEntry();
			entry.obj = factory.create(id);
			if (entry.obj == null) {
				return null;
			}
			map.put(id, entry);
		}

		// Enqueue the entry at the front, so we got the LRU order.
		hook(id, entry);

		return entry.obj;
	}

	private void hook(Object id, EvictorEntry entry) {
		queue.addFirst(id);
		entry.pos = queue.iterator();
		entry.pos.next();
	}

	private void evict() {
		// If the evictor queue has grown larger than the limit,
		// look at the excess elements to see whether any of them can be
		// evicted.
		while (map.size() > size) {
			Iterator p = queue.riterator();
			Object id = p.next();
			p.remove();
			map.remove(id);
		}
	}

	class EvictorEntry {
		public Object obj;

		public Iterator pos;
	}

	public Map<Object, EvictorEntry> map = new HashMap<Object, EvictorEntry>();

	public LinkedList queue = new LinkedList();

	private int size;

	private Factory factory;
}
