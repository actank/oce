package mop.hi.oce.domain.model;

import java.util.Map;

public class UserRightObject {

	public static final String STATUS = "STATUS";

	public static final String AUTH = "AUTH";

	public static final String COMMON_RIGHT = "COMMON_RIGHTS";

	public static final String DESCBAN = "DESCBAN";

	private int status;

	private int auth;

	private int commonRight;

	private int descban;

	public int getAuth() {
		return auth;
	}

	public int getCommonRight() {
		return commonRight;
	}

	public int getDescban() {
		return descban;
	}

	public int getStatus() {
		return status;
	}

	public void valueOf(Map<String, String> props) {
		this.auth = Integer.parseInt(props.get(UserRightObject.AUTH));
		this.commonRight = Integer.parseInt(props
				.get(UserRightObject.COMMON_RIGHT));
		this.descban = Integer.parseInt(props.get(UserRightObject.DESCBAN));
		this.status = Integer.parseInt(props.get(UserRightObject.STATUS));
	}

}
