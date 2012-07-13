package engine.search;

import java.io.Closeable;
import java.io.IOException;
import engine.index.TermDocs;
import engine.index.Document;

public interface Searchable extends Closeable {

	void close() throws IOException;

	int docFreq(TermDocs term) throws IOException;

	int[] docFreqs(TermDocs[] terms) throws IOException;

	int maxDoc() throws IOException;

	DocTf search(Weight weight, int n) throws IOException;

	Query rewrite(Query query) throws IOException;

	Document doc(int i) throws IOException;
}
