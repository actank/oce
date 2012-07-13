package xce.util.channel;

public class XwServiceUGCChannel extends Channel {
	protected XwServiceUGCChannel(){
		super("XwServiceUGCChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XwServiceUGC");
		properties.setProperty("Ice.Default.Locator",
				"XwServiceUGC/Locator:default -h XwServiceUGCMaster -p 4082"
						+ ":default -h XwServiceUGCReplica1 -p 4082"
						+ ":default -h XwServiceUGCReplica2 -p 4082");
	}
}