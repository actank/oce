package com.renren.xce.socialgraph.updator;

/**
 * 所有Updator的实现，都须要实现此接口
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public interface Application {

    /**
     * 程序入口
     */
    public void start();

    /**
     * 
     * 初始化环境变量，LOG等
     */
    public void initialize();

}
