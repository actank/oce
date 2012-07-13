package xce.util.channel;


public class UserChannel extends Channel {

	protected UserChannel() {
		super("UserChannel");
	}

	public static void main(String[] args) throws Exception {
		Channel ch = new UserChannel();
		Ice.ObjectPrx prx = ch.locate("UBVM", "@UserBase", Channel.Invoke.Twoway,
				Ice.ObjectPrxHelperBase.class, 300);
		prx.ice_ping();
		System.exit(0);
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "User");
		properties.setProperty("Ice.Default.Locator",
				"User/Locator:default -h UserMaster -p 4075"
						+ ":default -h UserReplica1 -p 4075"
						+ ":default -h UserReplica2 -p 4075");		
	}
}
