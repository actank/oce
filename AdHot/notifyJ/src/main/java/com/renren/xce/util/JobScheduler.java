package com.renren.xce.util;

import org.quartz.*;

public class JobScheduler {
	private static JobScheduler instance = null;
	private static Object instance_lock = new Object();
	
	public static JobScheduler getInstance() {
		if (instance == null) {
			synchronized(instance_lock) {
				if(instance == null) { // 双check. 实现上是否靠谱?
					instance = new JobScheduler();
				}
			}
		}
		return instance;
	}

    SchedulerFactory schedFactory = new org.quartz.impl.StdSchedulerFactory();
    Scheduler sched = null; 
    private JobScheduler() {
    	try {
    	    sched = schedFactory.getScheduler();
           sched.start();    
       } catch (Exception e) {
            e.printStackTrace();
        } 
    }

    public void Schedule(String jobName, Class jobClass, String cronExpr) {
        JobDetail jobDetail = new JobDetail(jobName, jobName, jobClass/*QuartzDemo.class*/);
        jobDetail.getJobDataMap().put( "type", "FULL");

        CronTrigger trigger = new CronTrigger(jobName, jobName);
        // trigger.setCronExpression("0 0 12 ? * SUN");
        // trigger.setCronExpression("* * * ? * *");
        // trigger.setCronExpression("*/5 * * ? * *");
    	try {
           trigger.setCronExpression(cronExpr);

           sched.scheduleJob(jobDetail, trigger);
    	} catch (Exception e) {
           e.printStackTrace();
    	}
    }
}
