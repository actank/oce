package com.renren.xce.socialgraph.character;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import xce.socialgraph.util.Clogging;

import com.renren.xce.socialgraph.updator.AbstractApplication;
import com.renren.xce.socialgraph.updator.Application;

/**
 * 叶银的个性化推荐策略实现
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class RecommendUpdateApplication extends AbstractApplication {

    private static ThreadPoolExecutor threadPool = new ThreadPoolExecutor(8,
	    16, 10, TimeUnit.MILLISECONDS, new LinkedBlockingDeque<Runnable>());

    @Override
    public void initialize() {
	Clogging.InitLogger("character_rcd_updator");
    }

    @Override
    public void run() {
	// 1.加载目标用户id列表
	// 2.计算
	// 为了节省内存，第1步不要全加载，加载一个，计算机一个为上策
	File userIdFile = new File(getDataDir(), "user_id.txt");
	if (!userIdFile.exists()) {
	    Clogging.Error("user_id.txt file doesn't exist!");
	    return;
	}
	BufferedReader reader = null;
	try {
	    reader = new BufferedReader(new FileReader(userIdFile));
	    String line = null;
	    int loop = 0;
	    while ((line = reader.readLine()) != null) {
		++loop;
		try {
					long begin = System.currentTimeMillis();
		    int userId = Integer.parseInt(line);
		    Runnable runnable = null;
		    if (userId % 100 == 69) {
			runnable = new ComputeMessage(userId, true);
		    } else {
			runnable = new ComputeMessage(userId, false);
		    }
		    //		    runnable = new ComputeMessage(userId, true);

		    //runnable.run();
                    threadPool.execute(runnable);
		    long end = System.currentTimeMillis();
//		    Clogging.Info("userId:" + userId + " timecost:"
//			    + (end - begin) + "ms loop:" + loop);
                    if (loop % 100 == 0) { 
                      try {
                        TimeUnit.MILLISECONDS.sleep(100);
                      } catch (Exception e) {
                        e.printStackTrace();
                      }
                    }
		} catch (NumberFormatException e) {
		    e.printStackTrace();
		    continue;
		}
	    }
	} catch (FileNotFoundException e) {
	    Clogging.Error(e.toString());
	} catch (IOException e) {
	    Clogging.Error(e.toString());
	} finally {
	    if (reader != null) {
		try {
		    reader.close();
		} catch (IOException e) {
		    Clogging.Error(e.toString());
		}
	    }
	}
	threadPool.shutdown();
    }

    public static void main(String[] args) {
	Clogging.Info("RecommendUpdateApplication start");
	Application app = new RecommendUpdateApplication();
	app.start();
	Clogging.Info("RecommendUpdateApplication finish");
    }

}
