package xce.distcache;

import java.util.List;

import xce.distcache.DistributedCacheSchema.NodeInfo;
import xce.distcache.DistributedCacheSchema.NodeStatus;

public class ClusterNode {
    private String _name;
    private NodeInfo _info;
    private NodeStatus _status;
    private List<Integer> _blocks;

    public ClusterNode(String name, NodeInfo info, NodeStatus status,
	    List<Integer> blocks) {
	_name = name;
	_info = info;
	_status = status;
	_blocks = blocks;
    }

    public String getName() {
	return _name;
    }

    public NodeInfo getNodeInfo() {
	return _info;
    }

    public NodeStatus getNodeStatus() {
	return _status;
    }

    public List<Integer> getBlocks() {
	return _blocks;
    }
}