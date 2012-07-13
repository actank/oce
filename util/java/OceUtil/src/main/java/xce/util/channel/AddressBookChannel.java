package xce.util.channel;


public class AddressBookChannel extends Channel {

	protected AddressBookChannel() {
		super("AddressBookChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "AddressBook");
		properties.setProperty("Ice.Default.Locator",
				"AddressBook/Locator:tcp -h AddressBookMaster -p 4089:"
						+ "tcp -h AddressBookReplica1 -p 4089:"
						+ "tcp -h AddressBookReplica2 -p 4089");

	}
}
