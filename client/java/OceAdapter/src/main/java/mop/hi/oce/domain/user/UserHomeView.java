package mop.hi.oce.domain.user;


public class UserHomeView {
	protected User user;

	protected UserBasic userbasic;

	protected UserDesc userdesc;

	protected int userOnlineStatus;

	protected int userVisitCount;

		
	public User getUser() {
		return user;
	}

	public void setUser(User user) {
		this.user = user;
	}

	public UserBasic getUserbasic() {
		return userbasic;
	}

	public void setUserbasic(UserBasic userbasic) {
		this.userbasic = userbasic;
	}

	public UserDesc getUserdesc() {
		return userdesc;
	}

	public void setUserdesc(UserDesc userdesc) {
		this.userdesc = userdesc;
	}

	public int getUserOnlineStatus() {
		return userOnlineStatus;
	}

	public void setUserOnlineStatus(int userOnlineStatus) {
		this.userOnlineStatus = userOnlineStatus;
	}

	public int getUserVisitCount() {
		return userVisitCount;
	}

	public void setUserVisitCount(int userVisitCount) {
		this.userVisitCount = userVisitCount;
	}

}
