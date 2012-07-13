package engine.search;

import java.io.Serializable;


public abstract class Similarity implements Serializable {
	 private static Similarity defaultImpl = new DefaultSimilarity();
	  public static void setDefault(Similarity similarity) {
		    Similarity.defaultImpl = similarity;
		  }

		  public static Similarity getDefault() {
		    return Similarity.defaultImpl;
		  }
		  public float tf(int freq) {
			    return tf((float)freq);
			  }
		  public abstract float tf(float freq);
		  public abstract float idf(int docFreq, int numDocs);
}
