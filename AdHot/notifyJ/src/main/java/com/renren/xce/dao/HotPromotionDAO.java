package com.renren.xce.dao;

import java.util.List;

import net.paoding.rose.jade.annotation.DAO;
import net.paoding.rose.jade.annotation.SQL;
import net.paoding.rose.jade.annotation.SQLParam;


import com.renren.xce.dao.PromotionConfig;

@DAO(catalog = "ad_db")
// @DAO(catalog = "im")
public interface HotPromotionDAO {
    // @SQL("SELECT id from ad_hot_promotion where start_time < now() and end_time > now()")
    @SQL("SELECT id from ad_hot_promotion where id = 33")
    public List<Integer> getPromotionIds();

    // @SQL("SELECT id, level, start_time, end_time, min_partners, max_daily_post from ad_hot_promotion where id = :1")
    // @SQL("SELECT id, level, start_time, end_time, min_partners, max_daily_post from ad_hot_promotion where id = :1")
    @SQL("SELECT id, level, start_time, end_time, min_partners, max_daily_post from ad_hot_promotion where id = :1")
    public PromotionConfig getPromotionConfig(int id);
    
    // @SQL("SELECT uid from ad_hot_partner where promotion_id = :1 limit 1000000")
    @SQL("SELECT uid from ad_hot_partner where promotion_id = :1 limit 1000000")
    public List<Integer> getPromotionPartners(int ad_id);
    
    @SQL("SELECT uid from ad_hot_blocked where promotion_id = :1 limit 1000000")
    public List<Integer> getPromotionBlocked(int ad_id);

    @SQL("INSERT IGNORE INTO ad_hot_blocked(uid,promotion_id) VALUES (:1, :2)")
    public int insertBlocked(int uid, int ad_id);
    
    // @SQL("SELECT local_id FROM hotshare_video_id WHERE local_id>:local_id limit :size")
    // public List<Integer> getIDInfo(@SQLParam(value="local_id")long arg0 , @SQLParam(value="size")int arg1);
    
    // @SQL("SELECT id,  user_id , share_id, user_name, title, head_url, thumb_url,url, creation_date FROM hot_share WHERE to_days(now())-to_days(creation_date)<=:day and stage=0 and type=:type limit :size")
    // public List<HotShare> getHotShareEmpty(@SQLParam(value="day")int arg0 , @SQLParam(value="type")int arg1 ,@SQLParam(value="size")int arg2);
}
