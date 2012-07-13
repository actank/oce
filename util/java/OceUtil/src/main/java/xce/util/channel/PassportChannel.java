package xce.util.channel;

public class PassportChannel extends Channel {

	protected PassportChannel() {
		super("PassportChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Passport");
		properties.setProperty("Ice.Default.Locator",
				"Passport/Locator:default -h PassportMaster -p 4076:"
						+ "default -h PassportReplica1 -p 4076:"
						+ "default -h PassportReplica2 -p 4076");
	}
}
