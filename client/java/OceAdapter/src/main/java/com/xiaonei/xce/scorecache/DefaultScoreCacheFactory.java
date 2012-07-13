package com.xiaonei.xce.scorecache;

public class DefaultScoreCacheFactory implements ScoreCacheFactory<DefaultScoreCache> {

	@Override
	public DefaultScoreCache createScoreCache() {
		return new DefaultScoreCache();
	}
	
}