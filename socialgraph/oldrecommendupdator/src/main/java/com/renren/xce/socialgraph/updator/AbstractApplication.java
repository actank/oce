package com.renren.xce.socialgraph.updator;

/**
 * 抽象类 <br/>
 * Template Pattern
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public abstract class AbstractApplication implements Application {

    public void initialize() {
    }

    public void start() {
	initialize();
	run();
	shutdown();
    }

    public void shutdown() {
	System.exit(0);
    }

    /**
     * 实现类的具体内容
     */
    public abstract void run();

    private static final String ROOT_DIR = "/data/xce/SGProject";

    public String getRootDir() {
	return ROOT_DIR;
    }

    private static final String CONF_DIR_NAME = "conf";

    public String getConfigurationDir() {
	return ROOT_DIR + "/" + CONF_DIR_NAME;
    }

    private static final String DATA_DIR_NAME = "data";

    public String getDataDir() {
	return ROOT_DIR + "/" + DATA_DIR_NAME;
    }
}
