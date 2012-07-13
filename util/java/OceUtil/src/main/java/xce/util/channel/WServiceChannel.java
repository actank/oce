package xce.util.channel;


public class WServiceChannel extends Channel {

	protected WServiceChannel() {
		super("WServiceChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "wService");
		properties.setProperty("Ice.Default.Locator",
				"wService/Locator:default -h wServiceMaster -p 4064:"
						+ "default -h wServiceReplica1 -p 4064:"
						+ "default -h wServiceReplica2 -p 4064");
	}
}
