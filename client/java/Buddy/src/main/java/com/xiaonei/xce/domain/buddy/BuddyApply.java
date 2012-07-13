package com.xiaonei.xce.domain.buddy;

import java.util.HashMap;
import java.util.Map;

/**
 * @author yuyang
 * 
 */
public class BuddyApply {
	public BuddyApply(int applicant, int accepter) {
		setApplicant(applicant);
		setAccepter(accepter);
	}

	protected Map<String, String> props = new HashMap<String, String>();

	public Map<String, String> getProperties() {
		return props;
	}

	public static String APPLICANT = "APPLICANT";

	public static String ACCEPTER = "ACCEPTER";

	private int applicant;

	private int accepter;

	/**
	 * @return
	 */
	public int getAccepter() {
		return accepter;
	}

	/**
	 * @param accepter
	 */
	public void setAccepter(int accepter) {
		this.accepter = accepter;
		this.props.put(BuddyApplyWithInfo.ACCEPTER, String.valueOf(accepter));
	}

	/**
	 * @return
	 */
	public int getApplicant() {
		return applicant;
	}

	/**
	 * @param applicant
	 */
	public void setApplicant(int applicant) {
		this.applicant = applicant;
		this.props.put(BuddyApplyWithInfo.APPLICANT, String.valueOf(applicant));
	}

}
