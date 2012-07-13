package engine.search;

import java.util.HashMap;
import java.util.Map;

public class TransformCondition {
	private Map<String, String> condition;
	private Map<String, String> keymap;

	public TransformCondition(Map<String, String> condition,
			Map<String, String> keymap) {
		this.condition = condition;
		this.keymap = keymap;
	}

	public Map<String, String> transform() {

		Map<String, String> query = new HashMap<String, String>();

		for (Map.Entry<String, String> entry : condition.entrySet()) {
			System.out.println("BEFORE TRANSFORM key:" + entry.getKey()
					+ " value:" + entry.getValue());
			if (entry.getKey().equals("query")) {
				query.put("DEFAULT", entry.getValue());
			} else {
				String[] keys = entry.getKey().split("\\|");
				int length = keys.length;
				if (length < 2) {
					continue;
				}
				String oldKey = keys[0] + "." + keys[length - 1];
				if (keymap.containsKey(oldKey) && entry.getValue() != null
						&& !entry.getValue().equals("")) {
					query.put(keymap.get(oldKey), entry.getValue());
				}
				System.out.println("oldKey:" + oldKey + " newKey:"
						+ keymap.get(oldKey));
			}
		}

		if (query.get("junior.year") != null
				&& !query.get("junior.year").equals("")
				&& query.get("junior.year").equals("0")) {
			query.remove("junior.year");
		}

		if (query.get("high.year") != null
				&& !query.get("high.year").equals("")
				&& query.get("high.year").equals("0")) {
			query.remove("high.year");
		}

		if (query.get("univ.year") != null
				&& !query.get("univ.year").equals("")
				&& query.get("univ.year").equals("0")) {
			query.remove("univ.year");
		}

		if (query.containsKey("junior.name")
				&& query.containsKey("junior.year")) {
			String value = query.get("junior.name") + "."
					+ query.get("junior.year");
			query.put("junior.nameyear", value);
			query.remove("junior.name");
			query.remove("junior.year");
		}

		if (query.containsKey("high.name") && query.containsKey("high.year")) {
			String value = query.get("high.name") + "."
					+ query.get("high.year");
			query.put("high.nameyear", value);
			query.remove("high.name");
			query.remove("high.year");
		}

		if (query.containsKey("univ.name") && query.containsKey("univ.year")) {
			String value = query.get("univ.name") + "."
					+ query.get("univ.year");
			query.put("univ.nameyear", value);
			query.remove("univ.name");
			query.remove("univ.year");
		}

		return query;
	}
}
