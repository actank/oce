package xce.util.objectcache;

import xce.util.tools.Closeable;

public class ObjectCache<T extends Closeable> {
	private static final int CHUNK_SIZE = 1021;;

	private ObjectEvictor<T>[] chunks;

	@SuppressWarnings("unchecked")
	public ObjectCache(ObjectFactory<T> factory, int size) {
		chunks = new ObjectEvictor[CHUNK_SIZE];
		for (int i = 0; i < CHUNK_SIZE; ++i) {
			chunks[i] = new ObjectEvictor<T>(factory, size / CHUNK_SIZE + 1);
		}
	}

	public T locate(int id) {
		return chunks[Math.abs(id) % CHUNK_SIZE].locate(id);
	}

	public T find(int id) {
		return chunks[Math.abs(id) % CHUNK_SIZE].find(id);
	}

}
