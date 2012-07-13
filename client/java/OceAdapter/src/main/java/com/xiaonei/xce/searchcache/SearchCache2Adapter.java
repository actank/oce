package com.xiaonei.xce.searchcache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Queue;
import java.util.Map.Entry;

import com.xiaonei.xce.log.Oce;

import Ice.ObjectPrx;
import xce.searchcache.SearchCacheManagerPrx;
import xce.searchcache.SearchCacheManagerPrxHelper;
import xce.searchcache.SearchCacheResult;
import xce.searchcache.UpdateUnit;
import xce.util.channel.Channel;
import xce.util.service.ReplicatedServiceAdapter;
import xce.util.tools.MathUtil;
import xce.util.task.Task;
import xce.util.task.TaskManager;

public class SearchCache2Adapter extends ReplicatedServiceAdapter {
	private static final String endpoints = "@SearchCacheN";
	
    private static SearchCache2Adapter _instance = new SearchCache2Adapter();

    private InvokeLoadThread _invokeLoad = new InvokeLoadThread();
    private InvokeUpdateThread _invokeUpdate = new InvokeUpdateThread();

	private SearchCache2Adapter() {
		super(endpoints, 4, Channel.newSearchCacheChannel(endpoints),"XiaoNeiSearchCache/Query");
        _invokeLoad.start();
        _invokeUpdate.start();
	}

    public static SearchCache2Adapter getInstance() {
        return _instance;
    }

	@Override
	protected boolean verifyProxy(ObjectPrx proxy) {
		return SearchCacheManagerPrxHelper.uncheckedCast(proxy).isValid();
	}

    class LoadTaskThread extends Task {
        private Vector<Integer> _userIds;
        public LoadTaskThread(Vector<Integer> userIds) {
            _userIds = userIds;
        }
        public void run() {
            doLoad(_userIds);
        }
    }

  class UpdateTaskThread extends Task {
        private Vector<UpdateUnit> _updateUnits;
        public UpdateTaskThread(Vector<UpdateUnit> updateUnits) {
            _updateUnits = updateUnits;
        }
        public void run() {
          //System.out.println("step in UpdateTaskThread run");
            doUpdate(_updateUnits);
          //System.out.println("step out UpdateTaskThread run");
        }
    }


    class InvokeLoadThread  extends Thread
    {
        private Vector<Integer> _userIds = new Vector<Integer>();
        private int _maxNum = 200; 
        private int _timeout = 1000;

        public void load(int userId) {
            synchronized(_userIds) {
                _userIds.add(userId);
                if (_userIds.size() > _maxNum) {
                    _userIds.notifyAll();
                }
            }
        }

        public void load(Vector<Integer> userIds) {
          synchronized(_userIds) {
            _userIds.addAll(userIds);
            if (_userIds.size() > _maxNum) {
              _userIds.notifyAll();
            }
          }
        }

        public void run() {
            while (true) {
                synchronized(_userIds) {
                    try {
                        if (_userIds.size() < _maxNum)
                            _userIds.wait(_timeout);

                        Vector<Integer> userIds = new Vector<Integer>();
                        userIds.addAll(_userIds);
                        _userIds.clear();
                        TaskManager.getInstance().push(new LoadTaskThread(userIds));
                    }
                    catch(InterruptedException e){
                        Oce.getLogger().error(this.getClass().getName() + " " + e);
                    }
                }
            }
        }
    }

    class InvokeUpdateThread  extends Thread
    {
        private Vector<UpdateUnit> _updateUnits = new Vector<UpdateUnit>();
        private int _maxNum = 200; 
        private int _timeout = 1000;

        public void update(int userId, String table, Map<String, String>fieldPairMap) {
          //System.out.println("step in InvokeUpdateThread update");

            synchronized(_updateUnits) {
                UpdateUnit updateUnit = new UpdateUnit();
                updateUnit.id = userId;
                updateUnit.table = table;
                updateUnit.fieldPairMap = new HashMap<String, String>();
                for(Map.Entry<String, String> m : fieldPairMap.entrySet())
                {
                    updateUnit.fieldPairMap.put(m.getKey(), m.getValue());
                }

                _updateUnits.add(updateUnit);
                if (_updateUnits.size() > _maxNum) {
                  _updateUnits.notifyAll();
                }
                //System.out.println("step out InvokeUpdateThread update");
            }
        }

     
        public void run() {
          //System.out.println("step in InvokeUpdateThread run");
            while (true) {
              //System.out.println("enter while cycle...");
                synchronized(_updateUnits) {
                    try {
                        if (_updateUnits.size() < _maxNum)
                            _updateUnits.wait(_timeout);

                        Vector<UpdateUnit> updateUnits = new Vector<UpdateUnit>();
                        updateUnits.addAll(_updateUnits);
                        _updateUnits.clear();
                        TaskManager.getInstance().push(new UpdateTaskThread(updateUnits));
                        //System.out.println("begin run UpdateTaskThread...");
                    }
                    catch(InterruptedException e){
                        Oce.getLogger().error(this.getClass().getName() + " " + e);
                    }
                }
            }
        }
    }


    public void load(int userId) {
        _invokeLoad.load(userId);
    }

    public void update(int userId, String table, Map<String, String> fieldPairMap)
    {
        _invokeUpdate.update(userId, table, fieldPairMap);
    }

    public void load(Vector<Integer> userIds) {
        _invokeLoad.load(userIds);
    }

    private void doLoad(Vector<Integer> userIds) {
	    Vector<SearchCacheManagerPrx> managers = new Vector<SearchCacheManagerPrx>();

        if (userIds.size() <= 0) {
            return;
        }
        
        int[] ids = new int[userIds.size()];
        {
            int i = 0;
            for(Integer id : userIds) {
                ids[i++] = id;
            }
        }

        for(int i=0; i<cluster(); i++)
        {
          List<ObjectPrx> resultList = locateAllProxy(managers,"M", i ,Channel.Invoke.Twoway,SearchCacheManagerPrxHelper.class,2000);
          for (int j=0; j<resultList.size(); j++) {
            SearchCacheManagerPrx prx = SearchCacheManagerPrxHelper.uncheckedCast(resultList.get(j));
            prx.load(ids);
          }
        }

    }        

    private void doUpdate(Vector<UpdateUnit> updateUnits) {
	    Vector<SearchCacheManagerPrx> managers = new Vector<SearchCacheManagerPrx>();

        if (updateUnits.size() <= 0) {
            return;
        }

        UpdateUnit[] updateUnitSeq = new UpdateUnit[updateUnits.size()];
        {
            int i = 0;

            for(UpdateUnit unit: updateUnits)
            {
                updateUnitSeq[i++] = unit;
            }
        }
        
        for(int i=0; i<cluster(); i++)
        {
          List<ObjectPrx> resultList = locateAllProxy(managers,"M", i ,Channel.Invoke.Twoway,SearchCacheManagerPrxHelper.class,2000);
          for (int j=0; j<resultList.size(); j++) {
            SearchCacheManagerPrx prx = SearchCacheManagerPrxHelper.uncheckedCast(resultList.get(j));
            prx.update(updateUnitSeq);
          }
        }


    }       

    //TODO(zhigang.wu@opi-corp.com):将locateAllProxy函数添加到ReplicatedServiceAdapter类中。
    List<ObjectPrx> locateAllProxy(Vector<SearchCacheManagerPrx> ts, String name, int id, 
        Channel.Invoke iv, Class helper, int timeout) {
      locateProxy(ts, name, id, iv, helper, timeout);

      Ice.ObjectPrx proxy = ts.get(id%cluster);
      //System.out.println("in LocateAllProxy, after locateProxy:" + proxy.toString());
      if (!replicatedProxy.containsKey(proxy)) {
        replicatedProxy.put(proxy, getReplicatedProxy(proxy));
      }   

      Map<Boolean, List<ObjectPrx>> allReplica = replicatedProxy.get(proxy);
      List<ObjectPrx> resultList = new ArrayList<ObjectPrx>();
      resultList.addAll(allReplica.get(false));
      resultList.addAll(allReplica.get(true));

      return resultList;
    }

    int cluster()
    {
      return 4;
    }

 //   public static void main(String []args)
 //   {
 //     SearchCache2Adapter adapter = SearchCache2Adapter.getInstance();
 //     //Vector<Integer> user_ids = new Vector<Integer>();
 //     //Vector<UpdateUnit> updateSeq = new Vector<UpdateUnit>();
 //     for(int i=58742; i<=58745; i++)
 //     {
 //       //System.out.println("i="+i);
 //       UpdateUnit unit = new UpdateUnit();
 //       unit.id = i;
 //       unit.table = "user_fond";
 //       Map<String, String> temp_pair = new HashMap<String, String>();
 //       temp_pair.put("MOVIE", "love");
 //       adapter.update(unit.id, unit.table, temp_pair);
 //       try{
 //         java.lang.Thread.sleep(5000);
 //       } catch (InterruptedException   e){}

 //     }

 //     System.out.println("finish load...");
 //     try{
 //       java.lang.Thread.sleep(5000);
 //     } catch (InterruptedException   e){}
 //     System.exit(0);
 //   }
}

