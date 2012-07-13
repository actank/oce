package externalsearch;

import java.io.IOException;

import xce.util.service.ServiceI;

public class ExternalSearcherService extends ServiceI {

	@Override
	public void finishup() { 
	}

	@Override
	public void initialize() {
		_adapter.add(new ExternalSearcherI(), Ice.Util.stringToIdentity("M"));
	}
}
