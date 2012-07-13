package com.xiaonei.xce.footprintview;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.footprint.FootprintViewManagerPrx;
import xce.footprint.FootprintViewManagerPrxHelper;
import xce.footprint.VisitCountInfo;

public class FootprintViewAdapter extends ReplicatedClusterAdapter {

    private static final String ctr_endpoints = "ControllerFootprintView";

    private static int _interval = 120;

    private int _cluster = 10;

    public FootprintViewAdapter() {

        super(ctr_endpoints, _interval);
    }

    private FootprintViewManagerPrx getFootprintViewManager(int owner) {
        Ice.ObjectPrx prx0 = getProxy(owner, 300);

        if (prx0 != null) {
            FootprintViewManagerPrx prx;
            prx = FootprintViewManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

    private FootprintViewManagerPrx getFootprintViewManagerOneWay(int owner) {
        Ice.ObjectPrx prx0 = getProxy(owner, 300);

        if (prx0 != null) {
            FootprintViewManagerPrx prx;
            prx = FootprintViewManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
            return prx;
        } else {
            return null;
        }
    }

    public List<VisitCountView> getVisitCountHistory(int owner) {
        try {
            List<VisitCountView> return_value = new ArrayList<VisitCountView>();
            List<VisitCountInfo> history = getFootprintViewManager(owner).getVisitCountHistory(
                    owner);

            for (VisitCountInfo info : history) {
                return_value.add(new VisitCountView(info));
            }
            return return_value;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new ArrayList<VisitCountView>();
    }

    public VisitCountView getRecentVisitCount(int owner) {
        try {
            return new VisitCountView(getFootprintViewManager(owner).getRecentVisitCount(owner));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new VisitCountView(new VisitCountInfo());
    }

    public void setVisitCountLimit(int owner) {
        try {
            getFootprintViewManager(owner).setVisitCountLimit(owner);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void setVisitCountState(int owner, int state) {
        try {
            getFootprintViewManager(owner).setVisitCountState(owner, state);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public boolean verifyVisitCount(Map<Integer, Integer> counts) {
        Map<Integer, Map<Integer, Integer>> count_map = new HashMap<Integer, Map<Integer, Integer>>();
        for (int i = 0; i < this._cluster; i++) {
            count_map.put(i, new HashMap<Integer, Integer>());
        }

        for (Entry<Integer, Integer> en : counts.entrySet()) {
            int index = en.getKey() % _cluster;
            count_map.get(index).put(en.getKey(), en.getValue());
        }

        boolean return_value = true;
        for (int i = 0; i < count_map.size(); i++) {
            try {
                if (!count_map.get(i).isEmpty()) {
                    boolean value = getFootprintViewManager(i).verifyVisitCount(count_map.get(i));
                    if (!value) {
                        return_value = false;
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return return_value;
    }
}
