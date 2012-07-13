package engine.search;

import java.io.IOException;
import java.io.Serializable;

public abstract class Weight implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public abstract Query getQuery();

	public abstract Scorer scorer(IndexReader reader) throws IOException;

	public abstract float getValue();

	public abstract float sumOfSquaredWeights() throws IOException;

	public boolean scoresDocsOutOfOrder() {
		return false;
	}

}
