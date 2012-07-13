package engine.search;

public class DocTf {
	public DocTf() {
	}

	public DocTf(int doc, int tf) {
		this.doc = doc;
		this.tf = tf;
	}

	public boolean equals(DocTf doctf1) {
		if (this.doc == doctf1.doc && this.tf == doctf1.tf) {
			return true;
		}
		return false;
	}

	public int doc;
	public int tf;

	public String toString() {
		return "doc:" + doc + " tf:" + tf;
	}
}
