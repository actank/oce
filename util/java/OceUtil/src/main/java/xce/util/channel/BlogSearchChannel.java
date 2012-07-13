package xce.util.channel;


public class BlogSearchChannel extends Channel {

	protected BlogSearchChannel() {
		super("BlogSearchChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "BlogSearch");
		properties.setProperty("Ice.Default.Locator",
				"BlogSearch/Locator:tcp -h BlogSearchMaster -p 4090:"
						+ "tcp -h XiaoNeiBlogSearchReplica1 -p 4090:"
						+ "tcp -h XiaoNeiBlogSearchReplica2 -p 4090");

	}
}
