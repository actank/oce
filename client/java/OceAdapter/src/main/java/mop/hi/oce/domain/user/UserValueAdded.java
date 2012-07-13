package mop.hi.oce.domain.user;

import java.util.Map;

import mop.hi.oce.domain.Markable;
import mop.hi.svc.model.ValueAddedData;

public class UserValueAdded extends Markable {
	public static final String USERID = "USERID";

	public static final String NICK_NAME = "NICKNAME";

	private ValueAddedData va = new ValueAddedData();

	public UserValueAdded parse(Map<String, String> properties) {
		this.va.nickname = (properties.get(UserValueAdded.NICK_NAME));
		return this;
	}

	public int getUserId() {
		return va.userId;
	}

	public void setUserId(int userId) {
		va.userId = userId;
		mark(UserValueAdded.USERID, String.valueOf(va.userId));
	}

	public String getNickName() {
		return va.nickname;
	}

	public void setNickName(String nickName) {
		va.nickname = nickName;
		mark(UserValueAdded.NICK_NAME, va.nickname);
	}

	public ValueAddedData get() {
		return va;
	}

	public void set(ValueAddedData va) {
		this.va = va;
	}

}
