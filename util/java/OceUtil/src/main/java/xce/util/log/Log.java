package xce.util.log;

import java.util.Properties;
import org.apache.log4j.Logger;
import org.apache.log4j.Priority;
import org.apache.log4j.PropertyConfigurator;

public class Log {
	
  private static Log instance; 
  private Logger log;
	private Log(){
		log = Logger.getLogger(Log.class);		
	}
	
	public static Log getInstance(){
		if (instance == null) {
      synchronized (Log.class) {
        if (instance == null) {
          instance = new Log();
        }
      }
		}
		return instance;
	}
	public void init(String name){
		/*Properties p = new Properties();
		
		p.put("log4j.rootLogger", "DEBUG, CONSOLE");
		p.put("log4j.appender.CONSOLE", "org.apache.log4j.DailyRollingFileAppender");
		p.put("log4j.appender.CONSOLE.File", "../log/" + name + "/oce_log");
		//p.put("log4j.appender.CONSOLE.ImmediateFlush", "false");
		p.put("log4j.appender.CONSOLE.DatePattern", "'.'yyyy-MM-dd-HH");
		p.put("log4j.appender.CONSOLE.layout", "org.apache.log4j.PatternLayout");
		p.put("log4j.appender.CONSOLE.layout.ConversionPattern", "%d{HH:mm:ss,SSS} - %5p [%t] (%F:%L) - %m%n");

	  PropertyConfigurator.configure(p);	*/
	}
	
	public Logger getLog(){
		return log;
	}
	
	public void error(Object message){
		log.error(message);
	}
	
	public void fatal(Object message){
		log.fatal(message);
	}
	
	public void warn(Object message){
		log.warn(message);
	}
	
	public void info(Object message){
		log.info(message);
	}
	
	public void debug(Object message){
		log.info(message);
	}
	
	public void trace(Object message){
		log.info(message);
	}
	
	public void log(Priority priority,Object message){
		log.log(priority, message);
	}
	
}
