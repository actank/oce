package xce.distcache.preloader;

public class SetThread extends Thread {
    public void run() {
	while (true) {
	    try {
		SetData.getInstance().push();
	    } catch (Exception e) {
		PreloaderLogger.warn(this + " " + e);
	    }
	}
    }
}
