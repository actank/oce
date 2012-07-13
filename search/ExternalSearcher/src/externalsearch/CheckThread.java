package externalsearch;

import java.io.File;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class CheckThread extends Thread {

	private String path;
	private ExternalSearcherI service;

	public CheckThread(String path, ExternalSearcherI service) {
		this.path = path;
		this.service = service;
	}

	public void run() {
		while (true) {
			try {
				Thread.sleep(10000);
				File dir = new File(path);
				String[] indexs = dir.list();
				DateFormat format = new SimpleDateFormat("yyyy-MM-dd");
				if (indexs.length == 0) {
					continue;
				}
				Date maxDate = null;
				for (int i = 0; i < indexs.length; ++i) {
					try {
						Date date = format.parse(indexs[i]);
						if (maxDate == null || date.after(maxDate)) {
							maxDate = date;
						}
					} catch (ParseException ex) {
						ex.printStackTrace();
					}
				}
				service.reloadIndex(format.format(maxDate));

			} catch (Throwable ex) {

			}
		}
	}
}
