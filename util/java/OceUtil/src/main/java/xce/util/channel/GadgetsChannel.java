package xce.util.channel;

public class GadgetsChannel extends Channel {
	protected GadgetsChannel(){
		super("GadgetsChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Gadgets");
		properties.setProperty("Ice.Default.Locator",
				"Gadgets/Locator:tcp -h GadgetsMaster -p 4079:" +
				"tcp -h GadgetsReplica1 -p 4079:" +
				"tcp -h GadgetsReplica2 -p 4079");
	}
}