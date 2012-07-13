package xce.util.channel;

public class LogChannel extends Channel {

	protected LogChannel() {
		super("LogChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XceLog");
		properties.setProperty("Ice.Default.Locator",
				"XceLog/Locator:default -h LogMaster -p 4062"
						+ ":default -h LogReplica1 -p 4062"
						+ ":default -h LogReplica2 -p 4062");
	}
}
