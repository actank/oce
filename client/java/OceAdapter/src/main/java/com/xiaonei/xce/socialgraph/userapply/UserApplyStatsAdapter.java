package com.xiaonei.xce.socialgraph.userapply;

import java.util.BitSet;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
import com.xiaonei.xce.socialgraph.puk.PukRecommendAdapter;

/**
 * Get User's applying status of recent month
 * @author Administrator
 * @email zhangnan@renren-inc.com
 */
public class UserApplyStatsAdapter {
	private static UserApplyStatsAdapter instance = null;
	private static final String biz = "USERAPPLY";
	private static int bitsetSize = 31;		//set 31 as size of bitset
	
	private UserApplyStatsAdapter() {
	}
	
	public static UserApplyStatsAdapter getInstance() {
		if (null == instance) {
			synchronized (PukRecommendAdapter.class) {
				if (null == instance) {
					try {
						instance = new UserApplyStatsAdapter();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	/**
	 * Get User Apply Status
	 * @param userId
	 * @return
	 */
	public BitSet getUserApplyStats(int userId) {
		BitSet bit = new BitSet(bitsetSize);
		byte[] data = BusinessCacheAdapter.getInstance().get(biz, userId);
		if (null == data || data.length == 0) { 		//if there is no data stored, that means user has no action of applying
			return bit;
		}
		
		try {
			Items result = Items.parseFrom(data);
			if (result.getItemsCount() != 0) {
				int applyDays = result.getItems(0).getField();	//store apply information in field of item located in position 0
				for (int i = 0; i < bitsetSize; ++i) {
					if ((applyDays & 1) != 0) {
						bit.set(i);
					}
					applyDays = applyDays >>> 1;
				}
			}
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		return bit;
	}
	
	public static void main(String[] args) {
		int userId = 4210;
		BitSet rr = UserApplyStatsAdapter.getInstance().getUserApplyStats(userId);
		System.out.println(rr.cardinality());
		System.exit(0);
	}
}
