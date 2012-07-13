package xce.distcache;

import java.util.List;

import xce.distcache.DistributedCacheSchema.ClusterInfo;

public class ClusterContent {
    private String _name;
    private ClusterInfo _ci;
    private List<ClusterNode> _nodes;

    public ClusterContent(String name, ClusterInfo ci, List<ClusterNode> nodes) {
	_name = name;
	_ci = ci;
	_nodes = nodes;
    }

    public ClusterInfo getClusterInfo() {
	return _ci;
    }

    public List<ClusterNode> getNodes() {
	return _nodes;
    }
}
