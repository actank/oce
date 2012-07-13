package com.renren.messageconfig;

/**
 * 异常
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class MessageConfigException extends Exception {

    private static final long serialVersionUID = 1L;

    public MessageConfigException(Throwable t) {
	super(t);
    }

    public MessageConfigException(String msg) {
	super(msg);
    }

    public MessageConfigException(String msg, Throwable t) {
	super(msg, t);
    }

}
