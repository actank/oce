package xce.util.channel;

public class ActiveTrackChannel extends Channel {
	protected ActiveTrackChannel() {
		super("ActiveTrackChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "ActiveTrack");
		properties.setProperty("Ice.Default.Locator",
				"ActiveTrack/Locator:default -h ActiveTrackMaster -p 4065"
						+ ":default -h ActiveTrackReplica1 -p 4065"
						+ ":default -h ActiveTrackReplica2 -p 4065");

	}
}
