package search;

public class DocTf {
	public DocTf() {
	}

	public DocTf(int doc, int tf) {
		this.doc = doc;
		this.tf = tf;
	}

	public int doc;
	public int tf;

	public String toString() {
		return "doc:" + doc + " tf:" + tf;
	}
}
