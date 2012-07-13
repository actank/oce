package mop.hi.oce.domain.buddy;

import java.util.Date;

import mop.hi.oce.adapter.AdapterFactory;
import xce.util.task.Task;

public class AddFriendTask extends Task {

	public AddFriendTask(int fromId, int toId) {
		_fromId = fromId;
		_toId = toId;
	}

	private final int _fromId;
	private final int _toId;

	public void run() {
		BuddyApplyWithInfo info = new BuddyApplyWithInfo(_fromId, _toId);
		info.setTime(new Date());
		AdapterFactory.getBuddyCoreAdapter().addApply(info);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
		}
		AdapterFactory.getBuddyCoreAdapter().acceptApply(
				new BuddyApply(_fromId, _toId));
	}
}
