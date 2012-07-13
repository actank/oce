package engine.search;

import java.io.IOException;

public abstract class DocIdSetIterator {

	private DocTf docTf = new DocTf(-1, -1);

	public static final DocTf NO_MORE_DOCS = new DocTf(Integer.MAX_VALUE, -1);

	public abstract DocTf docTf();

	public abstract DocTf nextDocTf() throws IOException;

	public abstract DocTf advance(DocTf target) throws IOException;

}
