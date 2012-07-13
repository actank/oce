package mop.hi.oce.adapter;


public interface ClassApplyCounterAdapter {

	mop.hi.oce.domain.model.ClassApply[] getCounterSeq(int userId);
	void sendApply(int applicant, int classId);
	void acceptApply(int userId, int classId);
	void denyApply(int userId, int classId);
	void addAdmin(int adminId, int classId);
	void removeAdmin(int adminId, int classId);
	void reloadAdmin(int adminId);
	void reloadClass(int classId);
}
