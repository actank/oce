package xce.distcache.admin;

import java.util.ArrayList;
import java.util.List;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.ZooDefs.Ids;
import org.dom4j.Element;

import xce.distcache.DistributedCacheSchema.ClusterInfo;
import xce.distcache.tools.ZkRMR;

public class ClusterConfigData extends ConfigData {
    private String _path;
    private String _name;
    private int _blocksize;
    private List<NodeConfigData> _nodes;

    public ClusterConfigData(String path) {
	_path = path;
	_name = "";
	_blocksize = 0;
	_nodes = new ArrayList<NodeConfigData>();
    }

    public void Parse(Element e) throws ParseConfigException {
	_name = e.attributeValue("name");
	try {
	    _blocksize = Integer.parseInt(e.attributeValue("blocksize"));
	} catch (NumberFormatException e1) {
	    throw new ParseConfigException(
		    "cluster blocksize has wrong format ");
	}

	List<Element> nodes = e.elements("node");
	for (Element node : nodes) {
	    NodeConfigData nodedata = new NodeConfigData(_path + "/"
		    + node.attributeValue("name"));
	    nodedata.Parse(node);
	    _nodes.add(nodedata);
	}
    }

    public void Add(ZooKeeper zk) throws KeeperException, InterruptedException {
	zk.create(_path, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		CreateMode.PERSISTENT);
	ClusterInfo.Builder cluster_info_builder = ClusterInfo.newBuilder();
	cluster_info_builder.setBlockSize(_blocksize);
	cluster_info_builder.setReplication(3);
	zk.create(_path + "/info", cluster_info_builder.build().toByteArray(),
		Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	for (NodeConfigData node : _nodes) {
	    node.Add(zk);
	}
    }

    public void Remove(ZooKeeper zk) throws InterruptedException,
	    KeeperException {
	ZkRMR.rmr(zk, _path);
    }

    @Override
    public void Update(ZooKeeper zk) throws InterruptedException,
	    KeeperException {
	// TODO Auto-generated method stub

    }
}
