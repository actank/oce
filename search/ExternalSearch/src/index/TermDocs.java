package index;

import java.util.ArrayList;

public class TermDocs {
	public class DocAndTf {
		public DocAndTf(int doc, int tf) {
			this.doc = doc;
			this.tf = tf;
		}

		public int doc;
		public int tf;
	}

	private ArrayList<DocAndTf> termDocs = new ArrayList<DocAndTf>(128);

	public void addTermDoc(int doc, int tf) {
		termDocs.add(new DocAndTf(doc, tf));
	}

	public ArrayList<DocAndTf> getDocAndTf() {
		return termDocs;
	}
}
