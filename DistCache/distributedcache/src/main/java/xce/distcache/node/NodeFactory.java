package xce.distcache.node;

import xce.distcache.DistributedCacheSchema;
import xce.distcache.DistributedCacheSchema.NodeTypeEnum;
import xce.distcache.node.impl.ICELoadNode;
import xce.distcache.node.impl.ICEMetaNode;
import xce.distcache.node.impl.ICEWriteNode;
import xce.distcache.node.impl.MemcachedReadNode;
import xce.distcache.node.impl.TTReadNode;

/**
 * 选择创建正确的Backend代理
 * 
 * */
public class NodeFactory {

	public static Node create(DistributedCacheSchema.NodeCategoryEnum category,
			DistributedCacheSchema.NodeTypeEnum type, String name, String config) {
		if (category == DistributedCacheSchema.NodeCategoryEnum.META) {
			return createMetaNode(type, name, config);
		} else if (category == DistributedCacheSchema.NodeCategoryEnum.READ) {
			return createReadNode(type, name, config);
		} else if (category == DistributedCacheSchema.NodeCategoryEnum.WRITE) {
			return createWriteNode(type, name, config);
		} else if (category == DistributedCacheSchema.NodeCategoryEnum.LOAD) {
			return createLoadNode(type, name, config);
		} else {
			return null;
		}
	}

    public static MetaNode createMetaNode(NodeTypeEnum type, String name,
	    String config) {
	return createMetaNode(translate(type), name, config);
    }

	public static MetaNode createMetaNode(Node.Type type, String name,
			String config) {
		switch (type) {
		case ICE:
			ICEMetaNode b = new ICEMetaNode();
			b.initialize(name, config);
			return b;
		default:
			break;
		}
		return null;
	}

  	private static Node.Type translate(DistributedCacheSchema.NodeTypeEnum type) {
		if (type.equals(DistributedCacheSchema.NodeTypeEnum.MEMCACHED)) {
			return Node.Type.Memcached;
		} else if (type.equals(DistributedCacheSchema.NodeTypeEnum.ICE)) {
			return Node.Type.ICE;
		} else if (type.equals(DistributedCacheSchema.NodeTypeEnum.TOKYOTYRANT)) {
			return Node.Type.TokyoTyrant;
		} else {
			return null;
		}
	}

	public static ReadNode createReadNode(Node.Type type, String name,
			String config) {
		switch (type) {
		case Memcached:
			MemcachedReadNode b = new MemcachedReadNode();
			b.initialize(name, config);
			return b;
		case TokyoTyrant:
			TTReadNode n = new TTReadNode();
			n.initialize(name, config);
			return n;
		default:
			break;
		}
		return null;
	}

    public static ReadNode createReadNode(
	    DistributedCacheSchema.NodeTypeEnum type, String name, String config) {
	return createReadNode(translate(type), name, config);
    }

    public static WriteNode createWriteNode(Node.Type type, String name,
	    String config) {
	switch (type) {
	case ICE:
	    ICEWriteNode b = new ICEWriteNode();
	    b.initialize(name, config);
	    return b;
	default:
	    break;
	}
	return null;
    }

    public static WriteNode createWriteNode(
	    DistributedCacheSchema.NodeTypeEnum type, String name, String config) {
	return createWriteNode(translate(type), name, config);
    }
    
    public static LoadNode createLoadNode(Node.Type type, String name,
    		String config) {
    	switch (type) {
    	case ICE:
    		ICELoadNode b = new ICELoadNode();
    		b.initialize(name, config);
    		return b;
    	default:
    		break;
    	}
    	return null;
    }
    public static LoadNode createLoadNode(
    	    DistributedCacheSchema.NodeTypeEnum type, String name, String config) {
    	return createLoadNode(translate(type), name, config);
    }
}
