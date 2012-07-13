package xce.distcache.tools;

import java.util.List;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;

import xce.distcache.DistributedCacheSchema;

import com.google.protobuf.InvalidProtocolBufferException;

public class RemoveBackends {

    /**
     * @param args
     */
    @SuppressWarnings("static-access")
    public static void main(String[] args) {
	Options options = new Options();
	options.addOption(OptionBuilder.isRequired().hasArg().create("name"));
	options
		.addOption(OptionBuilder.isRequired().hasArg()
			.create("backend"));

	String name = null;
	String backend = null;
	try {
	    CommandLine cmd = new PosixParser().parse(options, args);
	    name = cmd.getOptionValue("name");
	    backend = cmd.getOptionValue("backend");
	} catch (ParseException e) {
	    e.printStackTrace();
	    System.exit(1);
	}

	String result = setBackend(name, backend,
		DistributedCacheSchema.NodeStatusEnum.OFFLINE);
	System.out.println(result + " synced.");
    }

    private static String setBackend(String name, String backend,
	    DistributedCacheSchema.NodeStatusEnum state) {
	ZooKeeper zk = Tools.connectTo(name);
	if (zk == null) {
	    return null;
	}

	try {
	    String created = Tools.obtainUpgradingLock(zk, name);
	    if (created == null) {
		return null;
	    }
	    DistributedCacheSchema.NodeStatus orignal_status = Tools
		    .readNodeStatus(zk, name, backend);
	    System.out.println("Original status is: " + orignal_status);

	    List<Integer> to_blocks = Tools.getNodeBlocks(zk, name, backend);
	    System.out.println("Original blocks size: " + to_blocks.size());

	    Tools.removeNode(zk, name, backend);
	    return backend;
	} catch (KeeperException e) {
	    e.printStackTrace();
	} catch (InterruptedException e) {
	    e.printStackTrace();
	} catch (InvalidProtocolBufferException e) {
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
