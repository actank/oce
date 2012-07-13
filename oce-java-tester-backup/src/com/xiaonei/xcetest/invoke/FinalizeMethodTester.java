package com.xiaonei.xcetest.invoke;

public class FinalizeMethodTester {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		new FinalizeMethodTester();
		System.gc();
	}

	public FinalizeMethodTester() {
		System.out.println("Constructed");
	}

	public void finalize() {
		System.out.println("Destroyed");
	}
}
