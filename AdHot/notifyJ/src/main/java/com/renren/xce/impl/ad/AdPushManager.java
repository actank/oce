package com.renren.xce.impl.ad;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Date;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import net.paoding.rose.scanning.context.RoseAppContext;

import org.apache.log4j.Logger;

import xce.ad.HotAdInfo;
import xce.ad.UserHotAdState;

import com.renren.xce.dao.HotPromotionDAO;
import com.renren.xce.dao.PromotionConfig;

import org.quartz.*;
import com.renren.xce.util.WorkQueue;
import com.renren.xce.util.JobScheduler;
import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;

import com.renren.sns.msgcenter.xoa.client.adapter.MessageCenterClientAdapter;

import com.renren.sns.msgcenter.datatype.RequestTypes;

class ReloadTask implements Runnable {
	AdPushManager push_mgr;
    private final Logger logger = Logger.getLogger(AdNotifierI.class.getName());
	
	ReloadTask(AdPushManager push_mgr) {
		this.push_mgr = push_mgr;
	}
	
	@Override
	public void run() {
		logger.info("Start ReloadTask");
		push_mgr.doReload();
		logger.info("Finish ReloadTask");
	}
}

class LoginTask implements Runnable {
	AdPushManager push_mgr;
	int uid;
    private final Logger logger = Logger.getLogger(AdNotifierI.class.getName());
	
    LoginTask(AdPushManager push_mgr, int uid) {
		this.push_mgr = push_mgr;
		this.uid = uid;
	}
	
	@Override
	public void run() {
		logger.info("Start doLogin");
		push_mgr.doLogin(uid);
		
		logger.info("Finish doLogin. stat=" + push_mgr.getState());
	}
}

class ClearDailyPostedTimer implements Job {
    private final Logger logger = Logger.getLogger(AdNotifierI.class.getName());
    
	AdPushManager push_mgr;
	public ClearDailyPostedTimer(AdPushManager mgr) {
		push_mgr = mgr;		
	}
	
	public void execute(JobExecutionContext cntxt) throws JobExecutionException {
		push_mgr.ClearDailyPosted();
		logger.info("ClearDailyPosted");
	}
}

class ClearUserWeeklyReceivedTimer implements Job {
    private final Logger logger = Logger.getLogger(AdNotifierI.class.getName());
    
    static boolean IsNewWeek() {
    	Date date = new Date();
    	if (date.getHours() > 0 ||  date.getMinutes() > 5) {
    		return false;
    	}

    	if(date.getDay() == 1) {
    		return true;
    	}
    	return false;
	}

	public void execute(JobExecutionContext cntxt) throws JobExecutionException {	 
		logger.info("ClearUserWeeklyReceivedTimer");
		if (IsNewWeek()) {
			ReceivedCountCache.getInstance().ClearWeeklyCount();
		}
	}
}

class AdBlockedCache {

    private final static Logger logger = Logger.getLogger(AdNotifierI.class.getName());
    
	private static AdBlockedCache instance = null;
	private static Object instance_lock = new Object();
	
	public static AdBlockedCache getInstance() {
		if (instance == null) {
			synchronized(instance_lock) {
				if(instance == null) { // 双check. 实现上是否靠谱?
					instance = new AdBlockedCache();
				}
			}
		}
		logger.info("create AdBlockedCache instance");
		return instance;
	}
	
	private final ReentrantReadWriteLock readWriteLock = new ReentrantReadWriteLock();
	private final Lock read_lock  = readWriteLock.readLock();
	private final Lock write_lock = readWriteLock.writeLock();

    RoseAppContext rose = new RoseAppContext();
    protected HotPromotionDAO promotion_dao_ = rose.getBean(HotPromotionDAO.class);
    
	private Map<Integer, HashSet<Integer> > blocked_users_ = new HashMap<Integer, HashSet<Integer> >();
	
	// private Object blocked_mutex_ = new Object();
	  
	public void load(int ad_id) {
		  HashSet<Integer> blocked_users = new HashSet<Integer>();

		  logger.info("ad - " + ad_id + " begin dao load");
		  List<Integer> ad_blocked = promotion_dao_.getPromotionBlocked(ad_id);
		  for(int i = 0; i < ad_blocked.size(); ++i) {
		    blocked_users.add(ad_blocked.get(i));
		  }

		  logger.info("ad " + ad_id + " ad_hot_blocked size : " + blocked_users.size());
		  synchronized (this) {
			  blocked_users_.put(ad_id, blocked_users);
		  }
	}
	public void block(int uid, int ad_id) {
		write_lock.lock();
		try {
			Set<Integer> ad_blocked = blocked_users_.get(ad_id);
			ad_blocked.add(uid);
		} finally {
			write_lock.unlock();
		}
	}
	public void unblock(int uid, int ad_id) {
		write_lock.lock();
		try {
			Set<Integer> ad_blocked = blocked_users_.get(ad_id);
			ad_blocked.remove(uid);
		} finally {
			write_lock.unlock();
		}
	}
	public boolean isBlocked(int uid, int ad_id) {
		boolean blocked = true;
		read_lock.lock();
		// TODO : try 的开销有多大?
		try {
			Set<Integer> ad_blocked = blocked_users_.get(ad_id);
			blocked = ad_blocked.contains(uid);
		} finally {
			read_lock.unlock();
		}
		return blocked;
	}
};

class AdReceivedCache {
    private final static Logger logger = Logger.getLogger(AdNotifierI.class.getName());
    
	private static AdReceivedCache instance;
	private static Object instance_lock = new Object();
	public static AdReceivedCache getInstance() {
		logger.info("AdReceivedCache get instance");
		if (instance == null) {
			synchronized(instance_lock) {
				if(instance == null) { // 双check. 实现上是否靠谱?
					instance = new AdReceivedCache();
				}
			}
		}
		logger.info("AdReceivedCache get instance ok");
		return instance;
	}

	private Map<Integer, HashSet<Integer> > received_users_ = new HashMap<Integer, HashSet<Integer> >();
	
	private final ReentrantReadWriteLock readWriteLock = new ReentrantReadWriteLock();
	private final Lock read_lock  = readWriteLock.readLock();
	private final Lock write_lock = readWriteLock.writeLock();

	public void Add(int uid, int ad_id) {			  
		write_lock.lock();
		try {
			HashSet<Integer> ad_blocked = received_users_.get(ad_id);
			if (ad_blocked == null) {
				ad_blocked = new HashSet<Integer>();
				ad_blocked.add(uid);
				received_users_.put(new Integer(ad_id), ad_blocked);
			}
			ad_blocked.add(uid);
		} finally {
			write_lock.unlock();
		}
	}
	
	void Cancel(int uid, int ad_id) {
			write_lock.lock();
			try {
				Set<Integer> ad_blocked = received_users_.get(ad_id);
				if (ad_blocked != null) {
				    ad_blocked.remove(uid);
				}
			} finally {
				write_lock.unlock();
			}
	  }
	  boolean hasReceived(int uid, int ad_id) {
			logger.info("AdReceivedCache check recv. user=" + uid + " ad=" + ad_id);
			boolean ret = true;
			read_lock.lock();
			// TODO : try 的开销有多大?
			try {
				Set<Integer> received = received_users_.get(ad_id);
				if (received != null) {
					ret = received.contains(uid);
				} else {
					ret = false;
				}
			} finally {
				read_lock.unlock();
			}
			logger.info("AdReceivedCache check recv. user=" + uid + " ad=" + ad_id + " finished.");
			return ret;
	  }
}  

class ReceivedCountCache {
	private HashMap<Integer, Integer> weekly_map_ = new HashMap<Integer, Integer>();
	private Object weekly_mutex_ = new Object();	

    private final static Logger logger = Logger.getLogger(AdNotifierI.class.getName());
    

	private static ReceivedCountCache instance;
	private static Object instance_lock = new Object();
	public static ReceivedCountCache getInstance() {
		logger.info("to get instance");
		if (instance == null) {
			synchronized(instance_lock) {
				if(instance == null) { // 双check. 实现上是否靠谱?
					instance = new ReceivedCountCache();
				}
			}
		}
		logger.info("get instance ok");
		return instance;
	}

	private final ReentrantReadWriteLock readWriteLock = new ReentrantReadWriteLock();
	private final Lock read_lock  = readWriteLock.readLock();
	private final Lock write_lock = readWriteLock.writeLock();
	
	public void IncWeeklyCount(int uid) {
		write_lock.lock();
		try {
			Integer count = weekly_map_.get(uid);			
		    if (count == null) {
		      weekly_map_.put(uid, 1);
		    } else {
		      ++count;
		    }
		} finally {
			write_lock.unlock();
		}
	}
	  
	void DecWeeklyCount(int uid) {
		write_lock.lock();
		try {
			Integer count = weekly_map_.get(uid);			
		    if (count != null) {
		    	--count;
		    }
		} finally {
			write_lock.unlock();
		}
	}

	  int GetWeeklyCount(int uid) {
		  Integer count = 0;
		  read_lock.lock();
			try {
				count = weekly_map_.get(uid);
			} finally {
				read_lock.unlock();
			}
			
			if (count == null) {
				return 0;
			}
	      return count;
	  }
	  
	  void SetWeeklyCount(int uid, int count) {
			write_lock.lock();
			try {
				weekly_map_.put(uid, count);
			} finally {
				write_lock.unlock();
			}
		  }

		  void ClearWeeklyCount() {
			write_lock.lock();
			try {
				weekly_map_ = new HashMap<Integer, Integer>();
			} finally {
				write_lock.unlock();
			}
		  }
}

//记录各个 ad 所有未读的用户, 同步 home hot count.
class AdUnreadCache {

	private static AdUnreadCache instance;
	private static Object instance_lock = new Object();
	public static AdUnreadCache getInstance() {
		if (instance == null) {
			synchronized(instance_lock) {
				if(instance == null) { // 双check. 实现上是否靠谱?
					instance = new AdUnreadCache();
				}
			}
		}
		return instance;
	}

	private HashMap<Integer, HashSet<Integer> > unread_users_ = new HashMap<Integer, HashSet<Integer> >();
	
	private final ReentrantReadWriteLock readWriteLock = new ReentrantReadWriteLock();
	private final Lock read_lock  = readWriteLock.readLock();
	private final Lock write_lock = readWriteLock.writeLock();

    private final Logger logger = Logger.getLogger(AdNotifierI.class.getName());

	public static final int kMaxHomeHotCount = 2;
  public void MarkRead(int uid, int ad_id) {
		write_lock.lock();
		try {
		    Set<Integer> users = unread_users_.get(ad_id);
		    if (users != null) {
		        users.remove(uid);
		    }
		} finally {
			write_lock.unlock();
		}
	  
		int count = GetUnreadCount(uid);
		if (count < kMaxHomeHotCount) {
		    UpdateHomeHotCount(uid, count);
		}
  }

  void AddUnread(int uid, int ad_id) {
		write_lock.lock();
		try {
		    Set<Integer> users = unread_users_.get(ad_id);
		    if (users == null) {
		    	HashSet<Integer> t = new HashSet<Integer>();
		    	t.add(uid);
		    	unread_users_.put(uid, t);
		    } else {
		    	users.add(uid);
		    }
		} finally {
			write_lock.unlock();
		}

		  int count = GetUnreadCount(uid);
		  if (count <= kMaxHomeHotCount) {
		    UpdateHomeHotCount(uid, count);
		}
    }

  int GetUnreadCount(int uid) {

	  int count = 0;
		read_lock.lock();
		try {
		  count = GetUnreadCountNoLock(uid);
		} finally {
			read_lock.unlock();
		}
		return count;
  }
  void RemoveAd(int ad_id) {
	read_lock.lock();
	try {
	    Set<Integer> users = unread_users_.get(ad_id);
	    if (users == null) {
		      logger.info("ad " + ad_id + " no unread user.");
		  	   read_lock.unlock();
		      return;
		 }
	    int total_unread = users.size();
	    logger.info("ad " + ad_id + " total unread count " + total_unread);
		int erased_count = 0;
		Iterator<Integer> it = users.iterator();
		while (it.hasNext()) {		      
		     // 将 AdReceivedCache 清空, 未读就认为未发送
			int uid = it.next();
		    AdReceivedCache.getInstance().Cancel(uid, ad_id);
		    UpdateHomeHotCount(uid, GetUnreadCountNoLock(uid) - 1);
		    
		    //  AdHotMessageClient::instance().Set(uid, ad_id, "");
		      // 将 ReceivedCountCache 清空, 未读就认为未发送
		      ReceivedCountCache.getInstance().DecWeeklyCount(uid);
		      logger.info("ad " + ad_id + " remove unread user " + uid + " - " + ++erased_count + "/" + total_unread);
		    }
	} finally {
		read_lock.unlock();
	}

	read_lock.lock();
	try {
		unread_users_.remove(ad_id);
		logger.info("ad " + ad_id + " all unread user erased.");
	} finally {
		read_lock.unlock();
	}
  }

  int GetUnreadCountNoLock(int uid) {
	  int count = 0;
	  
	  for (Map.Entry<Integer, HashSet<Integer> > item : unread_users_.entrySet()) {
		  HashSet<Integer> users = item.getValue();		  
	     if (users.contains(uid)) {
	         ++count;
	      }
	  }
	  return count;
  }
  
  public void UpdateHomeHotCount(int uid, int count) {
	 // try {
	//	    UserCountAdapter::instance().set(uid, CADNOTIFY, count);
	//	  } catch (Ice::Exception& e) {
	//	    MCE_WARN("UserCountAdapter.set() error " << e);
	//	  } catch (std::exception& e) {
	//	    MCE_WARN("UserCountAdapter.set() error " << e.what());
	//	  }
	  logger.info("uid " + uid + " set home hot count = " + count);
  }  
}


public class AdPushManager {
    private final Logger logger = Logger.getLogger(AdNotifierI.class.getName());
    
    private WorkQueue task_mgr_ = new WorkQueue(10);

    // @Autowired
    RoseAppContext rose = new RoseAppContext();
    protected HotPromotionDAO promotion_dao_ = rose.getBean(HotPromotionDAO.class);
    
	enum State{READY, LOADING, REMOVING};
	private State state_;  // TODO:代码规范?

	private Object ad_config_lock = new Object();
	Map<Integer, PromotionConfig> ad_config_map_; // = new HashMap<Integer, AdConfig>();
	
	
	//TODO : java有构造函数初始化列表吗
	AdPushManager() {
		state_ = State.READY; 
	}
	
	public boolean init() {
		startReload();
		/*
		JobScheduler.getInstance().Schedule("reset ad daily posted",
				ClearDailyPostedTimer.class, "3 0 * ? * *");

		JobScheduler.getInstance().Schedule("reset user weekly received",
				ClearUserWeeklyReceivedTimer.class, "* * * ? * *");
		*/	
		return true;
	}
	
	boolean startReload() {
		logger.info("CacheManager::Init()");
		if (state_ != State.READY) {
		    logger.warn("please reload later");
		    return false;
		}
		state_ = State.LOADING;
		
		task_mgr_.execute(new ReloadTask(this));
		return true;		
	}
	
	void doReload() {
		logger.info("Enter doReload 1");
		System.out.println("do reload 1");
		if (promotion_dao_ == null) {
			logger.info("Enter doReload null");
			state_ = State.READY;
			return;
		}
		System.out.println("do reload x");

		List<Integer> promotion_list = promotion_dao_.getPromotionIds();
		logger.info("Enter doReload 2");
		
		Iterator<Integer> it = promotion_list.iterator();
		Map<Integer, PromotionConfig> ad_config_map = new HashMap<Integer, PromotionConfig>(); // 加载广告信息

		logger.info("doReload. hot ad count = " + promotion_list.size());
		while(it.hasNext()) {
			Integer id = it.next();
		    PromotionConfig config = LoadOneAd(id);
		    if(config != null) {
		      ad_config_map.put(id, config);
			    logger.info("Load ad " + id + " endtime=" + config.getEndTime());
		    } else {
			    logger.info("Load ad " + id + " error!");		    	
		    }
		    logger.info("Load ad pass " + id);		
		}

		synchronized (ad_config_lock) {
		    logger.info("swap new config map");
			ad_config_map_ = ad_config_map;
		}
		state_ = State.READY;
	    logger.info("set load state to " + state_);	
	}

	public void Login(int uid) {
		task_mgr_.execute(new LoginTask(this, uid));
	}

	public static final int kMaxWeeklyCount = 2;
	public static final int kMaxHomeHotCount = 2;
	
	public void doLogin(int uid) {
		if (state_ != State.READY) {
		    logger.info("login. uid " + uid + " : system is not ready " + state_);
		    return;
		}

		// TODO : 检查本周给用户发送的次数 
		int weekly_left = kMaxWeeklyCount - ReceivedCountCache.getInstance().GetWeeklyCount(uid);
		if (weekly_left <= 0) {
		    logger.info("login. uid " + uid + " : reached weekly limit " + kMaxWeeklyCount);
		    return;
		}

		// TODO : 检查剩余未读的条数
		int unread_left = kMaxHomeHotCount - AdUnreadCache.getInstance().GetUnreadCount(uid);
		if (unread_left <= 0) {
			logger.info("login. uid " + uid + " : reached max unread count " + kMaxHomeHotCount);
		    return;
		}
		
		int quota_left = weekly_left < unread_left ? weekly_left : unread_left; // 剩余的配额数

		logger.info("do login 3 " + uid);
		Map<Integer, List<Integer> > to_post = GetToPostAds(uid, quota_left);
		
		for(Map.Entry<Integer, List<Integer> > item : to_post.entrySet()) {
			List<Integer> buddies = item.getValue();
/*
	    	MessageCenterClientAdapter.addMessage(RequestTypes.notify, 
	    			"hotadnotify",
	    			1, 
	    			240599893, 
	    			"{\"content\":\"hello message center\", \"iconUrl\":\"http://xnimg.cn/imgpro/icons/reply.gif\", \"time\":\"2011-08-30\"}");
*/
			// buddy info 也是需要的
			
			Integer ad = item.getKey();
			
		    logger.info("login. uid " + uid + " receiving message ad=" + ad);
		    AdReceivedCache.getInstance().Add(uid, ad); 
		    AdUnreadCache.getInstance().AddUnread(uid, ad); 
		    ReceivedCountCache.getInstance().IncWeeklyCount(uid);
		}
	}
	
	public PromotionConfig LoadOneAd(int ad) {
		// 加载广告的block列表
		logger.info("Start load blocked users. ad=" + ad);
		AdBlockedCache.getInstance().load(ad);
		logger.info("Finish load blocked users. ad=" + ad);

		PromotionConfig cfg = promotion_dao_.getPromotionConfig(ad);
		
		if (cfg != null) {
			List<Integer> partners = promotion_dao_.getPromotionPartners(ad);
			cfg.setPartners(partners);
			logger.warn("ad " + ad + " partners count = " + partners.size());
		}
		return cfg;
	}
	
	public void MarkReadByUid(int uid) {
		// 设为已读
	}
	
	public void Block(int uid, int ad) { // 用户uid屏蔽ad
		AdBlockedCache.getInstance().block(uid, ad);
		promotion_dao_.insertBlocked(uid, ad);
	}

	public void ClearDailyPosted() {
		synchronized (ad_config_lock) {
			for (Map.Entry<Integer, PromotionConfig> ad : ad_config_map_.entrySet()) {
				PromotionConfig cfg = ad.getValue();
				cfg.resetDailyPosted();
		  }
		}
	}

	Map<Integer, List<Integer> > GetToPostAds(int uid, int count) {
		Map<Integer, PromotionConfig> ad_config_map = null;
		synchronized (ad_config_lock) {
		    logger.info("swap new config map");
			ad_config_map = ad_config_map_;
		}
		
		List<Integer> buddies = null;
		Map<Integer, List<Integer> > to_post = new HashMap<Integer, List<Integer> >();
		
		for(Map.Entry<Integer, PromotionConfig> item : ad_config_map.entrySet()) {
		    PromotionConfig cfg = item.getValue();		    
		
			int ad_id = cfg.getId();
			
			// 是活动参与者
			if (cfg.isPartner(uid)) {
			   logger.info("uid " + uid + " has participated ad " + ad_id);
			   continue;
			}

		    logger.info("uid " + uid + " check block ad " + ad_id);
			// 已经block这个活动
			if (AdBlockedCache.getInstance().isBlocked(uid, ad_id)) {
			    logger.info("uid " + uid + " has blocked ad " + ad_id);
			    continue;
			}

		    logger.info("uid " + uid + " check received ad " + ad_id + " earlier");
			// 已经接收过这个活动
			if (AdReceivedCache.getInstance().hasReceived(uid, ad_id)) {
			    logger.info("uid " + uid + " has received ad " + ad_id + " earlier");
			    // continue;
			}

		    logger.info("uid " + uid + " get buddy list");
		    if (buddies == null) {		        
		        buddies = BuddyByIdCacheAdapter.getInstance().getFriendListAsc(uid, 5000);
			     logger.info("uid " + uid + " has " + buddies.size() + " friends");
		     }
		    logger.info("uid " + uid + " get buddy list finish");
		    if (buddies == null || buddies.isEmpty()) {
		        return to_post;
		     }
		    
		    // 查找参加过该活动的好友
		    List<Integer> active_buddies = new ArrayList<Integer>();
		    for(Integer buddy : buddies) {
		        if (cfg.isPartner(buddy)) {
			        active_buddies.add(buddy);
			        if (active_buddies.size() >= 40) {
			          break;
			         }
			     }		        
		    }

		    logger.info("uid " + uid + " has " + active_buddies.size() + "/" + cfg.getMinPartners() + " friends participated ad " + ad_id);

		    // 好友中没有足够多参加该活动的
		    if (active_buddies.size() < cfg.getMinPartners()) {
		      continue;
		    }

		    if (cfg.UnderDailyLimit()) {
		      cfg.IncDailyPosted();  // 更新日推送计数
		      to_post.put(ad_id, active_buddies);
		    }
	    }

		return to_post;
	}

	public HotAdInfo[] GetAdInfoSeq() {
		return new HotAdInfo[0];		
	}
	
	void RemoveExpired() {
		if (state_ != State.READY) {
		    logger.info("please RemoveExpired() later");
		    return;
		}

		int del_ad = -1;

		Map<Integer, PromotionConfig> ad_config_map = null;
		synchronized (ad_config_lock) {
		    logger.info("swap new config map");
			ad_config_map = ad_config_map_;
		}
		
		for(Map.Entry<Integer, PromotionConfig> ad : ad_config_map.entrySet()) {
			PromotionConfig ad_cfg = ad.getValue();
			if (ad_cfg.Expired()) {
			        del_ad = ad.getKey();
			        logger.info("ad " + del_ad + " has expired");
			        break; // 如果有其他的需要下线，交给下次扫描
			}
		}

		if (del_ad >= 0) {
		    DelAd(del_ad);
		}
	}
	
	void DelAd(int ad_id) {
		if (state_ != State.READY) {
		    logger.info("please remove ad " + ad_id + " later.");
		    return;
		}
		state_ = State.REMOVING;
		synchronized (ad_config_lock) {
		    ad_config_map_.remove(ad_id);
		}		

		logger.info("remove ad " + ad_id + " in memory");
	    AdUnreadCache.getInstance().RemoveAd(ad_id);

	    state_ = State.READY;
	}


	void Unblock(int uid, int ad_id) {
	}

	boolean ResetUserAd(Integer uid, Integer ad_id) {
		  logger.info("ResetUserAd() uid=" + uid + " ad=" + ad_id);
		  ReceivedCountCache.getInstance().SetWeeklyCount(uid, 0);

			PromotionConfig ad_config = null;
			synchronized (ad_config_lock) {
			    logger.info("swap new config map");
				ad_config = ad_config_map_.get(ad_id);
			}			

		  if (ad_config != null) {
				synchronized (ad_config_lock) {
					ad_config.removePartner(uid);
				}			
		  }

		  AdUnreadCache.getInstance().MarkRead(uid, ad_id);
		  AdBlockedCache.getInstance().unblock(uid, ad_id);
		  AdReceivedCache.getInstance().Cancel(uid, ad_id);

		return false;
	}
	
	UserHotAdState GetUserAdState(Integer uid, Integer ad_id) {
		UserHotAdState user_state = new UserHotAdState();
		user_state.uid = uid;
		user_state.adId = ad_id;

		user_state.weeklyReceived = ReceivedCountCache.getInstance().GetWeeklyCount(uid);
		user_state.unreadCount = AdUnreadCache.getInstance().GetUnreadCount(uid);
		
		user_state.hasBlocked = AdBlockedCache.getInstance().isBlocked(uid, ad_id);
		user_state.hasReceived = AdReceivedCache.getInstance().hasReceived(uid, ad_id);
		

		PromotionConfig ad_config = null;
		synchronized (ad_config_lock) {
		    logger.info("swap new config map");
			ad_config = ad_config_map_.get(ad_id);
		}			

	  if (ad_config != null) {
		    List<Integer> buddies = BuddyByIdCacheAdapter.getInstance().getFriendListAsc(uid, 5000);
		    List<Integer> partner_buddies = new ArrayList<Integer>();
		    for (int i = 0; i < buddies.size(); ++i) {
		      if (ad_config.isPartner(buddies.get(i))) {
		        partner_buddies.add(buddies.get(i));
		        if (partner_buddies.size() >= 40) {
		          break;
		        }
		      }
		    }
		    user_state.partnerBuddies = partner_buddies.size();
		    user_state.isPartner = ad_config.isPartner(uid);
	   }

		return user_state;
	}
	public void setState(State s) {
		state_ = s;
	}
	public State getState() {
		return state_;
	}    
}
