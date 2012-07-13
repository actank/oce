package xce.util.tools;

import java.text.SimpleDateFormat;
import java.util.Date;

public class DateFormatter {
	// public static SimpleDateFormat formatter = new SimpleDateFormat(
	// "yyyy-MM-dd HH:mm:ss");

	public static String format(Date date) {
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		return formatter.format(date);
	}

	public static Date parse(String date) {
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		try {
			return formatter.parse(date);
		} catch (Exception e) {
			return new Date();
		}
	}
}
