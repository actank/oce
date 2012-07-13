package com.xiaonei.importer.yahoo;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.UUID;

import org.apache.commons.httpclient.Header;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpConnectionManager;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.MultiThreadedHttpConnectionManager;
import org.apache.commons.httpclient.cookie.CookiePolicy;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.StringRequestEntity;
import org.apache.commons.httpclient.params.DefaultHttpParams;
import org.apache.commons.httpclient.params.HttpConnectionManagerParams;
import org.apache.commons.httpclient.params.HttpMethodParams;

import com.xiaonei.importer.AbstractWorker;
import com.xiaonei.importer.WorkerCallback;
import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyListStatus;

public class YWorker extends AbstractWorker {
	
	private final String uid;
	private final String passwd;
	private StringBuilder cookie = new StringBuilder();
	
	private BuddyInfo[] _buddylist;

	protected YWorker(String uid, String passwd, UUID requestId, WorkerCallback callback) {
		super(requestId, callback);
		this.uid = uid;
		this.passwd = passwd;
		// TODO Auto-generated constructor stub
	}

	public void run() {
		boolean auth = getAuth();
		
		BuddyListStatus status = null;
		if(auth) {
			status = new BuddyListStatus(302, "Token OK, needs conntacts");
		} else {
			status = new BuddyListStatus(403, "Auth failed.");
		}
		BuddyList list = InfomationCache.getInstance().getContacts(this._requestId);
		list.status = status;
		
		if(auth) {
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
	
	private boolean getAuth() {
//		String url = "http://edit.bjs.yahoo.com/config/login";
//		PostMethod post = new PostMethod(url);
//		String postBody = ".intl=cn&.done=http%3A%2F%2Fmail.cn.yahoo.com%2Fycn_setcookie.html%3F.done%3Dhttp%253A%252F%252Fmail.cn.yahoo.com%252Finset.html%253Frr%253D195918111&.src=ym&.cnrid=ymhp_&login=" 
//			+ uid.replaceAll("@", "%40") +"&passwd=" + passwd+ "&submit=";
//		post.setFollowRedirects(false);
//		
//		try {
//			post.setRequestEntity(new StringRequestEntity(postBody, "application/xml", "UTF-8"));
//			httpclient.executeMethod(post);
//		} catch (UnsupportedEncodingException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} catch (HttpException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} finally {
//			post.releaseConnection();
//		}

		String url = "https://edit.bjs.yahoo.com/config/login";
		PostMethod post = new PostMethod(url);
		post.addParameter("login", uid);
		post.addParameter("passwd", passwd);
		post.addParameter(".intl", "cn");
		post.addParameter(".done", "http%3A%2F%2Fmail.cn.yahoo.com%2Finset.html%3Frr%3D195918111");
		post.addParameter(".src", "ym");
		post.addParameter(".cnrid", "ymhp_");
		post.addParameter(".challenge", "ZvbIiAn0OF8PaztJzt1Kh8llXh9f");
		try {
			httpclient.executeMethod(post);
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (HttpException e) {
			// TODO Auto-generated catch block 
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			post.releaseConnection();
		}
		if(post.getStatusCode() != 302)
			return false;
		
		for(Header head : post.getResponseHeaders("Set-Cookie")) {
			if(cookie.length() != 0) {
				cookie.append("; ");
			}
			cookie.append(head.getValue().split(";")[0]);
		}
		
		return true;
	}
	
	void exportContacts() {
		GetMethod get = new GetMethod("http://address.mail.yahoo.com/?1&VPC=import_export");
		get.setRequestHeader("Cookie", cookie.toString());
		get.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		get.setRequestHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		String crumb = null;
		try {
			httpclient.executeMethod(get);
			String text = get.getResponseBodyAsString();
			int i1 = text.indexOf("name=\".crumb\"");
			int i2 = text.indexOf('>', i1) - 1;
			String str = text.substring(i1, i2);
			for(String s : str.split(" ")) {
				if(s.startsWith("value")) {
					str = s;
					break;
				}
			}
			crumb = str.split("\"")[1];
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			get.releaseConnection();
		}
		if(crumb == null) {
			return;
		}
		
		PostMethod post = new PostMethod("http://address.mail.yahoo.com/index.php");
		post.setRequestHeader("Cookie", cookie.toString());
		post.setRequestHeader("Referer", "'http://address.mail.yahoo.com/?1&VPC=import_export");
		post.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		post.setRequestHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		String postBody =  ".crumb=" + crumb +"&VPC=import_export&submit%5Baction_export_yahoo%5D=%E7%AB%8B%E5%8D%B3%E5%AF%BC%E5%87%BA";
		String text = null;
		try {
			post.setRequestEntity(new StringRequestEntity(postBody, "application/xml", "UTF-8"));
			httpclient.executeMethod(post);
			text = post.getResponseBodyAsString();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			post.releaseConnection();
		}
		
		if(text != null) {
			try {
				_buddylist = YahooProtocal.prase(text);
				BuddyListStatus status = new BuddyListStatus(200, "OK");
				_callback.finishContactList(_requestId, _buddylist, status);
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
	}

}
