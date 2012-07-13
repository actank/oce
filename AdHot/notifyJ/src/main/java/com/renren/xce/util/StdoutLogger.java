package com.renren.xce.util;

public class StdoutLogger implements Ice.Logger {
	   public void print(String message) {
	        System.out.println("PRINT: " + message);
	    }

	    public void trace(String category, String message) {
	        System.out.println("TRACE(" + category + "): " + message);
	    }

	    public void warning(String message) {
	        System.out.println("WARNING: " + message);
	    }

	    public void error(String message) {
	        System.out.println("ERROR: " + message);
	    }

	    public Ice.Logger cloneWithPrefix(String prefix) {
	        return new StdoutLogger();
	    }

}
