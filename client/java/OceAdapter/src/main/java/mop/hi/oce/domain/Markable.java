package mop.hi.oce.domain;

import java.util.HashMap;
import java.util.Map;

public class Markable {
	private Map<String, String> marks;

	public void mark(String key, String value) {
		getMarks().put(key, value);
	}

	public Map<String, String> demark() {
		Map<String, String> result = marks;
		marks = null;
		return result;
	}
	
	public boolean marked(String key) {
		return marks != null && marks.containsKey(key);
	}

	private Map<String, String> getMarks() {
		if (marks == null) {
			marks = new HashMap<String, String>();
		}
		return marks;
	}
}
