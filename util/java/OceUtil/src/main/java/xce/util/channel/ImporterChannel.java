package xce.util.channel;

public class ImporterChannel extends Channel {
	protected ImporterChannel(){
		super("ImporterChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Importer");
		properties.setProperty("Ice.Default.Locator",
				"Importer/Locator:default -h ImporterMaster -p 4080"
						+ ":default -h ImporterReplica1 -p 4080"
						+ ":default -h ImporterReplica2 -p 4080");
	}
}