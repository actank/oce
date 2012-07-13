package xce.buddy.logic;

import java.util.List;
import java.util.Map;
import com.xiaonei.platform.core.model.User;

/**
 * 好友分组逻辑接口定义
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */
// TODO:需要做接口定义
public interface GroupLogic {

	/**
	 * 添加好友分组接口
	 * 
	 * @param host
	 * @param groupName
	 * @return
	 */
	public void addGroup(Integer host, String groupName);
	
	/**
	 * 添加好友分组接口
	 * 
	 * @param host
	 * @param groupName
	 * @return
	 */
	public void addGroup(User host, String groupName);

	/**
	 * 删除好友分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @return
	 */
	public void removeGroup(Integer host, Integer groupId);

	/**
	 * 删除好友分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @return
	 */
	public void removeGroup(User host, Integer groupId);
	
	/**
	 * 重命名好友分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @param groupName
	 * @return
	 */
	public void renameGroup(Integer host, Integer groupId, String groupName);
	
	/**
	 * 重命名好友分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @param groupName
	 * @return
	 */
	public void renameGroup(User host, Integer groupId, String groupName);

	/**
	 * 清空好友分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @return
	 */
	public void cleanGroup(Integer host, Integer groupId);
	
	/**
	 * 清空好友分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @return
	 */
	public void cleanGroup(User host, Integer groupId);

	/**
	 * 添加某个好友到某个分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @param guest
	 * @return
	 */
	public void addBuddy(Integer host, Integer groupId, Integer guest);

	/**
	 * 添加某个好友到某个分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @param guest
	 * @return
	 */
	public void addBuddy(User host, Integer groupId, Integer guest);
	
	/**
	 * 从某个分组清除某个好友接口
	 * 
	 * @param host
	 * @param groupId
	 * @param guest
	 * @return
	 */
	public void removeBuddy(Integer host, Integer groupId, Integer guest);

	/**
	 * 从某个分组清除某个好友接口
	 * 
	 * @param host
	 * @param groupId
	 * @param guest
	 * @return
	 */
	public void removeBuddy(User host, Integer groupId, Integer guest);
	
	/**
	 * 添加好友到多个分组接口
	 * 
	 * @param host
	 * @param groupIds
	 * @param guest
	 * @return
	 */
	public void addBuddy(Integer host, Integer[] groupIds, Integer guest);

	/**
	 * 添加好友到多个分组接口
	 * 
	 * @param host
	 * @param groupIds
	 * @param guest
	 * @return
	 */
	public void addBuddy(User host, Integer[] groupIds, Integer guest);
	
	/**
	 * 添加多个好友到某个分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @param guests
	 * @return
	 */
	public void addBuddy(Integer host, Integer groupId, Integer[] guests);

	/**
	 * 添加多个好友到某个分组接口
	 * 
	 * @param host
	 * @param groupId
	 * @param guests
	 * @return
	 */
	public void addBuddy(User host, Integer groupId, Integer[] guests);
	
	/**
	 * 获取所有分组id和组名
	 * 
	 * @param host
	 * @return Map<Integer,String>
	 */
	public Map<Integer, String> getGroup(Integer host);

	/**
	 * 获取所有分组id和组名
	 * 
	 * @param host
	 * @return Map<Integer,String>
	 */
	public Map<Integer, String> getGroup(User host);

	/**
	 * 获取好友guest所属分组id和组名
	 * 
	 * @param host
	 * @return Map<Integer,String>
	 */
	public Map<Integer, String> getGroup(Integer host, Integer guest);

	/**
	 * 获取好友guest所属分组id和组名
	 * 
	 * @param host
	 * @return Map<Integer,String>
	 */
	public Map<Integer, String> getGroup(User host, Integer guest);

	/**
	 * 获取所属某个分组好友id
	 * 
	 * @param host
	 * @return Map<Integer,String>
	 */
	public List<Integer> getBuddy(Integer host, Integer groupId);
	
	/**
	 * 组名，内容合法检查
	 * 
	 * @param host
	 * @param content
	 * @return Map<Integer,String>
	 */
	//public Boolean checkGroup(Integer host, String content);
}
