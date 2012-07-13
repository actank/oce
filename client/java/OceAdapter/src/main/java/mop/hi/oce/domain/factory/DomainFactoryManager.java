package mop.hi.oce.domain.factory;

public class DomainFactoryManager {
	private static DomainFactoryManager instance;

	private NetworkFactory networkFactory;

	public static DomainFactoryManager getInstance() {
		if (instance == null) {
			synchronized (DomainFactoryManager.class) {
				if (instance == null) {
					instance = new DomainFactoryManager();
				}
			}
		}
		return instance;
	}

	public NetworkFactory getNetworkFactory() {
		return networkFactory;
	}

	public void setNetworkFactory(NetworkFactory networkFactory) {
		this.networkFactory = networkFactory;
	}

	private FriendFinderFactory _friendFinderFactory;

	public void setFriendFinderFactory(FriendFinderFactory factory) {
		_friendFinderFactory = factory;
	}

	public FriendFinderFactory getFriendFinderFactory() {
		return _friendFinderFactory;
	}

}
