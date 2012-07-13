package com.renren.xce.socialgraph.userfeature;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import xce.socialgraph.util.Clogging;

import com.renren.xce.socialgraph.updator.AbstractApplication;
import com.renren.xce.socialgraph.updator.Application;
import com.renren.xce.socialgraph.userfeature.async.UserFeatureMessage;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;

/**
 * 用户特征库程序
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class UserFeatureApplication extends AbstractApplication {

    private ExecutorService threadPool;

    public void initialize() {
	Clogging.InitLogger("userfeature");
    }

    public UserFeatureApplication() {
	threadPool = Executors.newFixedThreadPool(2);
    }

    @Override
    public void run() {
	long begin = System.currentTimeMillis();
	// 1.加载user prefer data
	File userPreferFile = new File(getDataDir(), "UserPrefer.dat");
	if (!userPreferFile.exists()) {
	    Clogging.Error("UserPrefer.dat file doesn't exist!");
	    return;
	}
	// save cache
	try {
	    BufferedReader reader = new BufferedReader(new FileReader(
		    userPreferFile));
	    int loop = 0;
	    String line = null;

	    while ((line = reader.readLine()) != null) {
		Runnable runnble = new UserFeatureMessage(line);
		runnble.run();
		// threadPool.execute(runnble);
		++loop;
	    }
	    Clogging.Info("loop:" + loop);
	} catch (FileNotFoundException e) {
	    Clogging.Error(e.toString());
	} catch (IOException e) {
	    Clogging.Error(e.toString());
	}
	try {
	    TimeUnit.SECONDS.sleep(5);
	} catch (InterruptedException e) {
	    e.printStackTrace();
	}

	threadPool.shutdown();
	long end = System.currentTimeMillis();
	Clogging.Info("time cost:" + (end - begin) + " ms");
    }

    // ugly!!! shit!
    public static void main(String[] args) {
	Application app = new UserFeatureApplication();
	app.start();
    }
}
