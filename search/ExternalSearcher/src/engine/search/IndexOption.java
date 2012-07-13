package engine.search;

public class IndexOption {
	private int doc_freq;
	private long offset;

	IndexOption() {
	}

	public IndexOption getIndexOption() {
		IndexOption option = new IndexOption();
		option.doc_freq = this.doc_freq;
		option.offset = this.offset;
		return option;
	}

	public void setDoc_freq(int doc_freq) {
		this.doc_freq = doc_freq;
	}

	public int getDoc_freq() {
		return doc_freq;
	}

	public void setOffset(long offset) {
		this.offset = offset;
	}

	public long getOffset() {
		return offset;
	}

	public String toString() {
		return "doc_freq:" + doc_freq + " offset:" + offset;
	}
}
