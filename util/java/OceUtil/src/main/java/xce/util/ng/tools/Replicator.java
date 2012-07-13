package xce.util.ng.tools;

import java.util.Iterator;
import java.util.NoSuchElementException;

public class Replicator<T> implements Iterable<T> {
  public class Itr implements Iterator<T> {
    private int _pos = 0;

    @Override
    public boolean hasNext() {
      return _objs.length > 0;
    }

    @Override
    public synchronized T next() {
      if (_pos >= _objs.length)
        _pos = 0;
      return _objs[_pos++];
    }

    /**
     * always throws a NoSuchElementException
     */
    @Override
    public void remove() {
      throw new NoSuchElementException("Replicator cannot be modified.");
    }

  }

  private ReplicatorFactory<T> _factory;
  private T[] _objs;

  public Replicator(ReplicatorFactory<T> factory, T[] space) {
    _factory = factory;
    _objs = space;
    for (int pos = 0; pos < _objs.length; ++pos) {
      _objs[pos] = _factory.create();
    }
  }

  @Override
  public Iterator<T> iterator() {
    return new Itr();
  }
}
