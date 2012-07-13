package xce.distcache.node;

import java.util.Map;

public interface WriteNode extends Node {
    public void set(String name, Item id, byte[] data);

    public void set(String name, Map<Long, byte[]> bulkDatas);

    public void add(String name, Map<Long, byte[]> bulkDatas);
}
