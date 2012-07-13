package mop.hi.oce.domain.user;

public class UserHomeViewFactory {
	public UserHomeView create() {
		return new UserHomeView();
	}
	public User createUser() {
		return new User();
	}
	public UserBasic createUserBasic() {
		return new UserBasic();
	}
	public UserDesc createUserDesc() {
		return new UserDesc();
	}
}
