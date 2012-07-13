package engine.search;

public class TopDocs implements java.io.Serializable {
	public int totalHits;
	public ScoreDoc[] scoreDocs;
	private float maxScore;

	public float getMaxScore() {
		return maxScore;
	}

	public void setMaxScore(float maxScore) {
		this.maxScore = maxScore;
	}

	TopDocs(int totalHits, ScoreDoc[] scoreDocs) {
		this(totalHits, scoreDocs, Float.NaN);
	}

	public TopDocs(int totalHits, ScoreDoc[] scoreDocs, float maxScore) {
		this.totalHits = totalHits;
		this.scoreDocs = scoreDocs;
		this.maxScore = maxScore;
	}

}
