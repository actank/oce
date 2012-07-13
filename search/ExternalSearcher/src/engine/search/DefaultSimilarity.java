package engine.search;

public class DefaultSimilarity extends Similarity {

	@Override
	public float tf(float freq) {
		return (float) Math.sqrt(freq);
	}

	@Override
	public float idf(int docFreq, int numDocs) {
		return (float) (Math.log(numDocs / (double) (docFreq + 1)) + 1.0);
	}

}
