package com.xiaonei.xce.footprintcommon;

import xce.footprint.FootprintInfo;

import com.xiaonei.xce.footprintunite.FootprintFacadeAdapter;
import com.xiaonei.xce.log.Oce;

public class FootprintAdapter {
	private FootprintReplica _interviewer;
	private FootprintReplica _interviewee;
	private FootprintFacadeAdapter _facade;
	
	public FootprintAdapter(FootprintReplica interviewer, FootprintReplica interviewee) {
		this._interviewer = interviewer;
		this._interviewee = interviewee;
		this._facade = new FootprintFacadeAdapter();
	}
	
	// byInterviewer
	public FootprintResult getAllByInterviewer(int owner) {
		return getAllByInterviewer(owner, 0, 6, 0);
	}

	public FootprintResult getAllByInterviewer(int owner, int begin, int limit) {
		return getAllByInterviewer(owner, begin, limit, 0);
	}

	public FootprintResult getAllByInterviewer(int owner, int begin, int nLimit,
			int tLimit) {
		FootprintResult res = null;
		try {
			res = _interviewer.getAll(owner, begin, nLimit, tLimit);
		} catch(Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAllByInterviewer [" + _interviewer.toString() + "]"  + owner
							+ " Ice.TimeoutException");
		}
		return res;
	}

	public void removeByInterviewer(FootprintInfo fpt) {
		try {
			_interviewer.remove(fpt);
		} catch(Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeByInterviewer [" + _interviewer.toString() + "]"  + fpt.owner
							+ " Ice.TimeoutException");
		}
	}

	// byInterviewee
	public FootprintResult getAllByInterviewee(int owner) {
		return getAllByInterviewee(owner, 0, 6, 0);
	}

	public FootprintResult getAllByInterviewee(int owner, int begin,
			int limit) {
		return getAllByInterviewee(owner, begin, limit, 0);
	}

	public FootprintResult getAllByInterviewee(int owner, int begin,
			int nLimit, int tLimit) {
		FootprintResult res = null;
		try {
			res = _interviewee.getAll(owner, begin, nLimit,
				tLimit);
		} catch(Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAllByInterviewee [" + _interviewee.toString() + "]"  + owner
							+ " Ice.TimeoutException");
		}
		return res;
	}

	public void removeByInterviewee(FootprintInfo fpt) {
		try {
			_interviewee.remove(fpt);
		} catch(Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getAllByInterviewee [" + _interviewee.toString() + "]"  + fpt.owner
							+ " Ice.TimeoutException");
		}
	}

	// Facade
	public void visit(FootprintInfo fpt) {
		try {
			_facade.visit(fpt);
		} catch(Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".visit [" + _facade.toString() + "]"  + fpt.owner
							+ " Ice.TimeoutException");
		}
	}
}