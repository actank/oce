package xce.ilikeit;

import xce.ilikeit.ILikeItNoticeI;
import xce.util.service.ServiceI;

public class ILikeItService extends ServiceI {

	@Override
	public void finishup() {
	}

	@Override
	public void initialize() {
		_adapter.add(new ILikeItNoticeI(), Ice.Util.stringToIdentity("M"));
	}
}
