package xce.util.channel;

public class CoreChannel extends Channel {

	protected CoreChannel() {
		super("CoreChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "CoreCache");
		properties.setProperty("Ice.Default.Locator",
				"CoreCache/Locator:default -h CoreCacheMaster -p 4068:"
						+ "default -h CoreCacheReplica1 -p 4068:"
						+ "default -h CoreCacheReplica2 -p 4068");
	}
}
