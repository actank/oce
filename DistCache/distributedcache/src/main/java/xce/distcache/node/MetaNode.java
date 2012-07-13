package xce.distcache.node;

import java.util.List;

public interface MetaNode extends Node {
    public void alloc(Item item);

    public List<Long> get(int blockId);
}
