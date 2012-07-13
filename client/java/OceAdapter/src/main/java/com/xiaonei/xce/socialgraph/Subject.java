package com.xiaonei.xce.socialgraph;

import java.util.ArrayList;
import java.util.List;

/**
 * 负责广播通过第一个Observer
 * 
 * @author 李勇(liyong@opi-corp.com)
 * 
 */
public abstract class Subject {

    private List<Observer> observers = new ArrayList<Observer>();

    public synchronized void attach(Observer ob) {
        observers.add(ob);
    }

    public synchronized void detach(Observer ob) {
        observers.remove(ob);
    }

    public synchronized void notifyObservers(final String zNodePath, final String zNodeValue) {
        for (Observer ob : observers) {
            ob.update(zNodePath, zNodeValue);
        }
    }

}
