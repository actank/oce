package xce.util.channel;


public class BuddyChannel extends Channel {

	protected BuddyChannel() {
		super("BuddyChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Buddy");
		properties.setProperty("Ice.Default.Locator",
				"Buddy/Locator:default -h BuddyMaster -p 4077"
						+ ":default -h BuddyReplica1 -p 4077"
						+ ":default -h BuddyReplica2 -p 4077");		
	}
}
