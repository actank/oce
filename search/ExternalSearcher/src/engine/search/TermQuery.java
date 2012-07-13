package engine.search;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.lucene.store.IndexInput;

import engine.common.FieldConfig;

public class TermQuery extends Query {
	/**
	 * 
	 */
	private static final long serialVersionUID = 7661255888966619883L;
	private Term term;
	private FieldOffset fieldOffset;

	public TermQuery(Term t) throws IOException {
		this.term = t;
	}

	public Term getTerm() {
		return term;
	}

	@Override
	public Scorer scorer(IndexReader reader) throws IOException {
		// TODO Auto-generated method stub
		fieldOffset = new FieldOffset(term, reader);
		return new TermScorer(fieldOffset);
	}

	@Override
	public String toString(int field) {
		StringBuilder buffer = new StringBuilder();
		if (term.getField_() != field) {
			buffer.append(term.getField_());
			buffer.append(":");
		}
		buffer.append(term.getWord_());
		return buffer.toString();
	}

	@Override
	public boolean equals(Object o) {
		if (!(o instanceof TermQuery))
			return false;
		TermQuery other = (TermQuery) o;
		return this.term.equals(other.term);
	}

}
