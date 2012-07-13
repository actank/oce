package com.renren.xce.socialgraph.block;

import java.util.List;

import xce.socialgraph.recommend.DbUtil;

import com.renren.xce.socialgraph.common.BlockFetcher;

/**
 * fetch block list from table socialgraph_table
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class SgBlockFetcher implements BlockFetcher {
	public List<Long> getBlockList(int hostId) {
		List<Long> blockList = DbUtil.getInstance().getSgBlockList(hostId);
		return blockList;
	}
}
