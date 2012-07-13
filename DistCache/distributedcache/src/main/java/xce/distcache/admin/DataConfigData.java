package xce.distcache.admin;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.ZooDefs.Ids;
import org.dom4j.Element;

import xce.distcache.DistributedCacheSchema.NodeCategoryEnum;
import xce.distcache.DistributedCacheSchema.NodeInfo;
import xce.distcache.DistributedCacheSchema.NodeTypeEnum;

public class DataConfigData extends ConfigData {
    private String _path;
    private Map<String, String> _data;

    public DataConfigData(String path, Map<String, String> data) {
	_path = path;
	_data = data;
    }

    @Override
    public void Add(ZooKeeper zk) throws KeeperException, InterruptedException {
    }

    @Override
    public void Parse(Element e) throws ParseConfigException {
    }

    @Override
    public void Remove(ZooKeeper zk) throws InterruptedException,
	    KeeperException {
    }

    @Override
    public void Update(ZooKeeper zk) throws InterruptedException,
	    KeeperException {
	if (_data.containsKey("type") || _data.containsKey("category")
		|| _data.containsKey("config")) {
	    NodeInfo.Builder backend_info_builder = NodeInfo.newBuilder();
	    backend_info_builder.setCategory(NodeCategoryEnum.valueOf(_data
		    .get("category")));
	    backend_info_builder.setType(NodeTypeEnum
		    .valueOf(_data.get("type")));
	    backend_info_builder.setConfig(_data.get("config"));
	    zk.setData(_path + "/info", backend_info_builder.build()
		    .toByteArray(), -1);
	}

	if (_data.containsKey("blocks")) {
	    List<String> add = new ArrayList<String>();
	    List<String> remove = new ArrayList<String>();
	    List<String> children = zk.getChildren(_path + "/blocks", false);
	    Set<String> zkset = new HashSet<String>(children);
	    String[] blocks = _data.get("blocks").split("\\|");
	    Set<String> fileset = new HashSet<String>();
	    for (String block : blocks) {
		if (!zkset.contains(block)) {
		    add.add(block);
		}
		fileset.add(block);
	    }
	    for (String block : zkset) {
		if (!fileset.contains(block)) {
		    remove.add(block);
		}
	    }
	    for (String block : add) {
		zk.create(_path + "/blocks/" + block, new byte[] {},
			Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    }
	    for (String block : remove) {
		zk.delete(_path + "/blocks/" + block, -1);
	    }
	}
    }
}
