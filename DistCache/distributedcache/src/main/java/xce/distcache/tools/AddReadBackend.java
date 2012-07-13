package xce.distcache.tools;

import java.util.List;
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
import xce.distcache.DistributedCacheSchema.NodeCategoryEnum;
import xce.distcache.DistributedCacheSchema.NodeStatusEnum;
import xce.distcache.DistributedCacheSchema.NodeTypeEnum;

public class AddReadBackend {

    /**
     * @param args
     */
    @SuppressWarnings("static-access")
    public static void main(String[] args) {
	Options options = new Options();
	options.addOption(OptionBuilder.isRequired().hasArg().create("name"));
	options.addOption(OptionBuilder.isRequired().hasArg().create(
		"memcached"));
	options.addOption(OptionBuilder.isRequired().hasArg().create("blocks"));
	String name = null;
	String param = null;
	String blocks = null;
	try {
	    CommandLine cmd = new PosixParser().parse(options, args);
	    name = cmd.getOptionValue("name");
	    param = cmd.getOptionValue("memcached");
	    blocks = cmd.getOptionValue("blocks");
	    System.out.println();
	} catch (ParseException e) {
	    e.printStackTrace();
	    System.exit(1);
	}

	String result = add(name, param, blocks.split("\\|"));
	System.out.println(result + " added.");
    }

    private static String add(String name, String memcache_config, String[] ids) {
	ZooKeeper zk = Tools.connectTo(name);
	if (zk == null) {
	    return null;
	}
	try {
	    String created = Tools.obtainUpgradingLock(zk, name);
	    if (created == null) {
		return null;
	    }

	    DistributedCacheSchema.NodeInfo.Builder backend_info_builder = DistributedCacheSchema.NodeInfo
		    .newBuilder();
	    backend_info_builder.setType(NodeTypeEnum.MEMCACHED);
	    backend_info_builder.setConfig(memcache_config);
	    backend_info_builder.setCategory(NodeCategoryEnum.READ);

	    DistributedCacheSchema.NodeStatus.Builder backend_status_builder = DistributedCacheSchema.NodeStatus
		    .newBuilder();
	    backend_status_builder.setStatus(NodeStatusEnum.UPGRADING);

	    UUID id = UUID.randomUUID();

	    String backend_path = "/" + name + "/" + id.toString();
	    zk.create(backend_path, new byte[] {}, Ids.OPEN_ACL_UNSAFE,
		    CreateMode.PERSISTENT);
	    zk.create(backend_path + "/info", backend_info_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    zk.create(backend_path + "/status", backend_status_builder.build()
		    .toByteArray(), Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    zk.create(backend_path + "/blocks", new byte[] {},
		    Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    for (String i : ids) {
		zk.create(backend_path + "/blocks/" + i, new byte[] {},
			Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
	    }
	    return id.toString();
	} catch (KeeperException e) {
	    e.printStackTrace();
	} catch (InterruptedException e) {
	    e.printStackTrace();
	} finally {
	    try {
		zk.close();
	    } catch (Throwable e) {
		e.printStackTrace();
	    }
	}
	return null;
    }

}
