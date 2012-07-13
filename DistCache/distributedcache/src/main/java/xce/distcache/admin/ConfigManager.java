package xce.distcache.admin;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;

import com.google.protobuf.InvalidProtocolBufferException;

import xce.distcache.DistributedCacheSchema;
import xce.distcache.ZkConnectionFactory;
import xce.distcache.DistributedCacheSchema.ClusterInfo;
import xce.distcache.DistributedCacheSchema.NodeCategoryEnum;
import xce.distcache.DistributedCacheSchema.NodeInfo;
import xce.distcache.DistributedCacheSchema.NodeStatus;
import xce.distcache.tools.Tools;

public class ConfigManager {
    public static ConfigChangedData CheckConfig(String path) {
	List<ConfigData> deletedatas = new ArrayList<ConfigData>();
	List<ConfigData> adddatas = new ArrayList<ConfigData>();
	List<ConfigData> updatedatas = new ArrayList<ConfigData>();
	ZooKeeper zk = null;
	try {
	    zk = Tools.connectTo("");

	    // check cluster
	    List<String> clusters = zk.getChildren("/", false);
	    Set<String> zkclusterset = new HashSet<String>(clusters);
	    zkclusterset.remove("upgrading_lock");
	    File file = new File(path);
	    SAXReader r = new SAXReader();
	    Document d = r.read(file);
	    Element e = d.getRootElement();
	    List<Element> es = e.elements("cluster");
	    Set<String> fileclusterset = new HashSet<String>();
	    for (Element esi : es) {
		if (fileclusterset.contains(esi.attributeValue("name"))) {
		    throw new ParseConfigException("The cluster "
			    + esi.attributeValue("name") + " exists.");
		}
		fileclusterset.add(esi.attributeValue("name"));
	    }

	    for (String zki : zkclusterset) {
		if (!fileclusterset.contains(zki)) {
		    deletedatas.add(new ClusterConfigData("/" + zki));
		}
	    }

	    for (Element ee : es) {
		if (!zkclusterset.contains(ee.attributeValue("name"))) {
		    ClusterConfigData data = new ClusterConfigData("/"
			    + ee.attributeValue("name"));
		    data.Parse(ee);
		    adddatas.add(data);
		}
	    }

	    // check node

	    for (String cluster : clusters) {
		if (cluster.equals("upgrading_lock")) {
		    continue;
		}
		List<String> nodes = zk.getChildren("/" + cluster, false);
		Set<String> zknodeset = new HashSet<String>(nodes);
		zknodeset.remove("info");

		List<Element> nodees = null;
		for (Element ei : es) {
		    if (ei.attributeValue("name").equals(cluster)) {
			nodees = ei.elements();
		    }
		}
		if (nodees == null) {
		    continue;
		}
		Map<String, Element> filenodeset = new HashMap<String, Element>();
		for (Element ei : nodees) {
		    if (filenodeset.containsKey(ei.attributeValue("name"))) {
			throw new ParseConfigException("The cluster " + cluster
				+ " node " + ei.attributeValue("name")
				+ " exists");
		    }
		    filenodeset.put(ei.attributeValue("name"), ei);
		}

		for (String zki : zknodeset) {
		    if (zki.equals("info")) {
			continue;
		    }
		    if (!filenodeset.containsKey(zki)) {
			deletedatas.add(new NodeConfigData("/" + cluster + "/"
				+ zki));
		    }
		}

		for (Element ee : nodees) {
		    if (!zknodeset.contains(ee.attributeValue("name"))) {
			NodeConfigData data = new NodeConfigData("/" + cluster
				+ "/" + ee.attributeValue("name"));
			data.Parse(ee);
			adddatas.add(data);
		    } else {
			Map<String, String> data = CheckData(zk, "/" + cluster
				+ "/" + ee.attributeValue("name"), ee);
			if (data.size() > 0) {
			    updatedatas.add(new DataConfigData("/" + cluster
				    + "/" + ee.attributeValue("name"), data));
			}
		    }
		}
	    }
	} catch (ParseConfigException e) {
	    System.err.println(e);
	} catch (Exception e) {
	    e.printStackTrace();
	}
	return new ConfigChangedData(deletedatas, adddatas, updatedatas);
    }

    private static Map<String, String> CheckData(ZooKeeper zk, String path,
	    Element e) throws KeeperException, InterruptedException,
	    InvalidProtocolBufferException {
	Map<String, String> res = new HashMap<String, String>();
	boolean flag = false;
	byte[] node_info_data = zk.getData(path + "/info", false, null);
	DistributedCacheSchema.NodeInfo node_info = DistributedCacheSchema.NodeInfo
		.parseFrom(node_info_data);
	if (node_info.getType() != DistributedCacheSchema.NodeTypeEnum
		.valueOf(e.attributeValue("type"))) {
	    flag = true;
	}
	if (node_info.getCategory() != DistributedCacheSchema.NodeCategoryEnum
		.valueOf(e.attributeValue("category"))) {
	    flag = true;
	}
	if (!node_info.getConfig().equals(e.attributeValue("config"))) {
	    flag = true;
	}
	if (flag) {
	    res.put("type", e.attributeValue("type"));
	    res.put("category", e.attributeValue("category"));
	    res.put("config", e.attributeValue("config"));
	}
	if (e.attributeValue("category").equals("READ")) {
	    List<String> children = zk.getChildren(path + "/blocks", false);
	    Set<String> zkset = new HashSet<String>(children);
	    String[] blocks = e.attributeValue("blocks").split("\\|");
	    Set<String> fileset = new HashSet<String>();
	    for (String block : blocks) {
		if (!zkset.contains(block)) {
		    res.put("blocks", e.attributeValue("blocks"));
		    return res;
		}
		fileset.add(block);
	    }
	    for (String block : zkset) {
		if (!fileset.contains(block)) {
		    res.put("blocks", e.attributeValue("blocks"));
		    return res;
		}
	    }
	}
	return res;
    }

    public static void UpdateConfig(ConfigChangedData data) {
	ZooKeeper zk = Tools.connectTo("");
	for (ConfigData i : data.getDeletedatas()) {
	    try {
		i.Remove(zk);
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    } catch (KeeperException e) {
		e.printStackTrace();
	    }
	}
	for (ConfigData i : data.getAdddatas()) {
	    try {
		i.Add(zk);
	    } catch (KeeperException e) {
		e.printStackTrace();
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
	for (ConfigData i : data.getUpdatedatas()) {
	    try {
		i.Update(zk);
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    } catch (KeeperException e) {
		e.printStackTrace();
	    }
	}
    }

    public static String ProduceConfig() {
	StringBuffer sb = new StringBuffer();
	try {
	    ZooKeeper zk = Tools.connectTo("");
	    sb.append("<distributedcache>\n");
	    List<String> clusters = zk.getChildren("/", false);
	    for (String cluster : clusters) {
		if (cluster.equals("upgrading_lock")) {
		    continue;
		}
		String clusterpath = "/" + cluster;
		byte[] clusterinfodata = zk.getData(clusterpath + "/info",
			false, null);
		ClusterInfo clusterinfo = ClusterInfo
			.parseFrom(clusterinfodata);
		int clusterblocksize = clusterinfo.getBlockSize();
		sb.append("\t<cluster name=\"").append(cluster).append(
			"\" blocksize=\"").append(clusterblocksize).append(
			"\">\n");
		List<String> nodes = zk.getChildren("/" + cluster, false);
		for (String node : nodes) {
		    if (node.equals("info")) {
			continue;
		    }
		    byte[] nodestatusdata = zk.getData(clusterpath + "/" + node
			    + "/status", false, null);
		    NodeStatus nodestatus = NodeStatus
			    .parseFrom(nodestatusdata);
		    byte[] nodeinfodata = zk.getData(clusterpath + "/" + node
			    + "/info", false, null);
		    NodeInfo nodeinfo = NodeInfo.parseFrom(nodeinfodata);
		    if (nodeinfo.getCategory() == NodeCategoryEnum.READ) {
			sb.append("\t\t<node name=\"").append(node).append(
				"\" type=\"").append(nodeinfo.getType())
				.append("\" category=\"").append(
					nodeinfo.getCategory()).append(
					"\" config=\"").append(
					nodeinfo.getConfig()).append(
					"\" status=\"").append(
					nodestatus.getStatus()).append(
					"\" blocks=\"");
			List<String> blocks = zk.getChildren(clusterpath + "/"
				+ node + "/blocks", false);
			for (int i = 0; i < blocks.size(); ++i) {
			    if (i != 0) {
				sb.append("|");
			    }
			    sb.append(blocks.get(i));
			}
			sb.append("\"/>\n");
		    } else if (nodeinfo.getCategory() == NodeCategoryEnum.WRITE) {
			sb.append("\t\t<node name=\"").append(node).append(
				"\" type=\"").append(nodeinfo.getType())
				.append("\" category=\"").append(
					nodeinfo.getCategory()).append(
					"\" config=\"").append(
					nodeinfo.getConfig()).append(
					"\" status=\"").append(
					nodestatus.getStatus())
				.append("\"/>\n");
		    } else if (nodeinfo.getCategory() == NodeCategoryEnum.META) {
			sb.append("\t\t<node name=\"").append(node).append(
				"\" type=\"").append(nodeinfo.getType())
				.append("\" category=\"").append(
					nodeinfo.getCategory()).append(
					"\" config=\"").append(
					nodeinfo.getConfig()).append(
					"\" status=\"").append(
					nodestatus.getStatus())
				.append("\"/>\n");
		    }
		}
		sb.append("\t</cluster>\n");
	    }
	    sb.append("</distributedcache>\n");
	} catch (Exception e) {
	    e.printStackTrace();
	}
	return sb.toString();
    }

    public static void main(String[] args) {
	if (args.length < 1) {
	    System.err.println("less args");
	}
	if (args[0].toLowerCase().equals("config")) {
	    System.out.println(ProduceConfig());
	} else if (args[0].toLowerCase().equals("update")) {
	    try {
		ZooKeeper zk = ZkConnectionFactory.create(null, null);
		String created = Tools.obtainUpgradingLock(zk, null);
		if (null == created) {
		    System.out.println("Can't obtain UpgradingLock");
		} else {
		    ConfigChangedData data = CheckConfig(args[1]);
		    UpdateConfig(data);
		}
	    } catch (Exception e) {
		System.out.println("Can't obtain UpgradingLock for " + e);
	    }
	} else {
	    System.err.println("unknown order");
	}
    }
}
