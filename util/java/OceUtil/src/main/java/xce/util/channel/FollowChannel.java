package xce.util.channel;

public class FollowChannel extends Channel {

	protected FollowChannel() {
		super("FollowChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Follow");
		properties.setProperty("Ice.Default.Locator",
				"Follow/Locator:default -h FollowRegistry -p 15300:"
						+ "default -h FollowRegistryReplica1 -p 15301:"
						+ "default -h FollowRegistryReplica2 -p 15302");

	}
}
