package com.xiaonei.xce.socialgraph.friendfinderworker;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

/**
 * 二度好友数据结构为从db取数据提供中间结构
 * @author zhangnan
 *
 */
public class CommonFriend {
	private static Integer EMPTY_ID = -999;
	private static Integer SAVE_DATA_LENGTH = 16;	//db中shares字段为定长64Byte
	private Integer userId = 0;		//用户id
	private Integer weight = 0;		//权重
	private Integer sharesCount = 0;	//共同好友数
	private List<Integer> shares = null;	//共同好友
	
	public CommonFriend() {
		this.userId = 0;
		this.weight = 0;
		this.sharesCount = 0;
		shares = new ArrayList<Integer>();
	}

	/**
	 * 依据db返回的数据初始化CommonFriend数据结构
	 * @param item	db返回的数据结构
	 * @return	true表示有数据，false表示空字段
	 */
	public boolean Initialize(ResultSet item) {
		try {
			if (null == item ||
					EMPTY_ID == item.getInt("friend_id")) {		//判断是否是空数据
				return false;
			}
			this.userId = item.getInt("friend_id");
			this.weight = item.getInt("weight");
			
			List<Integer> tmpShares = getIntList(item.getBytes("shares"));	//shares字段存储为定长二进制
			this.sharesCount = tmpShares.get(0);		//开头一个int存储共同好友数
			tmpShares.remove(0);		//删除第一个int
			if (sharesCount >= SAVE_DATA_LENGTH) {		//共同好友数大于等于16个则全部取出
				this.shares = tmpShares;
			} else {			//共同好友数小于16个则按个数取出
				for (int i = 0; i < sharesCount; ++i) {
					this.shares.add(tmpShares.get(i));
				}
			}
		} catch (SQLException e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	/**
	 * 获取推荐用户id
	 * @return
	 */
	public int getUserId() {
		return userId;
	}
	
	/**
	 * 获取该推荐用户的权重
	 * @return
	 */
	public int getWeight() {
		return weight;
	}
	
	/**
	 * 获取共同好友数
	 * @return
	 */
	public int getsharesCount() {
		return sharesCount;
	}
	
	/**
	 * 获取共同好友列表
	 * @return
	 */
	public List<Integer> getShares() {
		return shares;
	}
	
	/**
	 * 将二进制数据转化为int列表
	 * @param data 二进制数据
	 * @return
	 */
	private List<Integer> getIntList(byte[] data) {
		List<Integer> intList = new ArrayList<Integer>();
		int length = data.length;
		for (int i = 0; i < length; i = i + 4) {
			int value = ((data[i + 3] & 0xff) << 24) + ((data[i + 2] & 0xff) << 16)		//java采用补码形式存储byte
					+ ((data[i + 1] & 0xff) << 8) + (data[i] & 0xff);
			intList.add(value);
		}
		return intList;
	}
}
