package engine.index;

public class DocField {
	public DocField(int doc, Field field) {
		this.doc = doc;
		this.field = field;
	}

	public int doc;
	public Field field;
}