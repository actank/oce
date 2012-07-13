package mop.hi.oce.adapter;

public interface PassportAdapter {
	public String createTicket(int userId, boolean isPersistent);

	public void destroyTicket(String ticket);

	public void impersistence(String ticket);

	public Integer verifyTicket(String ticket);

	public String queryTicket(int userId);
}
