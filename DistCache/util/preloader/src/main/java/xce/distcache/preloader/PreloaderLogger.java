package xce.distcache.preloader;

import xce.distcache.server.PreloaderServerI;

public class PreloaderLogger {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(PreloaderServerI.class);

    public static void debug(Object message) {
	_logger.debug(message);
    }

    public static void info(Object message) {
	_logger.info(message);
    }

    public static void warn(Object message) {
	_logger.warn(message);
    }

    public static void error(Object message) {
	_logger.error(message);
    }
}
