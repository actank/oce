package com.xiaonei.talk.gadget.redirect;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;

public class IMDownloadAction extends Action {
	private final static String DOMAIN = "renren.com";
	private final static String DOWNLOAD_URL = "http://im."+DOMAIN+".com/xnsetup.exe";
	private final static String DOWNLOAD_URL2 = "http://im."+DOMAIN+".com/xnsetup2.exe";
	private final static String DOWNLOAD_URL3 = "http://im."+DOMAIN+".com/xnsetup3.exe";
	private static int count = 0;

	public ActionForward execute(ActionMapping mapping, ActionForm form,
			HttpServletRequest request, HttpServletResponse response)
			throws Exception {
		int code = 0;
		try {
			String host = "none";
			Cookie[] cookies = request.getCookies();
			if (cookies != null) {
				for (Cookie cookie : cookies) {
					if (cookie.getName().equalsIgnoreCase("userid")) {
						host = cookie.getValue();
						break;
					}
				}
			}
			String addr = request.getRemoteAddr();
			String download = request.getParameter("d");
			if (download == null) {
				download = "none";
			}
			System.out.println("[STAT2]: HOST:" + host + "  IP:" + addr
					+ "  CLICK:" + download + "  TYPE:IM_DOWNLOAD");

			count++;
			if (count % 2 == 0) {
				response.sendRedirect(DOWNLOAD_URL);
				count = 0;
			} else {
				response.sendRedirect(DOWNLOAD_URL3);
			}

			return null;
		} catch (Exception e) {
			code = 1;
			throw e;
		} finally {
			if(code > 0){
				System.out.println("[STAT_ERR:"+String.valueOf(code)+"] redirect_download " + request.getRequestURI());
			}else{
				System.out.println("[STAT_SUCC:"+String.valueOf(code)+"] redirect_download "+ request.getRequestURI());
			}
		}
	}
}
