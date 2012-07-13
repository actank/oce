package com.xiaonei.talk.gadget.redirect;

import java.util.HashMap;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;
import mop.hi.oce.adapter.AdapterFactory;
import com.xiaonei.platform.core.opt.base.PassportManager;
import com.xiaonei.talk.TalkProxyAdapter;
import com.xiaonei.talk.util.Domain;
import com.xiaonei.xce.passport.*;

public class RedirectAction extends Action {
	protected TalkProxyAdapter adapter = TalkProxyAdapter.instance();

	public ActionForward execute(ActionMapping mapping, ActionForm form,
			HttpServletRequest request, HttpServletResponse response)
			throws Exception {

		String requestUrl = request.getRequestURL().append("?").append(
				request.getQueryString().toString()).toString();
		// String queryString = request.getQueryString();
		System.out.println("RequestUrl:" + requestUrl);
		boolean isRenRen = (requestUrl.indexOf("http://gadget.talk.renren.com") != -1);
		requestUrl = requestUrl.replaceAll("xiaonei.com", "renren.com");
		if (!isRenRen) {

			response.sendRedirect(requestUrl);
			return null;
		}
		String domain = "renren.com";

		int code = 0;

		String host = request.getParameter("h");
		String ticket = request.getParameter("t");
		String url = request.getParameter("u");
		url = url.replaceAll("xiaonei.com", "renren.com");

		String url1 = url;
		try {

			// System.out.println("host:" + host + " ticket:" + ticket + "url:"+
			// url);
			String[] list = ticket.split("\\$");

			if (!host.equals(list[0])) {
				// System.out.println("ticket is err");
				System.out.println("TEST: host:" + host + "  ticket:" + ticket
						+ "  url:" + url);
				response.sendRedirect("http://www." + domain);

				code = 1;
				return null;
			}

			Cookie[] cookies = request.getCookies();

			String xnticket = null;
			if (cookies != null) {
				for (Cookie cookie : cookies) {
					if (cookie.getName().equalsIgnoreCase("t")) {
						xnticket = cookie.getValue();
						break;
					}
				}
			}
			// boolean hasXNTicket = false;
			// if (xnticket != null) {
			// try {
			// // int id = AdapterFactory.getPassportAdapter().verifyTicket(
			// // xnticket);
			// int id =
			// PassportAdapterFactory.getWebTicket().verifyTicket(xnticket, new
			// HashMap<String,String>());
			//					 
			// if (id == Integer.parseInt(host)) {
			// hasXNTicket = true;
			// }
			// } catch (Exception e) {
			// System.out
			// .println("TEST: do passport verify  ticket:"
			// + xnticket + "  QueryString:"
			// + request.getQueryString() + " err:"
			// + e.toString());
			// e.printStackTrace();
			//
			// }
			// }

			// System.out.println("userid is right");
			// if (!hasXNTicket) {
			boolean flag = adapter.checkTalkTicket(ticket);
			String t = "";
			int count = 0;
			if (flag) {
				while(t.isEmpty() && count++ < 3){
					t = PassportAdapterFactory.getTransfer().createTicket(
						Integer.valueOf(host), 60 * 10);
				}
			}
			// System.out.println(flag);
			// if (flag) {
			// // System.out.println("do redirect");
			// try {
			// System.out.println("do set cookies --------");
			// PassportManager.getInstance().putHostIdtoCookie(
			// response, Integer.parseInt(host), false);
			// } catch (Exception e) {
			// e.printStackTrace();
			// System.out.println("TEST: do putHostIdtoCookie err:"
			// + e.toString());
			// }
			// // response.sendRedirect(URLDecoder.decode(url, "utf8"));
			//
			// }
			// }

			int index = url.indexOf('?');
			String u;
			if (index > 0)
				u = url.substring(0, index);
			else
				u = url;
			// use to stat url
			System.out.println("[STAT]:	HOST:" + host + "	URL:" + u);

			String gotoUrl = "http://goto." + domain + "/share_redirect?url=";
			if (url.indexOf(gotoUrl) == 0) {
				url = url.substring(gotoUrl.length());
				url = java.net.URLDecoder.decode(url, "UTF-8");
				System.out.println(url);
				if (!(url.indexOf(".renren.com") != -1
						|| url.indexOf(".kaixin.com") != -1
						|| url.indexOf(".xiaonei.com") != -1)) {

					
					System.out.println("[STAT]: GOTO_URL:" + url);
					response.sendRedirect(url);
					return null;
				}
			}
			System.out.println("#####");
			if(t.isEmpty()){
				response.sendRedirect(url);
				return null;
			}
			url = java.net.URLEncoder.encode(url, "UTF-8");
			StringBuffer buf = new StringBuffer();
			buf.append("http://passport.renren.com/transfer.do?transfer=")
					.append(t).append("&origURL=").append(url);
			System.out.println("redirect url:" + buf.toString());
			response.sendRedirect(buf.toString());

			// response.getWriter().print(zj.getName()+"love");
			return null;
		} catch (Exception e) {
			code = 2;
			e.printStackTrace();

			System.out.println("TEST url1:" + url1 + "  url:" + url
					+ "  QueryString:" + request.getQueryString() + " err:"
					+ e.toString());
			response.sendRedirect("http://www." + domain);
			return null;
		} finally {
			if (code > 0) {
				System.out.println("[STAT_ERR:" + String.valueOf(code)
						+ "] redirect_url " + request.getRequestURI());
			} else {
				System.out.println("[STAT_SUCC:" + String.valueOf(code)
						+ "] redirect_url " + request.getRequestURI());
			}

		}
	}

}
