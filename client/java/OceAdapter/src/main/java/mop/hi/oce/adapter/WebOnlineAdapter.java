package mop.hi.oce.adapter;

import java.util.List;
import java.util.Map;

@Deprecated
public interface WebOnlineAdapter {
	public void login(int userId);

	public void logout(int userId);

	public void keepOnline(int userId);

	public List<Integer> getOnlineUsers(int begin, int limit);

	public int getOnlineSize();

	public Map<Integer, Integer> getOnlineStatus(int[] users);

	public int getOnlineStatus(int user);
}
