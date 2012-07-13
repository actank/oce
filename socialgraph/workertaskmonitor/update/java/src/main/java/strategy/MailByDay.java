package strategy;

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
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

public class MailByDay implements Strategy  {
//	private String source = "";
//	private String target = "";
	
	private Map<String, List<TaskItem>> workerTaskState = null;
	
	public MailByDay() { 
		workerTaskState = new TreeMap<String, List<TaskItem>>();
	}
	
	@Override
	/**
	 * 创建监控邮件
	 */
	public void createMail(String source, String target) {
		System.out.println("[MailByDay] createMail source:" + source + "   target:" + target);
//		this.source = source;
//		this.target = target;
		
		String[] timeList = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
		Date date = new Date();
		String dateStr2 = new Timestamp(date.getTime() - 24*60*60*1000).toString();		//取昨天的日期
		String[] dateList = dateStr2.split(" ");
		
		for (int i = 0; i < timeList.length; ++i) {
//			if (i != 11 && i != 12) {
//				continue;
//			}
			String file = dateList[0] + "-" + timeList[i];		//拼接出原始数据文件
			if (source.endsWith("/")) {
				file = source + file;
			} else {
				file = source + "/" + file;
			}
//			System.out.println("file : " + file);
			
			try {
				File inputFile = new File(file);
				if (!inputFile.exists()) {
					continue;
				}
				BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
				
				String line = reader.readLine();
				while (null != line) {
//					System.out.println("each line : " + line);
					String[] lineItems = line.split("\t");
					if (lineItems.length != 3) {		//数据格式为service	level size
						line = reader.readLine();
						continue;
					}
//					System.out.println("lineItems size : " + lineItems.length);
					if (!workerTaskState.containsKey(lineItems[0])) {
						List<TaskItem> taskItemList = new ArrayList<MailByDay.TaskItem>();
						TaskItem item = new TaskItem();
						item.time = timeList[i];		//时间取当前遍历文件的时间
						taskItemList.add(item);
						workerTaskState.put(lineItems[0], taskItemList);
					}
					
					Integer taskIndex = Integer.valueOf(lineItems[1]);		//第二列为task_level
					Integer taskPoolSize = Integer.valueOf(lineItems[2]);		//第三列为当前task的pool size
					
					List<TaskItem> tmpTaskItemList = workerTaskState.get(lineItems[0]);
					
					int itemLength = tmpTaskItemList.size();
					if (tmpTaskItemList.get(itemLength - 1).time == timeList[i]) {			//判断是否为当前时间
						tmpTaskItemList.get(itemLength - 1).taskMap.put(taskIndex, taskPoolSize);			//放入task_level 和 pool_size
					} else {
						TaskItem newItem = new TaskItem();
						newItem.time = timeList[i];
						newItem.taskMap.put(taskIndex, taskPoolSize);
						tmpTaskItemList.add(newItem);
					}
					
					line = reader.readLine();
				}
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
//		show();			//显示
		
		generate(target);		//产生监控文件
	}
	
	public void show() {
		for (Entry<String, List<TaskItem>> workerTask : workerTaskState.entrySet()) {
			System.out.print(workerTask.getKey() + " : ");
			for (TaskItem item : workerTask.getValue()) {
				System.out.println(item.time);
				for (Entry<Integer, Integer> subItem : item.taskMap.entrySet()) {
					System.out.println(subItem.getKey() + " - " + subItem.getValue());
				}
				System.out.println();
			}
			
			System.out.println("-----------------------------------------------------------");
		}
	}
	
	/**
	 * 生成原始邮件文件
	 * @param target	邮件文件存放的位置
	 */
	private void generate(String target) {
		Set<String> specialItemSet = getSpecialItem();
		for (String ss : specialItemSet) {
			System.out.println(ss);
		}
		
		Date date = new Date();
		String dateStr2 = new Timestamp(date.getTime() - 24*60*60*1000).toString();		//取昨天的日期
		String[] dateList = dateStr2.split(" ");
		
		System.out.println("target : " + target);
		String file = "";
		if (target.endsWith("/")) {
			 file = target + "tasksize_" + dateList[0] + ".html";
		} else {
			file = target + "/tasksize_" + dateList[0] + ".html";
		}
		
		System.out.println("file : " + file);
		String specialBiz = "FriendFinderNewComerWorker0";			//FriendFinderNewComerWorker0优先展示
		File out = new File(file);
		if (out.exists()) {			//如果文件存在，则删除文件
			out.delete();
		}
		try {
			PrintWriter writer = new PrintWriter(new OutputStreamWriter(new FileOutputStream(file, true)), true);
			writer.write("<body>\n");
			
			//优先 FriendFinderNewComerWorker0，并且FriendFinderNewComerWorker0有队列积攒情况
			if (workerTaskState.containsKey(specialBiz)
					&& specialItemSet.contains(specialBiz)) {
				writer.write("<h4>" + specialBiz + "</h4>\n");
				writer.write("<table bordercolor=\"#000033\" bgcolor=\"#99ccff\" border=\"1\">\n");
				writer.write("<tr><td>Time</td>");
				List<TaskItem> taskItemList = workerTaskState.get(specialBiz);
				if (!taskItemList.isEmpty()) {
					for (Entry<Integer, Integer> ee : taskItemList.get(0).taskMap.entrySet()) {
						writer.write("<td>" + "Level_" + ee.getKey() + "</td>");
					}
				}
				writer.write("</tr>\n");
				
				if (!taskItemList.isEmpty()) {
					for (TaskItem it : taskItemList) {
						String time = it.time + ":00" + "-" + it.time + ":59";
						writer.write("<tr><td>" + time + "</td>");
						for (Entry<Integer, Integer> taskIt : it.taskMap.entrySet()) {
							writer.write("<td>" + String.valueOf(taskIt.getValue()) + "</td>");
						}
						writer.write("</tr>\n");
					}
				}
			}
			writer.write("<!--tr></tr-->\n");
			writer.write("</table>\n");
			writer.write("<br></br>\n");
			
			//优先选择 有问题的, 不再展示FriendFinderNewComerWorker0
			for (String bizName : specialItemSet) {
				if (workerTaskState.containsKey(bizName) 
						&& !specialBiz.equals(bizName)) {
					writer.write("<h4>" + bizName + "</h4>\n");
					writer.write("<table bordercolor=\"#000033\" bgcolor=\"#99ccff\" border=\"1\">\n");
					writer.write("<tr><td>Time</td>");
					List<TaskItem> taskItemList = workerTaskState.get(bizName);
					if (!taskItemList.isEmpty()) {
						for (Entry<Integer, Integer> ee : taskItemList.get(0).taskMap.entrySet()) {
							writer.write("<td>" + "Level_" + ee.getKey() + "</td>");
						}
					}
					writer.write("</tr>\n");
					
					if (!taskItemList.isEmpty()) {
						for (TaskItem it : taskItemList) {
							String time = it.time + ":00" + "-" + it.time + ":59";
							writer.write("<tr><td>" + time + "</td>");
							for (Entry<Integer, Integer> taskIt : it.taskMap.entrySet()) {
								writer.write("<td>" + String.valueOf(taskIt.getValue()) + "</td>");
							}
							writer.write("</tr>\n");
						}
					}
					writer.write("</table>\n");
					writer.write("<br></br>\n");
				}
			}
			
			//其他业务只列个名字即可
			if (!specialItemSet.contains(specialBiz)) {
				writer.write(specialBiz + "<br></br>\n");			//如果FriendFinderNewComerWorker0没有队列积攒，则优先列出
			}
			
			for (Entry<String, List<TaskItem>> ee : workerTaskState.entrySet()) {
				if (!specialItemSet.contains(ee.getKey()) &&
						!specialBiz.equals(ee.getKey())) {
					writer.write(ee.getKey() + "<br></br>\n");
				}
			}
			
			writer.write("</body>\n");
			writer.flush();
			writer.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 在昨天的监控数据中找出有问题的服务，提前显示
	 * @return 有问题的服务列表
	 */
	private Set<String> getSpecialItem() {
		Set<String> result = new TreeSet<String>();
		for (Entry<String, List<TaskItem>> workerTask : workerTaskState.entrySet()) {
			boolean judge = false;
			for (TaskItem item : workerTask.getValue()) {
				for (Entry<Integer, Integer> subItem : item.taskMap.entrySet()) {
					if (subItem.getValue() > 0) {
						judge = true;
					}
				}
			}
			if (judge) {
				result.add(workerTask.getKey());
			}
		}
		
		return result;
	}
	
	public static void main(String[] args) {
		MailByDay mail = new MailByDay();
		mail.createMail("/home/zhangnan/tmp", "/home/zhangnan/tmp");
	}
	
	private class TaskItem {
		public String time = "";
		public Map<Integer, Integer> taskMap = null;
		
		public TaskItem() {
			taskMap = new HashMap<Integer, Integer>();
		}
	};
}
