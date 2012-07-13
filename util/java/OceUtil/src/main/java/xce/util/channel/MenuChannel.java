package xce.util.channel;

public class MenuChannel extends Channel {

	protected MenuChannel() {
		super("MenuChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "MenuChannel");
		properties.setProperty("Ice.Default.Locator",
				"Menu/Locator:default -h MenuMaster -p 4078:"
						+ "default -h MenuReplica1 -p 4078:"
						+ "default -h MenuReplica2 -p 4078");
	}
}