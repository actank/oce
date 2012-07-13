package xce.distcache.node;

import java.util.List;
import java.util.Map;

import java.net.SocketAddress;

public interface ReadNode extends Node {
    public byte[] get(Item id);

    public Map<Item, byte[]> get(List<Item> id);

    public boolean internalSet(Item id, byte[] data);

    public boolean casSet(Item id, byte[] data);

    public void add(Item id, byte[] data);

    public Map<SocketAddress, Map<String, String>> getStats();
}
