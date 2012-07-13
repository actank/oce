package xce.util.objectcache;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.concurrent.ConcurrentHashMap;

import xce.util.task.Task;
import xce.util.task.TaskManager;
import xce.util.tools.Closeable;

/**
 * ObjectEvictor 使用LRU策略
 * 
 **/
public final class ObjectEvictor<T extends Closeable> {

	private class CloseObjectTask extends Task {

		private final ArrayList<T> toCloseObjects;

		public CloseObjectTask(ArrayList<T> toCloseObjects) {
			this.toCloseObjects = toCloseObjects;
		}

		@Override
		public void run() {
			for (T toClose : toCloseObjects) {
				toClose.close();
			}
		}

	}

	private final ConcurrentHashMap<Integer, T> _objects = new ConcurrentHashMap<Integer, T>();
	private final LinkedList<Integer> _keys = new LinkedList<Integer>();

	private final ConcurrentHashMap<Integer, T> _q2 = new ConcurrentHashMap<Integer, T>();

	private final ObjectFactory<T> factory;
	private final int limitSize;

	public ObjectEvictor(ObjectFactory<T> factory, int size) {
		if (size <= 0) {
			throw new IllegalArgumentException("size must be positive");
		}
		this.factory = factory;
		this.limitSize = size;
	}

	public T locate(Integer key) {
		T result = find(key);
		if (result == null && factory != null) {
			result = factory.create(key);
			if (result != null) {
				_q2.put(key, result);
			}
		}
		return result;
	}

	public int size() {
		return _objects.size();
	}

	public T find(Integer key) {
		T result = _objects.get(key);
		if (result != null) {
			synchronized (_keys) {
				_keys.removeFirstOccurrence(key);
				_keys.addFirst(key);
			}
			return result;
		}
		result = _q2.get(key);
		if (result != null) {
			_q2.remove(key);
			_objects.put(key, result);
			ArrayList<T> toCloseObjects = new ArrayList<T>();
			synchronized (_keys) {
				_keys.addFirst(key);
				while (_keys.size() > limitSize) {
					T removedObject = _objects.remove(_keys.removeLast());
					if (removedObject != null) {
						toCloseObjects.add(removedObject);
					} else {
						/* TODO this condition means some rare situation. */
					}
				}
			}
			TaskManager.getInstance().push(new CloseObjectTask(toCloseObjects));
			return result;
		}
		return result;
	}
}
