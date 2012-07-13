package xce.distcache.preloader;

import java.util.List;
import java.util.Map;

public interface PreloaderFactory {
    public enum LoadMod {
	DB, NOTE;
    }

    public Map<Long, byte[]> CreateFromDB(ProducerData data);

    public Map<Long, byte[]> CreateFromNode(List<Long> ids);

}