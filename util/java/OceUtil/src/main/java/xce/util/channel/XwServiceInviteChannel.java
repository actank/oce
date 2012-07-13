package xce.util.channel;

public class XwServiceInviteChannel extends Channel {
	protected XwServiceInviteChannel(){
		super("XwServiceInviteChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XwServiceInvite");
		properties.setProperty("Ice.Default.Locator",
				"XwServiceInvite/Locator:default -h XwServiceInviteMaster -p 4087"
						+ ":default -h XwServiceInviteReplica1 -p 4087"
						+ ":default -h XwServiceInviteReplica2 -p 4087");
	}
}
