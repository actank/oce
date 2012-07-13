package mop.hi.oce.domain.buddy;

public class CheckedBuddyRelation {
	public CheckedBuddyRelation(BuddyRelation relation, boolean success) {
		this.relation_ = relation;
		this.success_ = success;
	}
	public String toString() {
		return relation_.fromId + " " + relation_.toId + " " + success_ + " " + relation_.getDesc();
	}
	public BuddyRelation getRelation() {
		return this.relation_;
	}
	public boolean getSuccess() {
		return this.success_;
	}
	public void setRelation(BuddyRelation rhp) {
		this.relation_ = rhp;
	}
	public void setSuccess(boolean success) {
		this.success_ = success;
	}
	public boolean equals(CheckedBuddyRelation rhp) {
		BuddyRelation relation = rhp.getRelation();
		return rhp != null && this.success_ && rhp.getSuccess() && this.relation_.toId == relation.toId &&
			this.relation_.fromId == relation.fromId && this.relation_.equals(relation);
	}
	private BuddyRelation relation_;
	private boolean success_;
}
