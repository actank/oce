package xce.util.channel;

public class LikeChannel extends Channel {

	protected LikeChannel() {
		super("LikeChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Like");
		properties.setProperty("Ice.Default.Locator",
				"Like/Locator:default -h LikeMaster -p 4071:"
						+ "default -h LikeReplica1 -p 4071:"
						+ "default -h LikeReplica2 -p 4071");
	}
}