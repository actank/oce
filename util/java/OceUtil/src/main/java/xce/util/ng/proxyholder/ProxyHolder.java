package xce.util.ng.proxyholder;

public interface ProxyHolder<T extends Ice.ObjectPrx> {
  public T uncheckedCast(Ice.ObjectPrx baseProxy);
}
