package com.xiaonei.xce.socialgraph.ffucr.impl;

import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.SocialGraphAdapter;
import com.xiaonei.xce.socialgraph.SocialGraphBase;
import com.xiaonei.xce.socialgraph.ffucr.FriendClusterRecommendAdapter;
import com.xiaonei.xce.socialgraph.ffucr.Recommend;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

public class FriendClusterRecommendAdapterImpl extends SocialGraphBase implements
        FriendClusterRecommendAdapter {

    private static final String BIZ_TYPE = "FFUCR";

    private static FriendClusterRecommendAdapter instance = null;

    private FriendClusterRecommendAdapterImpl() {

    }

    public static FriendClusterRecommendAdapter getInstance() {
        if (instance == null) {
            synchronized (FriendClusterRecommendAdapterImpl.class) {
                if (instance == null) {
                    try {
                        instance = new FriendClusterRecommendAdapterImpl();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return instance;
    }

    @Override
    public List<Recommend> getFFUCR(int userId) {
        List<Recommend> ret = new ArrayList<Recommend>();
        byte[] bytes = getData(BIZ_TYPE, userId);
        if (bytes != null && bytes.length != 0) {
            Items items;
            try {
                items = Items.parseFrom(bytes);
                for (int i = 0; i < items.getItemsCount(); ++i) {
                    Item item = items.getItems(i);
                    Recommend recommend = new Recommend();
                    recommend.setId(item.getId());
                    recommend.setCommonFriendCount(item.getField());
                    for (Integer cf : item.getFieldsList()) {
                        recommend.addCommonFriend(cf);
                    }
                    ret.add(recommend);
                }
            } catch (InvalidProtocolBufferException e) {
                e.printStackTrace();
            }

        }
        return ret;
    }

    public static void main(String[] args) {
        List<Recommend> list = SocialGraphAdapter.getFriendClusterRecommendAdapter().getFFUCR(
                286162347);
        for (Recommend r : list) {
            System.out.println(r.getId() + "\t" + r.getCommonFriendCount() + "\t"
                    + r.getCommonFriends());
        }
        System.exit(0);
    }
}
