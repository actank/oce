package xce.util.channel;

public class SocialGraphChannel extends Channel {
	protected SocialGraphChannel(){
		super("SocialGraphChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "SocialGraph");
		properties.setProperty("Ice.Default.Locator",
				"SocialGraph/Locator:default -h SocialGraphMaster -p 4072"
						+ ":default -h SocialGraphReplica1 -p 4072"
						+ ":default -h SocialGraphReplica2 -p 4072");
	}
}
