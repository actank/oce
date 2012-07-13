package engine.search;

public class BooleanClause implements java.io.Serializable {

	public static enum Occur {

		MUST {
			@Override
			public String toString() {
				return "+";
			}
		},

		SHOULD {
			@Override
			public String toString() {
				return "";
			}
		},

		MUST_NOT {
			@Override
			public String toString() {
				return "-";
			}
		};

	}

	private Query query;

	private Occur occur;

	public BooleanClause(Query query, Occur occur) {
		this.query = query;
		this.occur = occur;

	}

	public Occur getOccur() {
		return occur;
	}

	public void setOccur(Occur occur) {
		this.occur = occur;

	}

	public Query getQuery() {
		return query;
	}

	public void setQuery(Query query) {
		this.query = query;
	}

	public boolean isProhibited() {
		return Occur.MUST_NOT == occur;
	}

	public boolean isRequired() {
		return Occur.MUST == occur;
	}

	@Override
	public boolean equals(Object o) {
		if (o == null || !(o instanceof BooleanClause))
			return false;
		BooleanClause other = (BooleanClause) o;
		return this.query.equals(other.query) && this.occur == other.occur;
	}

	@Override
	public int hashCode() {
		return query.hashCode() ^ (Occur.MUST == occur ? 1 : 0)
				^ (Occur.MUST_NOT == occur ? 2 : 0);
	}

	@Override
	public String toString() {
		return occur.toString() + query.toString();
	}
}
