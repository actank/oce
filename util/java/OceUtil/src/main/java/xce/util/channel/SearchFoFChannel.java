package xce.util.channel;


public class SearchFoFChannel extends Channel {

	protected SearchFoFChannel() {
		super("SearchFoFChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XiaoNeiSearchFoF");
		properties.setProperty("Ice.Default.Locator",
				"XiaoNeiSearchFoF/Locator:tcp -h XiaoNeiSearchFoFMaster -p 4061:"
						+ "tcp -h XiaoNeiSearchFoFReplica1 -p 4061:"
						+ "tcp -h XiaoNeiSearchFoFReplica2 -p 4061");

	}
}
