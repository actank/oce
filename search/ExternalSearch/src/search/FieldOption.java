package search;

public class FieldOption {
	private String field_name;
	private int field_id;
	private int field_config;

	FieldOption() {
	}

	public FieldOption getFieldOption() {
		FieldOption option = new FieldOption();
		option.field_name = this.field_name;
		option.field_id = this.field_id;
		option.field_config = this.field_config;
		return option;
	}

	public boolean contains(String name) {
		if (this.field_name == name) {
			return true;
		} else {
			return false;
		}
	}

	public void setField_name(String field_name) {
		this.field_name = field_name;
	}

	public String getField_name() {
		return field_name;
	}

	public void setField_id(int field_id) {
		this.field_id = field_id;
	}

	public int getField_id() {
		return field_id;
	}

	public void setField_config(int field_config) {
		this.field_config = field_config;
	}

	public int getField_config() {
		return field_config;
	}

	public String toString() {
		return "field_name:" + field_name + " field_id:" + field_id
				+ " field_config:" + field_config;
	}
}
