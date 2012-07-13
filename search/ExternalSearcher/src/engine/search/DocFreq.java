package engine.search;

import java.io.Closeable;
import java.io.IOException;

public interface DocFreq extends Closeable {

	void seek(Term term) throws IOException;

	int doc();

	int freq();

	boolean next() throws IOException;

	int read(int[] docs, int[] freqs) throws IOException;

	boolean skipTo(int target) throws IOException;

	void close() throws IOException;
}
