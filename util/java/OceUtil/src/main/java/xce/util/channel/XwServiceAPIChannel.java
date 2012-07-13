package xce.util.channel;

public class XwServiceAPIChannel extends Channel {
	protected XwServiceAPIChannel(){
		super("XwServiceAPIChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XwServiceAPI");
		properties.setProperty("Ice.Default.Locator",
				"XwServiceAPI/Locator:default -h XwServiceAPIMaster -p 4083"
						+ ":default -h XwServiceAPIReplica1 -p 4083"
						+ ":default -h XwServiceAPIReplica2 -p 4083");
	}
}
