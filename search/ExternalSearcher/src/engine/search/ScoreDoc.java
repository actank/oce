package engine.search;

public class ScoreDoc implements java.io.Serializable {
	 
	  /**
	 * 
	 */
	private static final long serialVersionUID = -4468336281125837986L;

	public float score;

	  public int doc;

	  public ScoreDoc(int doc, float score) {
	    this.doc = doc;
	    this.score = score;
	  }
	  
	  // A convenience method for debugging.
	  @Override
	  public String toString() {
	    return "doc=" + doc + " score=" + score;
	  }
	  
	}

