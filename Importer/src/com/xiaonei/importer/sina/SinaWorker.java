package com.xiaonei.importer.sina;

import java.io.IOException;
import java.util.UUID;

import org.apache.commons.httpclient.Header;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpConnectionManager;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.MultiThreadedHttpConnectionManager;
import org.apache.commons.httpclient.cookie.CookiePolicy;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.params.DefaultHttpParams;
import org.apache.commons.httpclient.params.HttpConnectionManagerParams;
import org.apache.commons.httpclient.params.HttpMethodParams;

import com.xiaonei.importer.AbstractWorker;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;

public class SinaWorker extends AbstractWorker {
	
	private final String uid;
	private final String passwd;
	
	private String host;
	private StringBuilder cookie = new StringBuilder();
	private BuddyInfo[] _buddylist;

	protected SinaWorker(String uid, String passwd, UUID requestId, WorkerCallback callback) {
		super(requestId, callback);
		this.uid = uid;
		this.passwd = passwd;
	}

	public void run() {
		boolean auth = getAuth();

		BuddyListStatus status = null;
		if (auth) {
			status = new BuddyListStatus(302, "Token OK, needs contacts");
		} else {
			status = new BuddyListStatus(403, "Auth failed.");
		}
		BuddyList list = InfomationCache.getInstance().getContacts(
				this._requestId);
		list.status = status;
		
		if (auth) {
			exportContacts();
		}
	}
	
	private static HttpClient httpclient;
	static {
		DefaultHttpParams.getDefaultParams().setBooleanParameter(HttpMethodParams.SINGLE_COOKIE_HEADER, true);
		DefaultHttpParams.getDefaultParams().setParameter(HttpMethodParams.COOKIE_POLICY, CookiePolicy.IGNORE_COOKIES);

		HttpConnectionManager manager = new MultiThreadedHttpConnectionManager();
		HttpConnectionManagerParams parm = new HttpConnectionManagerParams();
		parm.setDefaultMaxConnectionsPerHost(64);
		parm.setMaxTotalConnections(128);
		parm.setStaleCheckingEnabled(true);
		manager.setParams(parm);
		httpclient = new HttpClient(manager);
	}
	
	boolean getAuth() {
		String url = "http://mail.sina.com.cn/cgi-bin/login.cgi";
		PostMethod post = new PostMethod(url);
		post.addParameter("logintype", "uid");
		post.addParameter("u", uid);
		post.addParameter("psw", passwd);
		post.addParameter("btnloginfree", "%B5%C7+%C2%BC");
		post.setRequestHeader("Cookie", "sina_free_mail_recid=false; sina_vip_mail_recid=false");
		post.setRequestHeader("Referer", "http://mail.sina.com.cn/index.html");
		post.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		post.setFollowRedirects(false);
		
		try {
			httpclient.executeMethod(post);
		} catch (HttpException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			post.releaseConnection();
		}
		if(post.getStatusCode() != 302) {
			return false;
		}
		for(Header head : post.getResponseHeaders("Set-Cookie")) {
			if(cookie.length() != 0) {
				cookie.append("; ");
			}
			cookie.append(head.getValue().split(";")[0]);
		}
		
		String location =  post.getResponseHeader("Location").getValue();
		host = location.split("//")[1].split("/")[0];
		GetMethod get = new GetMethod(location);
		get.setRequestHeader("Cookie", cookie.toString());
		get.setFollowRedirects(false);
		
		try {
			httpclient.executeMethod(get);
		} catch (HttpException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		String swsid = null;
		try {
			String s = get.getResponseHeader("Set-Cookie").getValue();
			swsid = s.split(";")[0];
		} catch(Exception e) {
			e.printStackTrace();
		}
		if(swsid == null || swsid.length() == 0) {
			return false;
		}
		cookie.append("; ").append(swsid);
		
		return true;
	}
	
	private void exportContacts() {
		String url = "http://" + host + "/classic/addr_export.php";
		PostMethod post = new PostMethod(url);
		post.setRequestHeader("Cookie", cookie.toString());
		post.addParameter("extype", "csv");
		
		String text = null;
		try {
			httpclient.executeMethod(post);

			text = post.getResponseBodyAsString();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			post.releaseConnection();
		}
		
		if(text != null) {
			try {
				_buddylist = SinaProtocal.prase(text);
				BuddyListStatus status = new BuddyListStatus(200, "OK");
				_callback.finishContactList(_requestId, _buddylist, status);
			} catch(Exception e) {
				e.printStackTrace();
			}
		}

	}
}
