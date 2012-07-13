package xce.util.tools;

import java.io.BufferedReader;
import java.io.InputStreamReader;


public class UpTime {
	public static String GetUpTime() {
		return "";
		/* BufferedReader br = null;
		String tmp = "";
		try {
			Process p = Runtime.getRuntime().exec("uptime");
			br = new BufferedReader(new InputStreamReader(p.getInputStream()));
			tmp = br.readLine();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (null != br) {
				try {
					br.close();
				} catch (Exception e) {
				}
			}
		}
		return tmp; */
	}
	
	public static String GetLoad() {
		return "";
		/* String uptime = GetUpTime();
		int index = uptime.indexOf("load average:");
		if(index >= 0 && index < uptime.length()) {
			return uptime.substring(uptime.indexOf("load average:"));
		} else {
			return "";
		} */
	}
	
	public static double GetLoadOne() {
		return -1;
		/* String load = GetLoad();
		if (load.equals("")) {
			return 0.0;
		} else {			
			return Double.parseDouble(load.substring("load average:".length()).split(",")[0]);
		} */
	}
	
	public static double GetLoadFive() {
		return -1;
		/* String load = GetLoad();
		if (load.equals("")) {
			return 0.0;
		} else {
			return Double.parseDouble(load.substring("load average:".length()).split(",")[1]);
		} */
	}
	
	public static double GetLoadFifteen() {
		return -1;
		/* String load = GetLoad();
		if (load.equals("")) {
			return 0.0;
		} else {
			return Double.parseDouble(load.substring("load average:".length()).split(",")[2]);
		} */
	}
}

