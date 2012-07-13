package xce.distcache.admin;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.ZooDefs.Ids;
import org.dom4j.Element;

import xce.distcache.DistributedCacheSchema;
import xce.distcache.DistributedCacheSchema.ClusterInfo;
import xce.distcache.DistributedCacheSchema.NodeCategoryEnum;
import xce.distcache.DistributedCacheSchema.NodeInfo;
import xce.distcache.DistributedCacheSchema.NodeStatus;
import xce.distcache.DistributedCacheSchema.NodeStatusEnum;
import xce.distcache.DistributedCacheSchema.NodeTypeEnum;
import xce.distcache.tools.ZkRMR;

public class NodeConfigData extends ConfigData {
    private String _path;
    private String _name;
    NodeCategoryEnum _category;
    NodeTypeEnum _type;
    NodeStatusEnum _status;
    String _config;
    List<String> _blocks;

    public NodeConfigData(String path) {
	_path = path;
	_blocks = new ArrayList<String>();
    }

    public void Add(ZooKeeper zk) throws KeeperException, InterruptedException {
	NodeInfo.Builder backend_info_builder = NodeInfo.newBuilder();
	backend_info_builder.setCategory(_category);
	backend_info_builder.setType(_type);
	backend_info_builder.setConfig(_config);

	NodeStatus.Builder backend__statusbuilder = NodeStatus.newBuilder();
	backend__statusbuilder.setStatus(_status);

	zk.create(_path, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		CreateMode.PERSISTENT);
	zk.create(_path + "/info", backend_info_builder.build().toByteArray(),
		Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	zk.create(_path + "/status", backend__statusbuilder.build()
		.toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);

	if (_category == NodeCategoryEnum.READ) {
	    zk.create(_path + "/blocks", new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		    CreateMode.PERSISTENT);
	    for (String block : _blocks) {
		zk.create(_path + "/blocks/" + block, new byte[] {},
			Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    }
	}
    }

    public void Remove(ZooKeeper zk) throws InterruptedException,
	    KeeperException {
	ZkRMR.rmr(zk, _path);
    }

    @Override
    public void Parse(Element e) throws ParseConfigException {
	_name = e.attributeValue("name");
	_category = NodeCategoryEnum.valueOf(e.attributeValue("category")
		.toUpperCase());
	_type = NodeTypeEnum.valueOf(e.attributeValue("type").toUpperCase());
	_status = NodeStatusEnum.valueOf(e.attributeValue("status")
		.toUpperCase());
	_config = e.attributeValue("config");
	if (_category == NodeCategoryEnum.READ) {
	    String[] blocks = e.attributeValue("blocks").split("\\|");
	    for (String block : blocks) {
		_blocks.add(block);
	    }
	}
    }

    @Override
    public void Update(ZooKeeper zk) throws InterruptedException,
	    KeeperException {
    }
}
