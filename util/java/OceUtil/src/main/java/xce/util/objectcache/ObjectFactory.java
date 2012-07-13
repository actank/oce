package xce.util.objectcache;

import xce.util.tools.Closeable;

public interface ObjectFactory<T extends Closeable> {
	public T create(int key);
}
