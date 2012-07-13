package xce.distcache;

import java.util.List;
import java.util.Map;
import java.util.Random;

import xce.distcache.node.ReadNode;
import xce.distcache.node.Node.Item;

public class PreferManager {
    private Random _random;

    public PreferManager() {
	_random = new Random(System.currentTimeMillis());
    }

    public ReadNode choose(List<ReadNode> backends) {
	// TOOD complete this
	return backends.get(_random.nextInt(backends.size()));
    }

    public ReadNode choose(List<ReadNode> backends,
	    Map<ReadNode, List<Item>> categoried) {
	// TODO complete this using pref_data
	ReadNode current = backends.get(_random.nextInt(backends.size()));
	for (ReadNode b : backends) {
	    if (categoried.containsKey(b))
		return b;
	}
	return current;
    }
}
