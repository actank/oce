package index;

public class Field {
	private String name = null;
	private String value = null;

	public Field(String field_name, String field_value) {
		this.setName(field_name);
		this.setValue(field_value);
	}

	public void setName(String field_name) {
		this.name = field_name;
	}

	public String getName() {
		return name;
	}

	public void setValue(String value) {
		this.value = value;
	}

	public String getValue() {
		return value;
	}

}
