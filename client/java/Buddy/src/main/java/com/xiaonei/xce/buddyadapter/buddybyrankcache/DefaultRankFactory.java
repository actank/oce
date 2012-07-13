package com.xiaonei.xce.buddyadapter.buddybyrankcache;


public class DefaultRankFactory implements RankFactory<DefaultRank>{

    @Override
    public DefaultRank create() {
        return new DefaultRank();
    }

}
