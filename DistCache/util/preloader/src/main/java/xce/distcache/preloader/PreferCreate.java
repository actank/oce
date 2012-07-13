package xce.distcache.preloader;

import java.util.List;
import java.util.Map;

public class PreferCreate {

    private static PreferCreate _instance = new PreferCreate();

    private PreloaderFactory _factory;

    public static PreferCreate getInstance() {
	return _instance;
    }

    public void setName(String cacheName) {
	PreloaderLogger.info("PreferCreate.setName name: " + cacheName);
//	_factory = ParseXml.getInstance().getFactory(cacheName);
	_factory = PreloaderFactoryFactory.create(cacheName);
    }

    public Map<Long, byte[]> create(ProducerData data) {
	// PreloaderLogger.debug("PreferCreate, ids.size(): " + ids.size());
	if (_factory == null) {
	    PreloaderLogger.error("factory is null");
	    return null;
	}
	return _factory.CreateFromDB(data);
    }
}
