package com.xiaonei.service.util.descriptor;

import java.util.Map;

import Ice.Current;
import MyUtil.SvcInstance;
import MyUtil.SvcObserver;
import MyUtil.SvcObserverPrx;

public abstract class SvcDescriptorListener extends SvcObserver {

	private SvcInstance svci;
	private SvcObserverPrx xceo;

	public boolean update(SvcInstance svci, Current __current) {
		if (changed(svci)) {
			setSvcInstance(svci);
			if (__current != null && __current.ctx != null
					&& svci.properties != null) {
				svci.properties.putAll(__current.ctx);
			}
			return update(svci.properties);
		}
		return false;
	}

	protected abstract boolean update(Map<String, String> properties);

	private boolean changed(SvcInstance x) {
		return this.svci == null
				|| (x != null && this.svci.timestamp < x.timestamp);
	}

	public SvcInstance getSvcInstance() {
		return svci;
	}

	public void setSvcInstance(SvcInstance svci) {
		this.svci = svci;
	}

	public SvcObserverPrx getSvcObserver() {
		return xceo;
	}

	public void setSvcObserver(SvcObserverPrx xceo) {
		this.xceo = xceo;
	}
}
