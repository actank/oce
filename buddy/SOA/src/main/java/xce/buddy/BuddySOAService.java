package xce.buddy;

import xce.buddy.BuddySOAManagerI;
import xce.util.service.ServiceI;
import xce.util.log.Log;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Properties;

public class BuddySOAService extends ServiceI {

	@Override
	public void finishup() {
	}

	@Override
	public void initialize() {
		_adapter.add(new BuddySOAManagerI(), Ice.Util.stringToIdentity("M"));
		Log.getInstance().getLog().info("initialize");
		System.out.println("initialize");
	}
}
