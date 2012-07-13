package mop.hi.oce.domain.buddy;

import com.xiaonei.xce.log.Oce;

public class BuddyRelation {
	public enum BuddyDesc {
		Self, Friend, Apply, Applied, Block, Blocked, BiBlock, NoPath
	};

	public static BuddyDesc parseBuddyDesc(
			com.xiaonei.service.buddy.BuddyDesc svcDesc) {
		if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.Self) {
			return BuddyDesc.Self;
		} else if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.Friend) {
			return BuddyDesc.Friend;
		} else if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.Apply) {
			return BuddyDesc.Apply;
		} else if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.Applied) {
			return BuddyDesc.Applied;
		} else if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.Block) {
			return BuddyDesc.Block;
		} else if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.Blocked) {
			return BuddyDesc.Blocked;
		} else if (svcDesc == com.xiaonei.service.buddy.BuddyDesc.BiBlock) {
			return BuddyDesc.BiBlock;
		} else {
			return BuddyDesc.NoPath;
		}
	}
	
	public static BuddyDesc parseBuddyDesc(
			xce.buddy.BuddyDesc svcDesc) {
		if (svcDesc == xce.buddy.BuddyDesc.Self) {
			return BuddyDesc.Self;
		} else if (svcDesc == xce.buddy.BuddyDesc.Friend) {
			return BuddyDesc.Friend;
		} else if (svcDesc == xce.buddy.BuddyDesc.Apply) {
			return BuddyDesc.Apply;
		} else if (svcDesc == xce.buddy.BuddyDesc.Applied) {
			return BuddyDesc.Applied;
		} else if (svcDesc == xce.buddy.BuddyDesc.Block) {
			return BuddyDesc.Block;
		} else if (svcDesc == xce.buddy.BuddyDesc.Blocked) {
			return BuddyDesc.Blocked;
		} else if (svcDesc == xce.buddy.BuddyDesc.BiBlock) {
			return BuddyDesc.BiBlock;
		} else {
			return BuddyDesc.NoPath;
		}
	}
	
	public static xce.buddy.BuddyDesc parseBuddyDesc(
			BuddyDesc svcDesc) {
		if (svcDesc == BuddyDesc.Self) {
			return xce.buddy.BuddyDesc.Self;
		} else if (svcDesc == BuddyDesc.Friend) {
			return xce.buddy.BuddyDesc.Friend;
		} else if (svcDesc == BuddyDesc.Apply) {
			return xce.buddy.BuddyDesc.Apply;
		} else if (svcDesc == BuddyDesc.Applied) {
			return xce.buddy.BuddyDesc.Applied;
		} else if (svcDesc == BuddyDesc.Block) {
			return xce.buddy.BuddyDesc.Block;
		} else if (svcDesc == BuddyDesc.Blocked) {
			return xce.buddy.BuddyDesc.Blocked;
		} else if (svcDesc == BuddyDesc.BiBlock) {
			return xce.buddy.BuddyDesc.BiBlock;
		} else {
			return xce.buddy.BuddyDesc.NoPath;
		}
	}

	public final int fromId;
	public final int toId;
	private BuddyDesc desc;

	protected BuddyRelation(int fromId, int toId) {
		this.fromId = fromId;
		this.toId = toId;
	}

	public BuddyRelation(int fromId, int toId, BuddyDesc desc) {
		this.fromId = fromId;
		this.toId = toId;
		this.desc = desc;
	}

	@Deprecated
	public BuddyDesc getDesc() {
		Oce.getLogger().error("CAUTION: Use getDesc INTERNAL ONLY!");
		return desc;
	}

	// public void setDesc(BuddyDesc desc) {
	// this.desc = desc;
	// }

	public boolean isFriend() {
		if (desc.equals(BuddyDesc.Friend)) {
			return true;
		}
		return false;
	}

	public boolean isApply() {
		if (desc.equals(BuddyDesc.Apply)) {
			return true;
		}
		return false;
	}

	public boolean isApplied() {
		if (desc.equals(BuddyDesc.Applied)) {
			return true;
		}
		return false;
	}

	public boolean isBlock() {
		if (desc.equals(BuddyDesc.Block) || desc.equals(BuddyDesc.BiBlock)) {
			return true;
		}
		return false;
	}

	public boolean isBlocked() {
		if (desc.equals(BuddyDesc.Blocked) || desc.equals(BuddyDesc.BiBlock)) {
			return true;
		}
		return false;
	}
	
	public boolean isNoPath() {
		if (desc.equals(BuddyDesc.NoPath) ) {
			return true;
		}
		return false;
	}
	
	public boolean equals(BuddyRelation rhp) {
		if(rhp != null && fromId == rhp.fromId && toId == rhp.toId && desc == rhp.desc) {
			return true;
		} else {
			return false;
		}
	}
}
