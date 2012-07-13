package com.xiaonei.importer.WindowsLive;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.util.UUID;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpConnectionManager;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.MultiThreadedHttpConnectionManager;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.RequestEntity;
import org.apache.commons.httpclient.methods.StringRequestEntity;
import org.apache.commons.httpclient.params.HttpConnectionManagerParams;
import org.dom4j.DocumentException;

import com.sun.corba.se.spi.orbutil.fsm.Guard.Result;
import com.xiaonei.importer.AbstractWorker;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyListStatus;

public class WindowsLiveWorker extends AbstractWorker implements Runnable {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(WindowsLiveWorker.class);

	private final String _liveId;
	private final String _livePassword;

	private WindowsLiveAuthToken _token;
	private BuddyInfo[] _buddylist;

	public WindowsLiveWorker(String liveId, String livePassword, UUID requestId, WorkerCallback callback) {
		super(requestId, callback);
		_liveId = liveId;
		_livePassword = livePassword;
	}

	public void run() {
		_logger.trace("GMailWorker.run: " + _liveId + ", " + _livePassword +": " + _requestId);
		if (getToken()) {
			getContacts();
		}
	}

	private static HttpClient httpclient;
	static {
		HttpConnectionManager manager = new MultiThreadedHttpConnectionManager();
		HttpConnectionManagerParams parm = new HttpConnectionManagerParams();
		parm.setDefaultMaxConnectionsPerHost(64);
		parm.setMaxTotalConnections(128);
		parm.setStaleCheckingEnabled(true);
		manager.setParams(parm);
		httpclient = new HttpClient(manager);
	}
	private boolean getToken() {
		boolean ret = false;
		PostMethod post = new PostMethod(WindowsLiveProtocal.wstEndpoints);
		RequestEntity entity = null;
		try {
			entity = new StringRequestEntity(WindowsLiveProtocal.getAuthEnv(_liveId, _livePassword),
					"application/soap+xml", "UTF-8");
		} catch (UnsupportedEncodingException e) {
			assert (false);
		}
		post.setRequestEntity(entity);

		String response = null;
		int result = -1;
		try {
			while (result < 0 && result > -20) {
				try {
					result = httpclient.executeMethod(post);
				} catch (Exception e) {
					result--;
					_logger.error(result, e);
					post.releaseConnection();
				}
			}
			response = post.getResponseBodyAsString();
			if (_logger.isTraceEnabled()) {
				_logger.trace("Token Response body: " + response);
			}
		} catch (HttpException e) {
			_logger.warn("Token http error.", e);
		} catch (IOException e) {
			_logger.warn("Token http I/O error", e);
		} finally {
			if(result != -20)
				post.releaseConnection();
		}

		if (response != null && result >= 200 && result < 300) {
			try {
				_token = WindowsLiveProtocal.getTicket(response);
			} catch (DocumentException e) {
				_logger.warn("Get token response document error.", e);
			} catch (ParseException e) {
				_logger.warn("Parse token response document error.", e);
			}
			if (_logger.isTraceEnabled() && null != _token) {
				_logger.trace("Ticket is: " + _token.token);
				_logger.trace("Created: " + _token.validateFrom.toString());
				_logger.trace("Expires: " + _token.validateUntil.toString());
			}
			if (_callback instanceof WindowsLiveImporter) {
				BuddyListStatus status = null;
				if (null != _token) {
					status = new BuddyListStatus(302, "Token OK, needs contacts");
					_logger.debug("302");
					ret = true;
				} else {
					status = new BuddyListStatus(403, "Auth failed.");
					_logger.debug("403");
					ret = false;
				}
				WindowsLiveImporter new_name = (WindowsLiveImporter) _callback;
				new_name.finishAuthToken(_requestId, _token, status);
			} else {
				ret = false;
			}
		} else {
			ret = false;
		}
		return ret;
	}

	private void getContacts() {
		GetMethod request = new GetMethod(WindowsLiveProtocal.contactsEndpoints + "/" + _liveId
				+ "/LiveContacts/contacts");
		request.setRequestHeader(WindowsLiveProtocal.contactsHeaderName, WindowsLiveProtocal
				.getContactHeaderValue(_token.token));

		String response = null;
		int result = -1;
		try {
			while (result < 0 && result > -20) {
				try {
					result = httpclient.executeMethod(request);
				} catch (Exception e) {
					result--;
					_logger.error(result, e);
					request.releaseConnection();
				}
			}
			if (_logger.isTraceEnabled()) {
				_logger.trace("/: " + result);
				_logger.trace("Get Response body: ");
			}
			if(request.getStatusCode() == 200){
				response = request.getResponseBodyAsString().substring(1);
			}else if(request.getStatusCode() == 404) {
				BuddyListStatus status = new BuddyListStatus(404, "NO FRIENDS");
				_buddylist = new BuddyInfo[0];
				_callback.finishContactList(_requestId, _buddylist, status);
				return;
			} else if(request.getStatusCode() == 500) {
				BuddyListStatus status = new BuddyListStatus(500, "MSNServer Error");
				_buddylist = new BuddyInfo[0];
				_callback.finishContactList(_requestId, _buddylist, status);
				return;
			} else{
				BuddyListStatus status = new BuddyListStatus(503, "Interal Error");
				_buddylist = new BuddyInfo[0];
				_callback.finishContactList(_requestId, _buddylist, status);
				return;
			}
			if (_logger.isTraceEnabled()) {
				_logger.trace(response);
			}
		} catch (HttpException e) {
			_logger.warn("Contacts http error.", e);
		} catch (IOException e) {
			_logger.warn("Contacts http I/O error.", e);
		} finally {
			if(result != -20)
				request.releaseConnection();
		}

		if (response != null) {
			try {
				_buddylist = WindowsLiveProtocal.getBuddInfos(response);
				BuddyListStatus status = new BuddyListStatus(200, "OK");
				_callback.finishContactList(_requestId, _buddylist, status);
			} catch (DocumentException e) {
				e.printStackTrace();
			}
		}

	}

}
