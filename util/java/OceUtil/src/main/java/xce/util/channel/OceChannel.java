package xce.util.channel;


public class OceChannel extends Channel {

	protected OceChannel() {
		super("OceChannel");
	}

	public static void main(String[] args) throws Exception {
		Channel ch = new OceChannel();
		Ice.ObjectPrx prx = ch.locate("UM", "@UserN", Channel.Invoke.Twoway,
				Ice.ObjectPrxHelperBase.class, 300);
		prx.ice_ping();
		System.exit(0);
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XiaoNei");
		properties.setProperty("Ice.Default.Locator",
				"XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000"
						+ ":default -h XiaoNeiRegistryReplica1 -p 12001"
						+ ":default -h XiaoNeiRegistryReplica2 -p 12002");		
	}
}
