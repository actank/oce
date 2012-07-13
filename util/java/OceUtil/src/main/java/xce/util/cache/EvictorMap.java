package xce.util.cache;

import java.util.AbstractMap;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import Freeze.LinkedList;


public class EvictorMap<K,V> extends AbstractMap<K,V>{
	public EvictorMap() {
		this(10000);
	}

	public EvictorMap(int size) {
		if (size > 0) {
			this.size = size;
		} else {
			this.size = 10000;
		}
		
	}

	public int size() {
		return map.size();
	}

	public void setSize(int size) {
		if (size > 0) {
			this.size = size;
		}
	}
	
	public boolean isEmpty() {
		return map.size() == 0;
	}

	public  synchronized V get(Object key) {
		return  map.get(key).obj;
	}

	public synchronized V put(K key, V value) {
		if (map.containsKey(key)) {
			EvictorEntry entry = map.get(key);
			entry.pos.remove();
			map.remove(key);
		}
		EvictorEntry entry = new EvictorEntry();
		entry.obj = value;
		evictEntries();
		map.put(key, entry);
		hook(key, entry);
		return value;
	}
	
	private void hook(K key, EvictorEntry entry) {
		queue.addFirst(key);
		entry.pos = queue.iterator();
		entry.pos.next();
	}
	
	public V remove(Object key) {
		EvictorEntry entry = map.get(key);
		V oldValue = entry.obj;
		if (entry != null) {
			entry.pos.remove();
			map.remove(key);
		}
		return oldValue;
	}

	public void clear() {
		map.clear();
	}

	
	private void evictEntries() {
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
		public V obj;
		public Iterator pos;
	}

	public Map<Object, EvictorEntry> map = new HashMap<Object, EvictorEntry>();

	public LinkedList queue = new LinkedList();

	private int size;

	@Override
	public Set<java.util.Map.Entry<K, V>> entrySet() {
		// TODO Auto-generated method stub
		return null;
	}

}
