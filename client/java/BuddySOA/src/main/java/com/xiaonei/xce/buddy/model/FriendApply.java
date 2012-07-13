package com.xiaonei.xce.buddy.model;

/**
 * 发送好友申请时,使用者需要构造的对象
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class FriendApply {

    private xce.buddy.FriendApply fa;

    public FriendApply(int applicant, int accepter, String content, String from) {
        fa = new xce.buddy.FriendApply();
        fa.applicant = applicant;
        fa.accepter = accepter;
        fa.content = content;
        fa.from = from;
        fa.timestamp = System.currentTimeMillis();
        fa.ip = 0;
    }

    public xce.buddy.FriendApply getRpcData() {
        return fa;
    }

    public int getApplicant() {
        return fa.applicant;
    }

    public int getAccepter() {
        return fa.accepter;
    }

    public String getContent() {
        return fa.content;
    }

    public void setContent(String content) {
        fa.content = content;
    }

    public void setIp(String ipAddress) {
        // TODO : 把ipAddress转换成int
    }

}
