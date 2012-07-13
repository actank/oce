package xce.socialgraph.util;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Properties;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

/**
 * Logging util for socialgraph recommend system
 * @author zhangnan
 *
 */
public class Clogging {
	private static String loggerName = "sglog";
	
	/**
	 * initialize log directory
	 * @param bizName
	 */
	public static void InitLogger(String bizName) {
		String currentPath = System.getProperty("user.dir");
		String subStr = "SGProject";
		int pos = currentPath.indexOf(subStr);
		String rootPath = currentPath.substring(0, pos + subStr.length());
		String configuration = rootPath + "/conf/log4j.properties";
		
		File conf = new File(configuration);
		if (!conf.exists()) {		//whether configuration exists
			return;
		}
		
		Properties properties = new Properties();
		try {
			properties.load(new FileReader(conf));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return;
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		
		String logDir = rootPath + "/logs/" + bizName;
		File logDirFile = new File(logDir);
		if (!logDirFile.exists()) {			//the target_log directory does not exist, then create it
			logDirFile.mkdir();
		}
		String logFile = logDir + "/" + bizName + ".log";
		String key = "log4j.appender.fileAppender.File";
		properties.setProperty(key, logFile);
		
		PropertyConfigurator.configure(properties);
	}
	
	/**
	 * use default configuration
	 */
	public static void InitBasicLogger() {
		BasicConfigurator.configure();
	}
	
	public static void Trace(String message) {
		if (Logger.exists(loggerName) == null) {
			InitBasicLogger();
		}
		Logger.getLogger(loggerName).trace(message);
	}
	
	public static void Debug(String message) {
		if (Logger.exists(loggerName) == null) {
			InitBasicLogger();
		}
		Logger.getLogger(loggerName).debug(message);
	}
	
	public static void Info(String message) {
		if (Logger.exists(loggerName) == null) {
			InitBasicLogger();
		}
		Logger.getLogger(loggerName).info(message);
	}
	
	public static void Warn(String message) {
		if (Logger.exists(loggerName) == null) {
			InitBasicLogger();
		}
		Logger.getLogger(loggerName).warn(message);
	}
	
	public static void Error(String message) {
		if (Logger.exists(loggerName) == null) {
			InitBasicLogger();
		}
		Logger.getLogger(loggerName).error(message);
	}
	
	public static void Fatal(String message) {
		if (Logger.exists(loggerName) == null) {
			InitBasicLogger();
		}
		Logger.getLogger(loggerName).fatal(message);
	}
	
//	public static void main(String[] args) {
//		Clogging.InitLogger("JavaLog");
//		
//		Clogging.Trace("this is trace message ...");
//		Clogging.Debug("this is debug message ...");
//		Clogging.Info("this is info message ...");
//		Clogging.Warn("this is warn message ...");
//		Clogging.Error("this is error message ...");
//		Clogging.Fatal("this is fatal message ...");
//	}
}
