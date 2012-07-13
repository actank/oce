package mop.hi.oce.domain;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public class Updatable implements Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1320809976628799866L;
	private Map<String, String> updates;

	public Map<String, String> getUpdates() {
		if (updates == null) {
			updates = new HashMap<String, String>();
		}
		return updates;
	}

	public Map<String, String> swapUpdates() {
		Map<String, String> result = updates;
		updates = null;
		return result;
	}
	
	public void clearUpdates() {
		if (updates != null) {
			updates.clear();
			updates = null;
		}
	}	
}
