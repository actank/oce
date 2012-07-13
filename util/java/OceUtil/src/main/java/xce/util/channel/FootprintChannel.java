package xce.util.channel;

public class FootprintChannel extends Channel {

	protected FootprintChannel() {
		super("FootprintChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "FootprintChannel");
		properties.setProperty("Ice.Default.Locator",
				"Footprint/Locator:default -h FootprintMaster -p 4085:"
						+ "default -h FootprintReplica1 -p 4085:"
						+ "default -h FootprintReplica2 -p 4085");
	}
}
