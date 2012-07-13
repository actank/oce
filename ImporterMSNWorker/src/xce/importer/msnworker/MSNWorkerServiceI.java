package xce.importer.msnworker;

import xce.util.service.ServiceI;

public class MSNWorkerServiceI extends ServiceI{

	@Override
	public void finishup() {
		/** nothing to do **/
	}

	@Override
	public void initialize() {
		_adapter.add(new ImporterMSNWorkerI(), _ic.stringToIdentity("M"));
		_adapter.activate();
	}

}
