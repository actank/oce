package engine.search;

public class Term {
	private int field_;
	private String word_;

	public Term(int field, String word) {
		this.field_ = field;
		this.word_ = word;
	}

	public void setField_(int field_) {
		this.field_ = field_;
	}

	public int getField_() {
		return field_;
	}

	public void setWord_(String word_) {
		this.word_ = word_;
	}

	public String getWord_() {
		return word_;
	}
}
