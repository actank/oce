package engine.index;

import java.util.ArrayList;

public class TermDocs {
	static public final int MAX = 1024;

	abstract class Doc {
		public abstract void init(int doc, int tf);

		public int getDoc() {
			return doc;
		}

		public abstract int getTf();

		public int doc;
	};

	public class DocWithoutTf extends Doc {

		@Override
		public void init(int doc, int tf) {
			this.doc = doc;
		}

		@Override
		public int getTf() {
			return 0;
		}
	}

	public class DocWithTf extends Doc {

		public int tf;

		@Override
		public void init(int doc, int tf) {
			this.doc = doc;
			this.tf = tf;
		}

		@Override
		public int getTf() {
			return tf;
		}
	}

	private ArrayList<Doc> termDocs = new ArrayList<Doc>(1);

	public void addTermDoc(int doc, int tf) {

		Doc d;
		if (tf <= 0) {
			d = new DocWithoutTf();
		} else {
			d = new DocWithTf();
		}
		d.init(doc, tf);
		termDocs.add(d);
	}

	public ArrayList<Doc> getDocAndTf() {
		return termDocs;
	}

	public boolean isEnd() {
		if (termDocs.size() >= MAX) {
			return true;
		} else {
			return false;
		}
	}
}
