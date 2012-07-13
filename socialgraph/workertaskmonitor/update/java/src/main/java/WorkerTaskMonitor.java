import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.sql.Timestamp;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.TreeMap;

import strategy.MailByDay;
import strategy.Strategy;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;

/**
 * 
 * @author zhangnan
 * 各业务worker的监控类
 */
public class WorkerTaskMonitor {
	private static WorkerTaskMonitor instance = null;
	private Map<String, Map<Integer, Integer>> workerTaskMap;
	
	private WorkerTaskMonitor() {
//		workerTaskMap = new HashMap<String, Map<Integer,Integer>>();
		workerTaskMap = new TreeMap<String, Map<Integer,Integer>>();
	}
	
	public static WorkerTaskMonitor getInstance() {
		if (null == instance) {
			synchronized (WorkerTaskMonitor.class) {
				if (null == instance) {
					try {
						instance = new WorkerTaskMonitor();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	public void generateWorkerTaskFileByHour(String configFile, String outputPath) {
		createWorkerTask(configFile);				//获取每个worker的份数
		
//		for (Entry<String, Map<Integer, Integer>> item : workerTaskMap.entrySet()) {
//			for (Entry<Integer, Integer> secItem : item.getValue().entrySet()) {
//				System.out.println(item.getKey() + ":" + secItem.getKey() + ":" + secItem.getValue());
//			}
//		}
		
		createHourMonitor(outputPath);		//创建每小时的监控文件
	}
	
	public void createSentMail(String source, String target) {
		System.out.println("source:" + source + "   target:" + target);
		Strategy strategy = new MailByDay();
		strategy.createMail(source, target);
	}
	
	private void createWorkerTask(String configFile) {
		Map<String, Integer> bizCluster = new HashMap<String, Integer>();		//存放每个业务的份数
		String prefix = "Controller";			//前缀
		String line = "";
		
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(configFile)));
			line = reader.readLine();
			while (null != line) {
				ReplicatedClusterAdapter adapter = new ReplicatedClusterAdapter(line, 120);
				int cluster = adapter.getCluster();				//获取每个业务的份数
				
				if (line.startsWith(prefix)) {
					String subName = line.substring(10);				//Controller占10个字节大小，取去除Controller后的具体业务名 
					if (!bizCluster.containsKey(subName)) {
						bizCluster.put(subName, cluster);
					}
				}
				line = reader.readLine();
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		xce.svcapi.TaskManagerAPIPrx tma = null;
		for (Entry<String, Integer> item : bizCluster.entrySet()) {
			for (int index = 0; index < item.getValue(); ++index) {
				String workerServer = item.getKey() + String.valueOf(index);
				String endpoint = "TMA@" + workerServer;			//拼接endpoint
				Ice.ObjectPrx prx = Channel.newSocialGraphChannel("").getCommunicator().stringToProxy(endpoint);
				tma = xce.svcapi.TaskManagerAPIPrxHelper.checkedCast(prx);
				
				Map<Integer, Integer> state = tma.getLevel2Size();			//获取业务中不同task的size
				
				if (!workerTaskMap.containsKey(workerServer)) {
					workerTaskMap.put(workerServer, state);
				}
			}
		}
	}
	
	private void createHourMonitor(String path) {
		Date date = new Date();
		
		String dateStr2 = new Timestamp(date.getTime()).toString();
		System.out.println("dateStr2 : " + dateStr2);
		
		String[] dateList = dateStr2.split(" ");
		String[] timeList = dateList[1].split(":");
		
		String target = "";
		if (path.endsWith("/")) {				//对路径后缀的处理
			target = path + dateList[0] + "-" + timeList[0];
		} else {
			target = path + "/" + dateList[0] + "-" + timeList[0];
		}
		System.out.println("target : " + target);
		
		try {
			File out = new File(target);
			if (out.exists()) {
				out.delete();
			}
			PrintWriter writer = new PrintWriter(new OutputStreamWriter(new FileOutputStream(target, true)), true);
			String line = "";
			for (Entry<String, Map<Integer, Integer>> eachItem : workerTaskMap.entrySet()) {
				for (Entry<Integer, Integer> item : eachItem.getValue().entrySet()) {
					if (item.getKey() < 0) {
						continue;
					}
					line = eachItem.getKey() + "\t" + String.valueOf(item.getKey()) + "\t" + String.valueOf(item.getValue()) + "\n";
					writer.write(line);
					writer.flush();
				}
			}
			writer.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) {
		if (1 != args.length) {
			System.out.println("Usage: BYHOUR or MAIL");
			System.exit(0);
		}
		
		if (args[0].equals("BYHOUR")) {
			System.out.println("run as BYHOUR parameter!");
			WorkerTaskMonitor.getInstance().generateWorkerTaskFileByHour("/data/xce/Project/workermoniter/worker_task/update/config", "/data/xce/Project/workermoniter/worker_task/hour");
		} else if (args[0].equals("MAIL")) {
			System.out.println("run as MAIL parameter!");
			WorkerTaskMonitor.getInstance().createSentMail("/data/xce/Project/workermoniter/worker_task/hour", "/data/xce/Project/workermoniter/worker_task/mail");
		}
		
		System.exit(0);
	}
}
