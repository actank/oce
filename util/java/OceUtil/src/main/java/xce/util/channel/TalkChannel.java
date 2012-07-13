package xce.util.channel;


public class TalkChannel extends Channel {

	protected TalkChannel() {
		super("TalkChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XNTalk");

		properties.setProperty("Ice.Default.Locator",
				"XNTalk/Locator:default -h XNTalkRegistry -p 12000");

	}
}
