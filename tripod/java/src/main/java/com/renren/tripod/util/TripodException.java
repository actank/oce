package com.renren.tripod.util;

public class TripodException extends Exception {

    /**
     * 
     */
    private static final long serialVersionUID = -1882293841974978006L;

    /**
     * 
     */
    public TripodException() {
        super();
    }

    /**
     * @param msg
     */
    public TripodException(final String msg) {
        super(msg);
    }

    /**
     * @param msg
     * @param cause
     */
    public TripodException(final String msg, final Throwable cause) {
        super(msg, cause);
    }

    /**
     * @param cause
     */
    public TripodException(final Throwable cause) {
        super(cause);
    }

}
