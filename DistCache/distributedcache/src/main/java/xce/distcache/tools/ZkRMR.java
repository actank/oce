package xce.distcache.tools;

import java.util.List;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;

import xce.distcache.ZkConnectionFactory;

public class ZkRMR {

    public static void main(String[] args) throws Throwable {
	Options options = new Options();
	options.addOption(OptionBuilder.isRequired().hasArg().create("name"));
	options.addOption(OptionBuilder.isRequired().hasArg().create("paths"));
	CommandLine cmd = new PosixParser().parse(options, args);

	String name = cmd.getOptionValue("name");
	String[] paths = cmd.getOptionValue("paths").split("\\|");

	ZooKeeper zk = ZkConnectionFactory.create("DistCache", null);
	if (null == zk) {
	    System.err.println("no such connection to zk");
	    System.exit(1);
	}
	for (String path : paths) {
	    rmr(zk, path);
	}

	System.exit(0);
    }

    public static void rmr(ZooKeeper zk, String path) throws KeeperException,
	    InterruptedException {
	List<String> children = zk.getChildren(path, false);
	if (children.isEmpty()) {
	    zk.delete(path, -1);
	} else {
	    for (String child : children) {
		rmr(zk, path + "/" + child);
	    }
	    zk.delete(path, -1);
	}
    }
}
