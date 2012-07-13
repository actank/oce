package xce.distcache.node;

public interface Node extends Comparable<Node> {
    public enum Type {
	Memcached, ICE, TokyoTyrant;
    }

    public static class Item {
	public Item(int block, long id) {
	    this.block = block;
	    this.id = id;
	    this.cas = 0;
	}

	public Item(int block, long id, long cas) {
	    this.block = block;
	    this.id = id;
	    this.cas = cas;
	}

	public final int block;
	public final long id;
	public final long cas;

	@Override
	public String toString() {
	    return "ReadOnlyNode.Item[" + block + "/" + id + "/" + cas + "]";
	}
    }

    public void initialize(String name, String config);

    public String getConfig();

    public void close();
}
