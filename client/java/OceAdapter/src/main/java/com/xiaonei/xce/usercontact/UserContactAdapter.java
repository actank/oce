package com.xiaonei.xce.usercontact;

public final class UserContactAdapter<T extends UserContactInfo> {

	private UserContactReaderAdapter<T> _readerAdapter = null;

	private UserContactWriterAdapter _writerAdapter = null;

	public UserContactAdapter(UserContactFactory<T> factory) {
		_readerAdapter = new UserContactReaderAdapter<T>(factory);
		_writerAdapter = new UserContactWriterAdapter();
	}

	public T getUserContact(int id) {
		return _readerAdapter.getUserContact(id);
	}

	public void setUserContact(int id, UserContactInfo obj) {
		_writerAdapter.setUserContact(id, obj);
	}

	public void createUserContact(int id, UserContactInfo obj) {
		_writerAdapter.createUserContact(id, obj);
	}
//
//	//保留给XCE组快速调试使用，不对外发布
//	public static void main(String[] args) throws InterruptedException {
//		class MUserContactInfo extends UserContactInfo {
//		};
//		class MUserContactFactory implements UserContactFactory<MUserContactInfo> {
//			public MUserContactInfo create() {
//				return new MUserContactInfo();
//			}
//		};
//		UserContactAdapter<MUserContactInfo> ad = new UserContactAdapter<MUserContactInfo>(new MUserContactFactory());
//
//		int id = 35178;
//
//		MUserContactInfo oldData = ad.getUserContact(id);
//		oldData.setFixPhone(oldData.getFixPhone());
//		oldData.setHomepage(oldData.getHomepage());
//		oldData.setMobilePhone(oldData.getMobilePhone());
//		oldData.setMsn(oldData.getMsn());
//		oldData.setQq(oldData.getQq());
//		System.out.println("oldData=" + oldData);
//
////		MUserContactInfo setNewData = new MUserContactInfo();
////		setNewData.setFixPhone(oldData.getFixPhone());
////		setNewData.setHomepage(oldData.getHomepage());
////		setNewData.setMobilePhone(oldData.getMobilePhone());
////		setNewData.setMsn(oldData.getMsn());
////		setNewData.setQq(1234567891234L);
////		ad.setUserContact(id, setNewData);
////		MUserContactInfo newData = ad.getUserContact(id);
////		System.out.println("newData=" + newData);
////		if (!newData.getFixPhone().equals(setNewData.getFixPhone()) | !newData.getHomepage().equals(setNewData.getHomepage())
////				| !newData.getMobilePhone().equals(setNewData.getMobilePhone()) | !newData.getMsn().equals(setNewData.getMsn())
////				| newData.getQq() != setNewData.getQq()) {
////			System.out.println("error : set failed!");
////		}
////
//////		ad.setUserContact(id, oldData);
//////
//		System.exit(0);
//	}

};
