package mop.hi.oce.adapter;

public interface AntispamAdapter {

	public int check(int userid, String text);

	public void addNewSpam(String text);

	public void stopCheck(int serverId, int stat);

	public void changeTimeslot(int slot);

	public void changeRepeatslot(int slot);
}
