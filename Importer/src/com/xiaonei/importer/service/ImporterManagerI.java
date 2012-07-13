package com.xiaonei.importer.service;

import Ice.Current;

import com.xiaonei.importer.CSV.CSVImporter;
import com.xiaonei.importer.WindowsLive.WindowsLiveImporter;
import com.xiaonei.importer._126._126Importer;
import com.xiaonei.importer.google.GMailImporter;
import com.xiaonei.importer.netease.NeteaseImporter;
import com.xiaonei.importer.yahoo.YImporter;
import com.xiaonei.importer.sina.SinaImporter;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.OwnerInfo;
import com.xiaonei.svc.importer._ImporterManagerDisp;

public class ImporterManagerI extends _ImporterManagerDisp {

	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ImporterManagerI.class);

	public ImporterManagerI() {
	}

	public BuddyList getResponse(OwnerInfo owner, Current __current) {
		return getMSNResponse(owner, __current);
	}

	public void sendRequest(OwnerInfo owner, Current __current) {
		sendMSNRequest(owner, __current);
	}

	public void sendMSNRequest(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		WindowsLiveImporter.getInstance().requestBuddyList(owner);
	}

	public void sendGMailRequest(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		GMailImporter.getInstance().requestBuddyList(owner);
	}

	public void sendNeteaseRequest(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		NeteaseImporter.getInstance().requestBuddyList(owner);
	}

	public void send126Request(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		_126Importer.getInstance().requestBuddyList(owner);
	}

	public void sendCSVRequest(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		CSVImporter.getInstance().requestBuddyList(owner, __current);
	}

	public BuddyList getCSVResponse(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = CSVImporter.getInstance().retriveBuddyList(owner);
		return buddies;
	}

	public BuddyList getMSNResponse(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = WindowsLiveImporter.getInstance().retriveBuddyList(
				owner);
		return buddies;
	}

	public BuddyList getGMailResponse(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = GMailImporter.getInstance().retriveBuddyList(owner);
		return buddies;
	}

	public BuddyList getNeteaseResponse(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = NeteaseImporter.getInstance().retriveBuddyList(
				owner);
		return buddies;
	}

	public BuddyList get126Response(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = _126Importer.getInstance().retriveBuddyList(owner);
		return buddies;
	}

	public BuddyList getSinaResponse(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = SinaImporter.getInstance().retriveBuddyList(owner);
		return buddies;
	}

	public BuddyList getYahooResponse(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		BuddyList buddies = YImporter.getInstance().retriveBuddyList(owner);
		return buddies;
	}

	public void sendSinaRequest(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		SinaImporter.getInstance().requestBuddyList(owner);
	}

	public void sendYahooRequest(OwnerInfo owner, Current __current) {
		if (_logger.isTraceEnabled()) {
			_logger.trace(owner.userId + " " + owner.email + " "
					+ owner.password);
		}
		YImporter.getInstance().requestBuddyList(owner);
	}

	public boolean verifyMSNAccount(String liveId, String livePassword,
			Current __current) {
		return WindowsLiveImporter.getInstance().verify(liveId, livePassword);
	}

}
