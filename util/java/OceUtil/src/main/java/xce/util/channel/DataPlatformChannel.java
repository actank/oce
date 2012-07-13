package xce.util.channel;

public class DataPlatformChannel extends Channel {
	protected DataPlatformChannel(){
		super("DataPlatformChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "DataPlatform");
		properties.setProperty("Ice.Default.Locator",
				"DataPlatform/Locator:default -h DataPlatformMaster -p 4086"
						+ ":default -h DataPlatformReplica1 -p 4086"
						+ ":default -h DataPlatformReplica2 -p 4086");
	}
}
