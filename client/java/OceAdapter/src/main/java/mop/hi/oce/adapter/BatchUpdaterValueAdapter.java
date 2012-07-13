package mop.hi.oce.adapter;

public interface BatchUpdaterValueAdapter {
    void incTribePageViewCount(int tribeId);
    void incFlyerUserAdCount(int userId, int adId);
    
    void incTribePageViewCount(long tribeId);
    void incFlyerUserAdCount(int userId, long adId);

}
