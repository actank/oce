package com.xiaonei.xce.usertime;

import java.util.Date;

public final class UserTimeAdapter<T extends UserTimeInfo> {

	private UserTimeReaderAdapter<T> _readerAdapter = null;
	private UserTimeWriterAdapter _writerAdapter = null;

	public UserTimeAdapter(UserTimeFactory<T> factory) {
		_readerAdapter = new UserTimeReaderAdapter<T>(factory);
		_writerAdapter = new UserTimeWriterAdapter();
	}

	public T getUserTime(int id) {
		return _readerAdapter.getUserTime(id);
	}

	public void setUserTime(int id, UserTimeInfo obj) {
		_writerAdapter.setUserTime(id, obj);
	}

	public void createUserTime(int id, UserTimeInfo obj) {
		_writerAdapter.createUserTime(id, obj);
	}

	public static void main(String[] args) throws InterruptedException {
		class MUserTimeInfo extends UserTimeInfo {
		}
		;
		class MUserTimeFactory implements UserTimeFactory<MUserTimeInfo> {
			public MUserTimeInfo create() {
				return new MUserTimeInfo();
			}
		}
		;
		UserTimeAdapter<MUserTimeInfo> ad = new UserTimeAdapter<MUserTimeInfo>(
				new MUserTimeFactory());
			Thread.sleep(1000);
			
			int id = 386246921;
			
			MUserTimeInfo oldData = ad.getUserTime(id);
			oldData.setActivateTime(oldData.getActivateTime());
			oldData.setLastLoginTime(oldData.getLastLoginTime());
			oldData.setLastLogoutTime(oldData.getLastLogoutTime());
			oldData.setRegisterTime(oldData.getRegisterTime());
			
			MUserTimeInfo setNewData = new MUserTimeInfo();
			setNewData.setActivateTime(new Date(id * 1000l));
			setNewData.setLastLoginTime(new Date((id+1) * 1000l));
			setNewData.setLastLogoutTime(new Date((id+2) * 1000l));
			setNewData.setRegisterTime(new Date((id+3) * 1000l));
			
			ad.setUserTime(id, setNewData);
			
			MUserTimeInfo newData = ad.getUserTime(id);
			
			if(!newData.getActivateTime().equals(setNewData.getActivateTime()) | 
					!newData.getLastLoginTime().equals(setNewData.getLastLoginTime()) |
					!newData.getLastLogoutTime().equals( setNewData.getLastLogoutTime()) |
					!newData.getRegisterTime().equals(setNewData.getRegisterTime()) )
			{
				System.out.println("error : set failed!");
			}
			
			ad.setUserTime(id, oldData);

		 System.exit(0);
	}

}
