package xce.util.channel;


public class ShareSearchChannel extends Channel {

	protected ShareSearchChannel() {
		super("ShareSearchChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "ShareSearch");
		properties.setProperty("Ice.Default.Locator",
				"ShareSearch/Locator:tcp -h ShareSearchMaster -p 4090:"
						+ "tcp -h ShareSearchReplica1 -p 4090:"
						+ "tcp -h ShareSearchReplica2 -p 4090");

	}
}
