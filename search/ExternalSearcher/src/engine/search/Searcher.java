package engine.search;

import java.io.IOException;

import engine.index.Document;
import engine.index.TermDocs;

public abstract class Searcher implements Searchable {

	public DocTf search(Query query, int n) throws IOException {
		return search(query, n);
	}

	private Similarity similarity = Similarity.getDefault();

	public void setSimilarity(Similarity similarity) {
		this.similarity = similarity;
	}

	public Similarity getSimilarity() {
		return this.similarity;
	}

	public int[] docFreqs(TermDocs[] terms) throws IOException {
		int[] result = new int[terms.length];
		for (int i = 0; i < terms.length; i++) {
			result[i] = docFreq(terms[i]);
		}
		return result;
	}

	abstract public void close() throws IOException;

	abstract public int docFreq(TermDocs term) throws IOException;

	abstract public int maxDoc() throws IOException;

	abstract public DocTf search(Weight weight, int n) throws IOException;

	abstract public Document doc(int i) throws IOException;

	abstract public Query rewrite(Query query) throws IOException;

}
