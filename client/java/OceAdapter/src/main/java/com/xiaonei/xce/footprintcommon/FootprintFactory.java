package com.xiaonei.xce.footprintcommon;

public class FootprintFactory {

	private static FootprintAdapter _loveFootprintAdapter;

	public static FootprintAdapter create(FootprintType.Type type) {
		switch (type) {
		case love:
			if (null == _loveFootprintAdapter) {
				synchronized (FootprintAdapter.class) {
					if (null == _loveFootprintAdapter) {
						FootprintReplica interviewer = new FootprintReplica(
								"ControllerLoveFootprintByInterviewer", 120);
						FootprintReplica interviewee = new FootprintReplica(
								"ControllerLoveFootprintByInterviewee", 120);
						_loveFootprintAdapter = new FootprintAdapter(
								interviewer, interviewee);
						return _loveFootprintAdapter;
					}
				}
			}
			return _loveFootprintAdapter;
			// case :
		default:
			return null;
		}
	}
}