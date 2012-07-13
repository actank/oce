package engine.search;

import java.io.IOException;
import java.util.ArrayList;

public abstract class Query implements java.io.Serializable, Cloneable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 5788685310761042402L;

	public abstract String toString(int field);

	private Similarity similarity;

	public Similarity getSimilarity(Searcher searcher) {
		return searcher.getSimilarity();
	}

	public abstract Scorer scorer(IndexReader reader) throws IOException;

	@Override
	public Object clone() {
		try {
			return super.clone();
		} catch (CloneNotSupportedException e) {
			throw new RuntimeException("Clone not supported: " + e.getMessage());
		}
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		return true;
	}

	public void setSimilarity(Similarity similarity) {
		this.similarity = similarity;
	}

	public Similarity getSimilarity() {
		return similarity;
	}
}
