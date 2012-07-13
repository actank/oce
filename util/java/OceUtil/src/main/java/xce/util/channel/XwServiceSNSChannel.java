package xce.util.channel;

public class XwServiceSNSChannel extends Channel {
	protected XwServiceSNSChannel(){
		super("XwServiceSNSChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XwServiceSNS");
		properties.setProperty("Ice.Default.Locator",
				"XwServiceSNS/Locator:default -h XwServiceSNSMaster -p 4081"
						+ ":default -h XwServiceSNSReplica1 -p 4081"
						+ ":default -h XwServiceSNSReplica2 -p 4081");
	}
}