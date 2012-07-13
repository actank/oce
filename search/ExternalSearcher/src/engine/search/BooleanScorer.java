//package engine.search;
//
//import java.io.IOException;
//import java.util.List;
//
//final class BooleanScorer extends Scorer {
//
//	private static final class BooleanScorerCollector extends Collector {
//		private BucketTable bucketTable;
//		private int mask;
//		private Scorer scorer;
//
//		public BooleanScorerCollector(int mask, BucketTable bucketTable) {
//			this.mask = mask;
//			this.bucketTable = bucketTable;
//		}
//
//		@Override
//		public final void collect(final int doc) throws IOException {
//			final BucketTable table = bucketTable;
//			final int i = doc & BucketTable.MASK;
//			Bucket bucket = table.buckets[i];
//			if (bucket == null)
//				table.buckets[i] = bucket = new Bucket();
//
//			if (bucket.doc != doc) { // invalid bucket
//				bucket.doc = doc; // set doc
//				bucket.score = scorer.score(); // initialize score
//				bucket.bits = mask; // initialize mask
//
//				bucket.next = table.first; // push onto valid list
//				table.first = bucket;
//			} else { // valid bucket
//				bucket.score += scorer.score(); // increment score
//				bucket.bits |= mask; // add bits in mask
//			}
//		}
//
//		@Override
//		public void setNextReader(IndexReader reader, int docBase) {
//			// not needed by this implementation
//		}
//
//		@Override
//		public void setScorer(Scorer scorer) throws IOException {
//			this.scorer = scorer;
//		}
//
//		@Override
//		public boolean acceptsDocsOutOfOrder() {
//			return true;
//		}
//
//	}
//
//	private static final class BucketScorer extends Scorer {
//
//		float score;
//		int doc = NO_MORE_DOCS;
//
//		public BucketScorer() {
//			super(null);
//		}
//
//		@Override
//		public int advance(int target) throws IOException {
//			return NO_MORE_DOCS;
//		}
//
//		@Override
//		public int docID() {
//			return doc;
//		}
//
//		@Override
//		public int nextDoc() throws IOException {
//			return NO_MORE_DOCS;
//		}
//
//		@Override
//		public float score() throws IOException {
//			return score;
//		}
//
//	}
//
//	static final class Bucket {
//		int doc = -1; // tells if bucket is valid
//		float score; // incremental score
//		int bits; // used for bool constraints
//		Bucket next; // next valid bucket
//	}
//
//	/** A simple hash table of document scores within a range. */
//	static final class BucketTable {
//		public static final int SIZE = 1 << 11;
//		public static final int MASK = SIZE - 1;
//
//		final Bucket[] buckets = new Bucket[SIZE];
//		Bucket first = null; // head of valid list
//
//		public BucketTable() {
//		}
//
//		public Collector newCollector(int mask) {
//			return new BooleanScorerCollector(mask, this);
//		}
//
//		public final int size() {
//			return SIZE;
//		}
//	}
//
//	static final class SubScorer {
//		public Scorer scorer;
//		public boolean required = false;
//		public boolean prohibited = false;
//		public Collector collector;
//		public SubScorer next;
//
//		public SubScorer(Scorer scorer, boolean required, boolean prohibited,
//				Collector collector, SubScorer next) throws IOException {
//			this.scorer = scorer;
//			this.required = required;
//			this.prohibited = prohibited;
//			this.collector = collector;
//			this.next = next;
//		}
//	}
//
//	private SubScorer scorers = null;
//	private BucketTable bucketTable = new BucketTable();
//	private int maxCoord = 1;
//	private int requiredMask = 0;
//	private int prohibitedMask = 0;
//	private int nextMask = 1;
//	private final int minNrShouldMatch;
//	private int end;
//	private Bucket current;
//	private int doc = -1;
//
//	BooleanScorer(Similarity similarity, int minNrShouldMatch,
//			List<Scorer> optionalScorers, List<Scorer> prohibitedScorers)
//			throws IOException {
//		super(similarity);
//		this.minNrShouldMatch = minNrShouldMatch;
//
//		if (optionalScorers != null && optionalScorers.size() > 0) {
//			for (Scorer scorer : optionalScorers) {
//				maxCoord++;
//				if (scorer.nextDoc() != NO_MORE_DOCS) {
//					scorers = new SubScorer(scorer, false, false,
//							bucketTable.newCollector(0), scorers);
//				}
//			}
//		}
//
//		if (prohibitedScorers != null && prohibitedScorers.size() > 0) {
//			for (Scorer scorer : prohibitedScorers) {
//				int mask = nextMask;
//				nextMask = nextMask << 1;
//				prohibitedMask |= mask; // update prohibited mask
//				if (scorer.nextDoc() != NO_MORE_DOCS) {
//					scorers = new SubScorer(scorer, false, true,
//							bucketTable.newCollector(mask), scorers);
//				}
//			}
//		}
//
//	}
//
//	// firstDocID is ignored since nextDoc() initializes 'current'
//	@Override
//	protected boolean score(Collector collector, int max, int firstDocID)
//			throws IOException {
//		boolean more;
//		Bucket tmp;
//		BucketScorer bs = new BucketScorer();
//		// The internal loop will set the score and doc before calling collect.
//		collector.setScorer(bs);
//		do {
//			bucketTable.first = null;
//
//			while (current != null) { // more queued
//
//				// check prohibited & required
//				if ((current.bits & prohibitedMask) == 0
//						&& (current.bits & requiredMask) == requiredMask) {
//
//					if (current.doc >= max) {
//						tmp = current;
//						current = current.next;
//						tmp.next = bucketTable.first;
//						bucketTable.first = tmp;
//						continue;
//					}
//				}
//
//				current = current.next; // pop the queue
//			}
//
//			if (bucketTable.first != null) {
//				current = bucketTable.first;
//				bucketTable.first = current.next;
//				return true;
//			}
//
//			// refill the queue
//			more = false;
//			end += BucketTable.SIZE;
//			for (SubScorer sub = scorers; sub != null; sub = sub.next) {
//				int subScorerDocID = sub.scorer.docID();
//				if (subScorerDocID != NO_MORE_DOCS) {
//					more |= sub.scorer
//							.score(sub.collector, end, subScorerDocID);
//				}
//			}
//			current = bucketTable.first;
//
//		} while (current != null || more);
//
//		return false;
//	}
//
//	@Override
//	public int advance(int target) throws IOException {
//		throw new UnsupportedOperationException();
//	}
//
//	@Override
//	public int docID() {
//		return doc;
//	}
//
//	@Override
//	public int nextDoc() throws IOException {
//		boolean more;
//		do {
//			while (bucketTable.first != null) { // more queued
//				current = bucketTable.first;
//				bucketTable.first = current.next; // pop the queue
//
//				// check prohibited & required, and minNrShouldMatch
//				if ((current.bits & prohibitedMask) == 0
//						&& (current.bits & requiredMask) == requiredMask) {
//					return doc = current.doc;
//				}
//			}
//
//			// refill the queue
//			more = false;
//			end += BucketTable.SIZE;
//			for (SubScorer sub = scorers; sub != null; sub = sub.next) {
//				Scorer scorer = sub.scorer;
//				sub.collector.setScorer(scorer);
//				int doc = scorer.docID();
//				while (doc < end) {
//					sub.collector.collect(doc);
//					doc = scorer.nextDoc();
//				}
//				more |= (doc != NO_MORE_DOCS);
//			}
//		} while (bucketTable.first != null || more);
//
//		return doc = NO_MORE_DOCS;
//	}
//
//	@Override
//	public void score(Collector collector) throws IOException {
//		score(collector, Integer.MAX_VALUE, nextDoc());
//	}
//
//	@Override
//	public String toString() {
//		StringBuilder buffer = new StringBuilder();
//		buffer.append("boolean(");
//		for (SubScorer sub = scorers; sub != null; sub = sub.next) {
//			buffer.append(sub.scorer.toString());
//			buffer.append(" ");
//		}
//		buffer.append(")");
//		return buffer.toString();
//	}
//
//	@Override
//	public float score() throws IOException {
//		// TODO Auto-generated method stub
//		return 0;
//	}
//
//}
