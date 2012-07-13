package xce.util.channel;


public class SearchCacheChannel extends Channel {

	protected SearchCacheChannel() {
		super("SearchCacheChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "XiaoNeiSearchCache");
		properties.setProperty("Ice.Default.Locator",
				"XiaoNeiSearchCache/Locator:tcp -h XiaoNeiSearchCacheMaster -p 4061:"
						+ "tcp -h XiaoNeiSearchCacheReplica1 -p 4061:"
						+ "tcp -h XiaoNeiSearchCacheReplica2 -p 4061");

	}
}
