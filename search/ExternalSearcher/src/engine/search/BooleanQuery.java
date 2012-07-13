package engine.search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import javax.swing.text.html.HTMLDocument.HTMLReader.IsindexAction;

import engine.index.TermDocs;

public class BooleanQuery extends Query {
	/**
	 * 
	 */
	private static final long serialVersionUID = -3259188174245547744L;
	private static int maxClauseCount = 1024;
	private ArrayList<BooleanClause> clauses = new ArrayList<BooleanClause>();

	public void add(Query query, BooleanClause.Occur occur) {
		add(new BooleanClause(query, occur));
	}

	public void add(BooleanClause clause) {
		if (clauses.size() >= maxClauseCount)
			System.out.println("maxClauseCount is " + maxClauseCount);
		clauses.add(clause);
	}

	public BooleanClause[] getClauses() {
		return clauses.toArray(new BooleanClause[clauses.size()]);
	}

	public List<BooleanClause> clauses() {
		return clauses;
	}

	public final Iterator<BooleanClause> iterator() {
		return clauses().iterator();
	}

	@Override
	public Similarity getSimilarity(Searcher searcher) {
		Similarity result = super.getSimilarity(searcher);
		return result;
	}

	public void setMinimumNumberShouldMatch(int min) {
		this.minNrShouldMatch = min;
	}

	protected int minNrShouldMatch = 0;

	public int getMinimumNumberShouldMatch() {
		return minNrShouldMatch;
	}

	@Override
	public Scorer scorer(IndexReader reader) throws IOException {
		List<Scorer> required = new ArrayList<Scorer>();
		List<Scorer> prohibited = new ArrayList<Scorer>();
		List<Scorer> optional = new ArrayList<Scorer>();
		for (BooleanClause c : clauses) {
			if (c.getQuery() instanceof TermQuery) {
				System.out.println("query is instanceof termquery");
			} else {
				System.out.println("query is not instanceof termquery");
			}
			Scorer subScorer = c.getQuery().scorer(reader);
			if (c.isRequired()) {
				required.add(subScorer);
			} else if (c.isProhibited()) {
				prohibited.add(subScorer);
			} else {
				optional.add(subScorer);
			}
		}

		if (required.size() == 0 && optional.size() == 0) {
			return null;
		} else if (optional.size() < minNrShouldMatch) {
			return null;
		}

		// Return a BooleanScorer2
		return new BooleanScorer2(minNrShouldMatch, required, prohibited,
				optional);
	}

	@Override
	@SuppressWarnings("unchecked")
	public Object clone() {
		BooleanQuery clone = (BooleanQuery) super.clone();
		clone.clauses = (ArrayList<BooleanClause>) this.clauses.clone();
		return clone;
	}

	/** Prints a user-readable version of this query. */
	@Override
	public String toString(int field) {
		StringBuilder buffer = new StringBuilder();
		boolean needParens = (getMinimumNumberShouldMatch() > 0);
		if (needParens) {
			buffer.append("(");
		}

		for (int i = 0; i < clauses.size(); i++) {
			BooleanClause c = clauses.get(i);
			if (c.isProhibited())
				buffer.append("-");
			else if (c.isRequired())
				buffer.append("+");

			Query subQuery = c.getQuery();
			if (subQuery != null) {
				if (subQuery instanceof BooleanQuery) { // wrap sub-bools in
														// parens
					buffer.append("(");
					buffer.append(subQuery.toString(field));
					buffer.append(")");
				} else {
					buffer.append(subQuery.toString(field));
				}
			} else {
				buffer.append("null");
			}

			if (i != clauses.size() - 1)
				buffer.append(" ");
		}

		if (needParens) {
			buffer.append(")");
		}

		if (getMinimumNumberShouldMatch() > 0) {
			buffer.append('~');
			buffer.append(getMinimumNumberShouldMatch());
		}

		return buffer.toString();
	}

	@Override
	public boolean equals(Object o) {
		if (!(o instanceof BooleanQuery))
			return false;
		BooleanQuery other = (BooleanQuery) o;
		return this.clauses.equals(other.clauses)
				&& this.getMinimumNumberShouldMatch() == other
						.getMinimumNumberShouldMatch();
	}

}
