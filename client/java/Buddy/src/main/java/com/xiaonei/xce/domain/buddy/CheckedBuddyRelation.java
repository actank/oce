package com.xiaonei.xce.domain.buddy;

/**
 * @author yuyang
 * 
 */
public class CheckedBuddyRelation {

	/**
	 * @param relation
	 * @param success
	 */
	public CheckedBuddyRelation(BuddyRelation relation, boolean success) {
		this.relation_ = relation;
		this.success_ = success;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		return relation_.fromId + " " + relation_.toId + " " + success_ + " "
				+ relation_.getDesc();
	}

	/**
	 * @return
	 */
	public BuddyRelation getRelation() {
		return this.relation_;
	}

	/**
	 * @return
	 */
	public boolean getSuccess() {
		return this.success_;
	}

	/**
	 * @param rhp
	 */
	public void setRelation(BuddyRelation rhp) {
		this.relation_ = rhp;
	}

	/**
	 * @param success
	 */
	public void setSuccess(boolean success) {
		this.success_ = success;
	}

	/**
	 * @param rhp
	 * @return
	 */
	public boolean equals(CheckedBuddyRelation rhp) {
		BuddyRelation relation = rhp.getRelation();
		return rhp != null && this.success_ && rhp.getSuccess()
				&& this.relation_.toId == relation.toId
				&& this.relation_.fromId == relation.fromId
				&& this.relation_.equals(relation);
	}

	private BuddyRelation relation_;
	private boolean success_;
}
