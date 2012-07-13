package mop.hi.oce.domain.model.importer;

public class ImporterOwner {
	/**
	 * 构造带密码的ImporterOwner结构
	 * 用来发送导入请求。
	 * @param u
	 * @param e
	 * @param p
	 */
	public ImporterOwner(int u, String e, String p) {
		userId = u;
		email = e;
		password = p;
	}

	/**
	 * 构造不带密码的ImporterOwner结构
	 * 用来取导入结果。
	 * @param u
	 * @param e
	 */
	public ImporterOwner(int u, String e) {
		userId = u;
		email = e;
		password = "";
	}

	/**
	 * Web不需要使用的方法。
	 * @param owner
	 */
	public ImporterOwner(com.xiaonei.svc.importer.OwnerInfo owner){
		userId = owner.userId;
		email = owner.email;
		password = owner.password;
	}
	
	/**
	 * Web不需要使用的方法。
	 * @return
	 */
	public com.xiaonei.svc.importer.OwnerInfo toSvcOwner() {
		com.xiaonei.svc.importer.OwnerInfo result = new com.xiaonei.svc.importer.OwnerInfo();
		result.userId = userId;
		result.email = email;
		result.password = password;
		return result;
	}

	public final int userId;
	public final String email;
	public final String password;
}
