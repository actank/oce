package xce.distcache.tools;

import java.io.IOException;
import java.util.List;
import java.util.Stack;
import java.util.UUID;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.ZooDefs.Ids;

import xce.distcache.DistributedCacheSchema;
import xce.distcache.ZkConnectionFactory;
import xce.distcache.DistributedCacheSchema.NodeCategoryEnum;
import xce.distcache.DistributedCacheSchema.NodeStatusEnum;
import xce.distcache.DistributedCacheSchema.NodeTypeEnum;

public class InitCluster {

    /**
     * @param args
     * @throws InterruptedException
     * @throws KeeperException
     * @throws IOException
     * @throws ParseException
     */
    public static void main(String[] args) throws KeeperException,
	    InterruptedException, IOException, ParseException {

	Options options = new Options();
	options.addOption(OptionBuilder.isRequired().hasArg().create("name"));
	options.addOption(OptionBuilder.isRequired().hasArg().create(
		"block_size"));
	options.addOption(OptionBuilder.isRequired().hasArg()
		.create("ice_meta"));
	options.addOption(OptionBuilder.isRequired().hasArg().create(
		"memcached_read"));
	options.addOption(OptionBuilder.isRequired().hasArg().create(
		"ice_write"));
	options.addOption(OptionBuilder.create("force"));

	CommandLine cmd = new PosixParser().parse(options, args);

	String name = cmd.getOptionValue("name");
	Integer blockSize = Integer.valueOf(cmd.getOptionValue("block_size"));
	String[] ice_meta_config = cmd.getOptionValues("ice_meta");
	String[] memcache_read_config = cmd.getOptionValues("memcached_read");
	String[] ice_write_config = cmd.getOptionValues("ice_write");
	boolean isForce = cmd.hasOption("force");

	ZooKeeper zk = ZkConnectionFactory.create("DistCache", null);
	if (null == zk) {
	    System.err.println("no such connection to zk");
	    System.exit(1);
	}
	if (null != zk.exists("/" + name, false)) {
	    if (!isForce) {
		System.err.println(name + " already exists");
		System.exit(1);
	    } else {
		System.out.println(name + " already exists, remove it");
		Stack<String> targets = new Stack<String>();
		targets.push("/" + name);
		while (!targets.empty()) {
		    String path = targets.peek();
		    List<String> children = zk.getChildren(path, false);
		    if (children.size() != 0) {
			for (String child : children) {
			    targets.push(path + "/" + child);
			}
		    } else {
			targets.pop();
			System.out.println("Removing " + path);
			zk.delete(path, -1);
		    }
		}
	    }
	}

	DistributedCacheSchema.ClusterInfo.Builder cluster_info_builder = DistributedCacheSchema.ClusterInfo
		.newBuilder();
	cluster_info_builder.setBlockSize(blockSize);
	cluster_info_builder.setReplication(3);

	zk.create("/" + name, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		CreateMode.PERSISTENT);
	zk.create("/" + name + "/info", cluster_info_builder.build()
		.toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);

	for (int pos = 0; pos < ice_meta_config.length; ++pos) {
	    System.out.println("Creating ice meta nodes: " + pos + "/"
		    + ice_meta_config.length);
	    /*
	     * prepare /{name}/{node_id}/info
	     */
	    DistributedCacheSchema.NodeInfo.Builder backend_info_builder = DistributedCacheSchema.NodeInfo
		    .newBuilder();
	    backend_info_builder.setCategory(NodeCategoryEnum.META);
	    backend_info_builder.setType(NodeTypeEnum.ICE);
	    backend_info_builder.setConfig(ice_meta_config[pos]);

	    /*
	     * prepare /{name}/{node_id}/status
	     */
	    DistributedCacheSchema.NodeStatus.Builder backend_status_builder = DistributedCacheSchema.NodeStatus
		    .newBuilder();
	    backend_status_builder.setStatus(NodeStatusEnum.UPGRADING);

	    /*
	     * write above two to FS.
	     */
	    UUID id = UUID.randomUUID();
	    System.out.println("Creating META node: " + id.toString());
	    String node_path = "/" + name + "/" + id.toString();
	    zk.create(node_path, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		    CreateMode.PERSISTENT);
	    zk.create(node_path + "/info", backend_info_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    zk.create(node_path + "/status", backend_status_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	}

	for (int pos = 0; pos < memcache_read_config.length; ++pos) {
	    System.out.println("Creating memcached read nodes: " + pos + "/"
		    + memcache_read_config.length);
	    /*
	     * prepare /{name}/{node_id}/info
	     */
	    DistributedCacheSchema.NodeInfo.Builder backend_info_builder = DistributedCacheSchema.NodeInfo
		    .newBuilder();
	    backend_info_builder.setCategory(NodeCategoryEnum.READ);
	    backend_info_builder.setType(NodeTypeEnum.MEMCACHED);
	    backend_info_builder.setConfig(memcache_read_config[pos]);

	    /*
	     * prepare /{name}/{node_id}/status
	     */
	    DistributedCacheSchema.NodeStatus.Builder backend_status_builder = DistributedCacheSchema.NodeStatus
		    .newBuilder();
	    backend_status_builder.setStatus(NodeStatusEnum.ONLINE);

	    /*
	     * write above two to FS.
	     */
	    UUID id = UUID.randomUUID();
	    System.out.println("Creating READ node: " + id.toString());
	    String backend_path = "/" + name + "/" + id.toString();
	    zk.create(backend_path, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		    CreateMode.PERSISTENT);
	    zk.create(backend_path + "/info", backend_info_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    zk.create(backend_path + "/status", backend_status_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);

	    /*
	     * read node should have /{name}/{node_id}/blocks also.
	     */
	    zk.create(backend_path + "/blocks", new byte[] {},
		    Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);

	    System.out.print("Creating READ node blocks: ");
	    for (int i = pos; i < blockSize; i += memcache_read_config.length) {
		System.out.print(".");
		zk.create(backend_path + "/blocks/" + i, new byte[] {},
			Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    }
	    System.out.println();
	}

	for (int pos = 0; pos < ice_write_config.length; ++pos) {
	    System.out.println("Creating ice write nodes: " + pos + "/"
		    + ice_write_config.length);
	    /*
	     * prepare /{name}/{node_id}/info
	     */
	    DistributedCacheSchema.NodeInfo.Builder backend_info_builder = DistributedCacheSchema.NodeInfo
		    .newBuilder();
	    backend_info_builder.setCategory(NodeCategoryEnum.WRITE);
	    backend_info_builder.setType(NodeTypeEnum.ICE);
	    backend_info_builder.setConfig(ice_write_config[pos]);

	    /*
	     * prepare /{name}/{node_id}/status
	     */
	    DistributedCacheSchema.NodeStatus.Builder backend_status_builder = DistributedCacheSchema.NodeStatus
		    .newBuilder();
	    backend_status_builder.setStatus(NodeStatusEnum.ONLINE);

	    /*
	     * write above two to FS.
	     */
	    UUID id = UUID.randomUUID();
	    System.out.println("Creating WRITE node: " + id.toString());
	    String backend_path = "/" + name + "/" + id.toString();
	    zk.create(backend_path, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		    CreateMode.PERSISTENT);
	    zk.create(backend_path + "/info", backend_info_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    zk.create(backend_path + "/status", backend_status_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	}
	zk.close();
	System.out.println("done");
	System.exit(0);
    }
}
