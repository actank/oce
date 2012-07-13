package com.xiaonei.importer.google;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import junit.framework.TestCase;

import org.apache.commons.httpclient.Header;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpConnectionManager;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.MultiThreadedHttpConnectionManager;
import org.apache.commons.httpclient.NameValuePair;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.params.HttpConnectionManagerParams;

public class ImportGMail extends TestCase {

	private HttpClient client;

	protected void setUp() throws Exception {
		HttpConnectionManager cm = new MultiThreadedHttpConnectionManager();
		HttpConnectionManagerParams cmParms = new HttpConnectionManagerParams();
		cmParms.setMaxTotalConnections(128);
		cmParms.setDefaultMaxConnectionsPerHost(64);
		cm.setParams(cmParms);
		client = new HttpClient(cm);
	}

	public void testGMail() throws InterruptedException {
		Map<String, String> users = new HashMap<String, String>();
		List<Thread> ts = new LinkedList<Thread>();
		users.put("importerxn@gmail.com", "123test456");
		//users.put("", "");
		for (Entry<String, String> entry : users.entrySet()) {
			Worker t = new Worker(entry.getKey(), entry.getValue());
			t.start();
			ts.add(t);
		}
		
		Thread.sleep(30*1000);
	}

	public  class Worker extends Thread{
		private final String _u;
		private final String _p;
		public Worker(String u, String p){
			_u=u;
			_p=p;
		}
		@Override
		public void run() {
			String token = gmailAuth(_u, _p);
			System.err.println(token);
			Map<String, String> result = gmailExport(token);
			for (Entry<String, String> e : result.entrySet()) {
				System.out.println(_u + " : " + e.getKey() + " = " + e.getValue());
			}
		}
	}
	
	private Map<String, String> gmailExport(String token) {
		Map<String, String> contacts = new HashMap<String, String>();
		GetMethod get = new GetMethod("https://mail.google.com/mail/contacts/data/export");
		get.setRequestHeader("Authorization", "GoogleLogin " + token);
		NameValuePair[] params = new NameValuePair[] { new NameValuePair("exportType", "ALL"),
				new NameValuePair("groupToExport", ""), new NameValuePair("out", "OUTLOOK_CSV") };
		get.setQueryString(params);
		try {
			int result = client.executeMethod(get);
			System.out.println("Result: " + result);

			Header[] rh = get.getResponseHeaders();
			System.out.println("Headers: ");
			for (Header h : rh) {
				System.out.println(h.getName() + " => " + h.getValue());
			}

			String response = get.getResponseBodyAsString();
			System.out.println("Response: ");
			System.out.println(response);
			System.out.println(":End Response");

			if (result == 200) {
				String[] res = response.split("\n");
				for (String r : res) {
					String[] fields = r.split(",");
					if (fields[1].contains("@")) {
						contacts.put(fields[0], fields[1]);
					}
				}
			}
		} catch (HttpException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			get.releaseConnection();
		}
		return contacts;
	}

	private String gmailAuth(String username, String passwd) {
		String authToken = null;
		PostMethod auth = new PostMethod("https://www.google.com/accounts/ServiceLoginAuth?service=mail");
		// auth.setParameter("Content-Type",
		// "application/x-www-form-urlencoded");
		// auth.addParameter("accountType", "GOOGLE");
		auth.addParameter("Email", username);
		auth.addParameter("Passwd", passwd);
		auth.addParameter("service", "mail");
		// auth.addParameter("PersistentCookie", "no");
		// auth.addParameter("source", "XiaoNei-Importer-0.1");
		try {
			int result = client.executeMethod(auth);
			System.out.println("Result: " + result);

			String response = auth.getResponseBodyAsString();
			System.out.println("Response: ");
			System.out.println(response);
			if (result == 302) {
				Header[] rh = auth.getResponseHeaders();
				System.out.println("Headers: ");
				for (Header h : rh) {
					System.out.println(h.getName() + " => " + h.getValue());
					if (h.getName().equals("Set-Cookie")) {
						authToken = h.getValue();
					}
				}
			}
		} catch (HttpException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			auth.releaseConnection();
		}
		return authToken;
	}
}
