package engine.index;

import java.util.ArrayList;
import java.util.List;

public class Document {
	private int docid;
	private List<Field> fields = null;
	private int maxField = 0;

	public Document(int maxField) {
		this.maxField = maxField;
		fields = new ArrayList<Field>(maxField);
	}

	public void setDoc(int doc) {
		this.docid = doc;
	}

	public int getDoc() {
		return docid;
	}

	public void addField(Field field) {
		if (fields.size() < maxField)
			fields.add(field);
	}

	public List<Field> getFields() {
		return fields;
	}

	public int getFieldSize() {
		return maxField;
	}

	public Field getField(int field) {
		if (field >= 0 && field < fields.size()) {
			return fields.get(field);
		} else {
			return null;
		}
	}
}
