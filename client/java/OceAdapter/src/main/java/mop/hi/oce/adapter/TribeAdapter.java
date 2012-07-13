package mop.hi.oce.adapter;

import java.util.List;
import java.util.Map;

import mop.hi.oce.domain.model.Tribe;
import mop.hi.oce.domain.model.TribeAdmin;
import mop.hi.oce.domain.model.TribeMember;
import mop.hi.svc.model.TribeException;

public interface TribeAdapter {
	/**
	 * 创建一个群组
	 * 
	 * @param tribe
	 * @return
	 */
	Tribe createTribe(Tribe tribe) throws TribeException;

	/**
	 * 删除一个群组，不可恢复！！
	 * 
	 * @param tribeId
	 */
	void removeTribe(int tribeId) throws TribeException;

	/**
	 * 让中间层重新加载这个群组。 在直接操作过数据库之后，用于同步。
	 * 
	 * @param tribeId
	 */
	void reloadTribe(int tribeId);

	/**
	 * 获得一个群组。
	 * 
	 * @param tribeId
	 * @return
	 * @throws TribeException
	 */
	Tribe getTribe(int tribeId) throws TribeException;

	/**
	 * 设置群组属性
	 * 
	 * @param tribeId
	 * @param tribe
	 */
	void setTribe(int tribeId, Tribe tribe) throws TribeException;

	// Member

	void reloadTribeMember(int tribeId);

	/**
	 * 给一个群组增加一个成员
	 */
	void addMember(int tribeId, TribeMember member) throws TribeException;

	/**
	 * 从一个群组删除一个成员
	 * 
	 * @param tribeId
	 * @param userId
	 * @return
	 */
	void removeMember(int tribeId, int userId) throws TribeException;

	int getMemberCount(int tribeId, Map<String, String> filter)
			throws TribeException;

	TribeMember getMember(int tribeId, int userId) throws TribeException;

	List<TribeMember> getMembers(int tribeId, List<Integer> memberIds)
			throws TribeException;

	List<TribeMember> getMatchedMembers(int tribeId,
			Map<String, String> filter, int offset, int limit)
			throws TribeException;

	void setMember(int tribeId, int userId, Map<String, String> properties)
			throws TribeException;

	// Admin
	void reloadTribeAdmin(int tribeId);

	void addAdmin(int tribeId, TribeAdmin admin) throws TribeException;

	void removeAdmin(int tribeId, int userId) throws TribeException;

	int getAdminCount(int tribeId, Map<String, String> filter)
			throws TribeException;

	TribeAdmin getAdmin(int tribeId, int userId) throws TribeException;

	List<TribeAdmin> getAdmins(int tribeId, List<Integer> memberIds)
			throws TribeException;

	List<TribeAdmin> getMatchedAdmins(int tribeId, Map<String, String> filter,
			int offset, int limit) throws TribeException;

	void setAdmin(int tribeId, int userId, Map<String, String> properties)
			throws TribeException;

}
