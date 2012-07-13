package mop.hi.oce.domain.buddy;

import java.util.HashMap;
import java.util.Map;

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

	public int getAccepter() {
		return accepter;
	}

	public void setAccepter(int accepter) {
		this.accepter = accepter;
		this.props.put(BuddyApplyWithInfo.ACCEPTER, String.valueOf(accepter));
	}

	public int getApplicant() {
		return applicant;
	}

	public void setApplicant(int applicant) {
		this.applicant = applicant;
		this.props.put(BuddyApplyWithInfo.APPLICANT, String
				.valueOf(applicant));
	}

}
