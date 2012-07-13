package engine.search;

import java.io.IOException;

public abstract class Collector {

	public abstract void setScorer(Scorer scorer) throws IOException;

	public abstract void collect(DocTf docTf) throws IOException;

	public abstract void setNextReader(IndexReader reader, int docBase)
			throws IOException;

	public abstract boolean acceptsDocsOutOfOrder();

}
