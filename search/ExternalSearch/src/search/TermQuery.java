package search;

public class TermQuery {
	private int field_;
	private String word_;

	public TermQuery(int field, String word) {
		this.setField_(field);
		this.setWord_(word);
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
