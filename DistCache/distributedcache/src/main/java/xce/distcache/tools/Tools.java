package xce.distcache.tools;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.ZooDefs.Ids;

import xce.distcache.DistributedCacheSchema;
import xce.distcache.ZkConnectionFactory;

import com.google.protobuf.InvalidProtocolBufferException;

public class Tools {
    public static ZooKeeper connectTo(String name) {
	ZooKeeper zk = null;
	try {
	    zk = ZkConnectionFactory.create(null, null);
	} catch (IOException e) {
	    e.printStackTrace();
	}
	if (zk == null) {
	    System.out.println("Failed to connect to zk.");
	    return null;
	}
	return zk;
    }

    public static String obtainUpgradingLock(ZooKeeper zk, String name) {
	String created = null;
	try {
	    if (null == name) {
		created = zk.create("/upgrading_lock", new byte[] {},
			Ids.OPEN_ACL_UNSAFE, CreateMode.EPHEMERAL);
	    } else {
		created = zk.create("/" + name + "/upgrading_lock",
			new byte[] {}, Ids.OPEN_ACL_UNSAFE,
			CreateMode.EPHEMERAL);
	    }
	} catch (KeeperException e) {
	    e.printStackTrace();
	} catch (InterruptedException e) {
	    e.printStackTrace();
	}

	if (created == null) {
	    System.out.println("Failed to obtain upgrading lock.");
	    return null;
	}
	return created;
    }

    public static DistributedCacheSchema.ClusterInfo readClusterInfo(
	    ZooKeeper zk, String name, boolean flag) throws KeeperException,
	    InterruptedException, InvalidProtocolBufferException {
	if(flag&&zk.exists("/" + name + "/info", true)==null){
	    return null;
	}
	byte[] data = zk.getData("/" + name + "/info", flag, null);
	return DistributedCacheSchema.ClusterInfo.parseFrom(data);
    }

    public static DistributedCacheSchema.ClusterInfo readClusterInfo(
	    ZooKeeper zk, String name) throws KeeperException,
	    InterruptedException, InvalidProtocolBufferException {
	byte[] data = zk.getData("/" + name + "/info", false, null);
	return DistributedCacheSchema.ClusterInfo.parseFrom(data);
    }

    public static DistributedCacheSchema.NodeInfo readNodeInfo(ZooKeeper zk,
	    String name, String backend) throws KeeperException,
	    InterruptedException, InvalidProtocolBufferException {
	byte[] data = zk.getData("/" + name + "/" + backend + "/info", false,
		null);
	return DistributedCacheSchema.NodeInfo.parseFrom(data);
    }

    public static boolean writeNodeInfo(ZooKeeper zk, String name,
	    String backend, DistributedCacheSchema.NodeInfo info)
	    throws KeeperException, InterruptedException,
	    InvalidProtocolBufferException {
	byte[] data = info.toByteArray();
	zk.setData("/" + name + "/" + backend + "/info", data, -1);
	return true;
    }

    public static DistributedCacheSchema.NodeStatus readNodeStatus(
	    ZooKeeper zk, String name, String backend) throws KeeperException,
	    InterruptedException, InvalidProtocolBufferException {
	byte[] data = zk.getData("/" + name + "/" + backend + "/status", false,
		null);
	return DistributedCacheSchema.NodeStatus.parseFrom(data);
    }

    public static boolean writeNodeStatus(ZooKeeper zk, String name,
	    String backend, DistributedCacheSchema.NodeStatus status)
	    throws KeeperException, InterruptedException,
	    InvalidProtocolBufferException {
	byte[] data = status.toByteArray();
	zk.setData("/" + name + "/" + backend + "/status", data, -1);
	return true;
    }

    public static boolean addNodeBlocks(ZooKeeper zk, String name,
	    String backend, int block) throws KeeperException,
	    InterruptedException, InvalidProtocolBufferException {
	zk.create("/" + name + "/" + backend + "/blocks/" + block, new byte[0],
		Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	return true;
    }

    public static boolean removeNodeBlocks(ZooKeeper zk, String name,
	    String backend, int block) throws KeeperException,
	    InterruptedException, InvalidProtocolBufferException {
	zk.delete("/" + name + "/" + backend + "/blocks/" + block, -1);
	return true;
    }

    public static List<Integer> getNodeBlocks(ZooKeeper zk, String name,
	    String backend) throws NumberFormatException, KeeperException,
	    InterruptedException {
	List<Integer> ret = new ArrayList<Integer>();
	for (String block : zk.getChildren("/" + name + "/" + backend
		+ "/blocks", false)) {
	    ret.add(Integer.valueOf(block));
	}
	return ret;
    }

    public static boolean removeNode(ZooKeeper zk, String name, String backend)
	    throws KeeperException, InterruptedException {
	zk.delete("/" + name + "/" + backend + "/status", -1);
	for (String block : zk.getChildren("/" + name + "/" + backend
		+ "/blocks", false)) {
	    zk.delete(block, -1);
	}
	zk.delete("/" + name + "/" + backend + "/info", -1);
	zk.delete("/" + name + "/" + backend, -1);
	return true;
    }

}
