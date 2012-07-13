package mop.hi.svc.personalmessage;

import Ice.Communicator;
import Ice.LocalObjectImpl;
import Ice.ObjectAdapter;
import IceBox.Service;

public class PersonalMessageService extends LocalObjectImpl implements Service {

	private ObjectAdapter _adapter;

	public void start(String name, Communicator ic, String[] args) {
		_adapter = ic.createObjectAdapter(name);
		Ice.Object obj = PersonalMessageManagerI.getInstance();
		_adapter.add(obj, Ice.Util.stringToIdentity("M"));
		_adapter.activate();
	}

	public void stop() {
		_adapter.deactivate();
	}

}
