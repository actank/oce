package com.xiaonei.importer.WindowsLive;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.util.HashMap;
import java.util.NoSuchElementException;
import java.util.UUID;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpConnectionManager;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.MultiThreadedHttpConnectionManager;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.RequestEntity;
import org.apache.commons.httpclient.methods.StringRequestEntity;
import org.apache.commons.httpclient.params.HttpConnectionManagerParams;
import org.dom4j.DocumentException;

import com.xiaonei.importer.TaskManager;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.importer.relationship.RelationshipHelper;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;
import com.xiaonei.svc.importer.BuddyUserInfo;
import com.xiaonei.svc.importer.OwnerInfo;

public class WindowsLiveImporter implements WorkerCallback {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(WindowsLiveImporter.class);
	private static WindowsLiveImporter _instance = new WindowsLiveImporter();

	public static WindowsLiveImporter getInstance() {
		return _instance;
	}

	public WindowsLiveImporter() {
	}

	public void requestBuddyList(OwnerInfo owner) {
		if (_logger.isDebugEnabled()) {
			_logger
					.debug("requestBuddyList OwnerInfo[" + owner.userId + "," + owner.email + "," + owner.password
							+ "]");
		}
		BuddyList newBuddyList = new BuddyList();
		newBuddyList.status = new BuddyListStatus(301, "Needs Token");
		newBuddyList.buddies = new BuddyInfo[] {};
		newBuddyList.owner = owner;
		newBuddyList.users = new HashMap<String, BuddyUserInfo>();

		UUID requestId = UUID.randomUUID();

		InfomationCache.getInstance().putContacts(requestId, newBuddyList);
		InfomationCache.getInstance().putUUID(owner, requestId);

		TaskManager.getInstance().addTask(new WindowsLiveWorker(owner.email, owner.password, requestId, this));
	}

	public BuddyList retriveBuddyList(OwnerInfo owner) {
		if (_logger.isDebugEnabled()) {
			_logger
					.debug("retriveBuddyList OwnerInfo[" + owner.userId + "," + owner.email + "," + owner.password
							+ "]");
		}
		UUID requestId = InfomationCache.getInstance().getUUID(owner);
		if (_logger.isDebugEnabled()) {
			_logger.debug("retriveBuddyList OwnerInfo[" + owner.userId + "," + owner.email + "," + owner.password
					+ "] UUID=" + requestId);
		}
		if (null == requestId) {
			throw new NoSuchElementException("No Such request for userId=" + String.valueOf(owner.userId));
		}
		return getBuddyList(requestId);
	}

	public BuddyList getBuddyList(UUID requestId) {
		return InfomationCache.getInstance().getContacts(requestId);
	}

	protected void finishAuthToken(UUID requestId, WindowsLiveAuthToken token, BuddyListStatus status) {
		if (_logger.isDebugEnabled()) {
			_logger.debug("finishAuthToken UUID[" + requestId + "] => WindowsLiveAuthToken["
					+ (token == null ? "NULL" : token.token) + "] BuddyListStatus[" + status.code + ", "
					+ status.message + "]");
		}
		BuddyList list = InfomationCache.getInstance().getContacts(requestId);
		list.status = status;
		if (token != null) {
			InfomationCache.getInstance().putToken(requestId, token);
		}
	}

	public void finishContactList(UUID requestId, BuddyInfo[] list, BuddyListStatus status) {
		if (_logger.isDebugEnabled()) {
			_logger.debug("finishContactList UUID[" + requestId + "] => BuddyListStatus[" + status.code + ", "
					+ status.message + "]");
		}
		BuddyList buddies = InfomationCache.getInstance().getContacts(requestId);
		if (_logger.isDebugEnabled()) {
			_logger.debug("finishContactList original BuddyList size=" + buddies.buddies.length + " status="
					+ buddies.status.code + " " + buddies.status.message);
		}
		// We need list to be put into BuddyList before fillRelationship
		buddies.buddies = list;
		buddies.status = status;
		if (status.code >= 200 && status.code < 300 && buddies.buddies.length > 0) {
			RelationshipHelper.getInstance().fillRelationship(buddies);
		}
	}

	public boolean verify(String liveId, String livePassword){
		HttpConnectionManager manager = new MultiThreadedHttpConnectionManager();
		HttpConnectionManagerParams parm = new HttpConnectionManagerParams();
		parm.setDefaultMaxConnectionsPerHost(64);
		parm.setMaxTotalConnections(128);
		parm.setStaleCheckingEnabled(true);
		manager.setParams(parm);
		HttpClient httpclient = new HttpClient(manager);
		WindowsLiveAuthToken token=null;
		boolean ret = false;
		PostMethod post = new PostMethod(WindowsLiveProtocal.wstEndpoints);
		RequestEntity entity = null;
		try {
			entity = new StringRequestEntity(WindowsLiveProtocal.getAuthEnv(liveId, livePassword),
					"application/soap+xml", "UTF-8");
		} catch (UnsupportedEncodingException e) {
			assert (false);
		}
		post.setRequestEntity(entity);

		String response = null;
		int result = -1;
		try {
				try {
					result = httpclient.executeMethod(post);
				} catch (Exception e) {
					post.releaseConnection();
				}
			response = post.getResponseBodyAsString();
		} catch (HttpException e) {
			_logger.warn("Token http error.", e);
		} catch (IOException e) {
			_logger.warn("Token http I/O error", e);
		} finally {
			post.releaseConnection();
		}
		
		if (response != null && result >= 200 && result < 300) {
			try {
				token = WindowsLiveProtocal.getTicket(response);
			} catch (DocumentException e) {
				_logger.warn("Get token response document error.", e);
			} catch (ParseException e) {
				_logger.warn("Parse token response document error.", e);
			}
			if(null != token) {
				ret = true;
				_logger.debug("302-302");
			}else{
				_logger.debug("403");
			}
		}
		return ret;
	}

}
