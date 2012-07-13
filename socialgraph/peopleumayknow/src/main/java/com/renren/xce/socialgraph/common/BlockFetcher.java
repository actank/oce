package com.renren.xce.socialgraph.common;

import java.util.List;

/**
 * interface of getting block list
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public interface BlockFetcher {
	public List<Long> getBlockList(int hostId); 
}
