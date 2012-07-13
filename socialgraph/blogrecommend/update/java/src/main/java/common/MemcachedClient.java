package common;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

import xce.distcache.DistCacheAdapter;

/**
 * socialgraphd各种业务对distcache使用的封装
 * 
 * @author zhangnan(zhangnan@opi-corp.com)
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class MemcachedClient implements Observer {

    private static final String BIZ_PATH = "/BIZ";

    private static MemcachedClient instance = null;

    private Object lock = new Object();

    private Map<String, BizType> bizMap = new HashMap<String, MemcachedClient.BizType>();

    private MemcachedClient() {
        ZookeeperClient.getInstance().attach(this);
        Init();
    }

    public static MemcachedClient getInstance() {
        if (null == instance) {
            synchronized (MemcachedClient.class) {
                if (null == instance) {
                    try {
                        instance = new MemcachedClient();
                    } catch (Exception e) {
                        e.printStackTrace();
                        instance = null;
                    }
                }
            }
        }
        return instance;
    }

    /**
     * 实现父类update函数
     * 
     * @parameter key zk路径 /BIZ
     * @parameter value zk路径中的数据
     */
    public void update(String key, String value) {
    	System.out.println("update key:" + key + " value:" + value);
        if (BIZ_PATH.equals(key)) {
            Map<String, BizType> target = new HashMap<String, MemcachedClient.BizType>();
            String[] bizList = value.split("#");
            for (String item : bizList) {
                String[] biz = item.split(",");
                String bizName = biz[0];
                int bizType = 0;
                try {
                    bizType = Integer.parseInt(biz[1]);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                if (2 == biz.length) {
                    int loop = 0;
                    DistCacheAdapter adapter = null;
                    while (null == adapter) {
                        try {
                            adapter = DistCacheAdapter.connect(bizName);
                        } catch (Exception e) {
                            adapter = null;
                        }
                        if (null == adapter) {
                            try {
                                TimeUnit.MICROSECONDS.sleep(50);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        if (++loop > 10) {
                            break;
                        }
                    }
                    if (null != adapter) {
                        BizType bizItem = new BizType(bizName, bizType, adapter);
                        target.put(bizName, bizItem);
                    }
                }
            }
            synchronized (lock) {
            	System.out.println("value:" + value + " targetsize:" + target.size());
            	bizMap.clear();
            	bizMap.putAll(target);
            }
        }
    }

    /**
     * 向distcache中插入数据
     * 
     * @param type
     * @param userId
     * @param data
     * @return
     */
    public boolean setByDistCache(String type, int userId, byte[] data) {
        boolean result = true;
        synchronized (lock) {
            if (bizMap.containsKey(type)) {
                BizType bizType = bizMap.get(type);
                long key = bizType.getType();
                key = (key << 32) + userId;
                DistCacheAdapter adapter = bizType.getDistcacheAdapter();
                try {
                    adapter.set(key, data);
                } catch (Exception e) {
                    e.printStackTrace();
                    result = false;
                }
            } else {
            		Init();   //失败后再次初始化
            }
        }
        return result;
    }

    /**
     * 从distcache中取数据
     * 
     * @parameter mckey 取出数据的key
     */
    public byte[] getByDistCache(String bizName, int userId) {
        byte[] data = new byte[0];
        synchronized (lock) {
            if (bizMap.containsKey(bizName)) {
                BizType bizType = bizMap.get(bizName);
                long key = bizType.getType();
                key = (key << 32) + userId;
                DistCacheAdapter adapter = bizType.getDistcacheAdapter();
                try {
                    data = adapter.get(key);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            } else {
            		Init();   //失败后再次初始化
            }
        }
        return data;
    }
    
    /**
     * 向distcache中插入数据
     * @param type
     * @param id	为long值，不必加入前缀
     * @param data
     * @return
     */
    public boolean set2ByDistCache(String type, long id, byte[] data) {
        boolean result = true;
        synchronized (lock) {
            if (bizMap.containsKey(type)) {
                BizType bizType = bizMap.get(type);
                DistCacheAdapter adapter = bizType.getDistcacheAdapter();
                try {
                	System.out.println("type:" + type + " id:" + id + " data length:" + data.length);
                  adapter.set(id, data);
                } catch (Exception e) {
                  e.printStackTrace();
                  result = false;
                }
            } else {
            		Init();   //失败后再次初始化
            }
        }
        return result;
    }
    
    /**
     * 从distcache中取数据
     * @param bizName	业务名
     * @param id	long值，不用加前缀
     * @return
     */
    public byte[] get2ByDistCache(String bizName, long id) {
      byte[] data = new byte[0];
      synchronized (lock) {
          if (bizMap.containsKey(bizName)) {
              BizType bizType = bizMap.get(bizName);
              DistCacheAdapter adapter = bizType.getDistcacheAdapter();
              try {
                  data = adapter.get(id);
              } catch (Exception e) {
                  e.printStackTrace();
              }
          } else {
          		Init();   //失败后再次初始化
          }
      }
      return data;
  }

    /**
     * 初始化函数，如不成功初始化10次
     */
    private void Init() {
    	int initCount = 10;
      try {
      	for (int i = 0; i < initCount; ++i) {
      		String value = ZookeeperClient.getInstance().getValue(BIZ_PATH);
          if (null != value) {
          	update(BIZ_PATH, value);
            break;
          }
//          Thread.sleep(2000);
      	}
      } catch (Exception e) {
      	e.printStackTrace();
      }
    }

    /**
     * 转化zk中的信息和distcacheadapter
     */
    private static class BizType {

        private String biz;

        private int type;

        private DistCacheAdapter distcacheAdapter;

        public BizType(String biz, int type, DistCacheAdapter adapter) {
            this.biz = biz;
            this.type = type;
            this.distcacheAdapter = adapter;
        }
        
        public String getBiz() {
            return biz;
        }

        public int getType() {
            return type;
        }

        public DistCacheAdapter getDistcacheAdapter() {
            return distcacheAdapter;
        }

    }
}
