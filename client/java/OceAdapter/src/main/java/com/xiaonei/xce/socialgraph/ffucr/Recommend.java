package com.xiaonei.xce.socialgraph.ffucr;

import java.util.ArrayList;
import java.util.List;

public class Recommend {

    private int id;

    private int commonFriendCount;

    private List<Integer> commonFriends;

    public Recommend() {
        commonFriendCount = 0;
        commonFriends = new ArrayList<Integer>();
    }

    public Recommend(int id, int count, List<Integer> commonFriends) {
        this.id = id;
        this.commonFriendCount = count;
        if (commonFriends != null && commonFriends.size() != 0) {
            this.commonFriends = commonFriends;
        } else {
            commonFriends = new ArrayList<Integer>();
        }
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getCommonFriendCount() {
        return commonFriendCount;
    }

    public void setCommonFriendCount(int commonFriendCount) {
        this.commonFriendCount = commonFriendCount;
    }

    public List<Integer> getCommonFriends() {
        return commonFriends;
    }

    public boolean addCommonFriend(int commonFriendId) {
        return commonFriends.add(commonFriendId);
    }
}
