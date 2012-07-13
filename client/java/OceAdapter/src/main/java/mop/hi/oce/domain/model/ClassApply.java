package mop.hi.oce.domain.model;

public class ClassApply {

	private int classId;
	private int count;
	private String className;
	
	public ClassApply() {
	}
	public ClassApply(int classId, int count, String className) {
		this.classId = classId;
		this.count = count;
		this.className = className;
	}
	
	public int getClassId() {
		return classId;
	}
	public void setClassId(int classId) {
		this.classId = classId;
	}
	public int getCount() {
		return count;
	}
	public void setCount(int count) {
		this.count = count;
	}
	public String getClassName() {
		return className;
	}
	public void setClassName(String className) {
		this.className = className;
	}
	
}
