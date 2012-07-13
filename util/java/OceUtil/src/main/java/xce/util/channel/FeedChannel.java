package xce.util.channel;

public class FeedChannel extends Channel {

	protected FeedChannel() {
		super("FeedChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XceFeed");
		properties.setProperty("Ice.Default.Locator",
				"XceFeed/Locator:default -h XceFeedRegistry -p 14300:"
						+ "default -h XceFeedRegistryReplica1 -p 14301:"
						+ "default -h XceFeedRegistryReplica2 -p 14302");

	}
}
