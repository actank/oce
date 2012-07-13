package com.xiaonei.talk.gadget.redirect;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;

public class IMHomeAction extends Action {

	private final static String DOMAIN = "renren.com";
	private final static String HOME_REDIRECT = "home";
	private final static String HOME_REDIRECT_URL = "http://im."+DOMAIN+".com/pages/index1.html";

	private final static String POP0_REDIRECT = "pop0";
	private final static String POP0_REDIRECT_URL = "http://im."+DOMAIN+".com/pages/index4.html";

	private final static String POP5_REDIRECT = "pop5";
	private final static String POP5_REDIRECT_URL = "http://im."+DOMAIN+".com/pages/index3.html";

	private final static String OLLIST1_REDIRECT = "xnhome";
	private final static String OLLIST1_REDIRECT_URL = "http://im."+DOMAIN+".com/pages/index2.html";

	public ActionForward execute(ActionMapping mapping, ActionForm form,
			HttpServletRequest request, HttpServletResponse response)
			throws Exception {
		int code = 0;
		try {
			String host = "none";
			Cookie[] cookies = request.getCookies();
			if (cookies != null) {
				for (Cookie cookie : cookies) {
					if (cookie.getName().equalsIgnoreCase("hostid")) {
						host = cookie.getValue();
						break;
					}
				}
			}
			String addr = request.getRemoteAddr();
			String click = request.getParameter("c");
			if (click == null) {
				click = "none";
			}
			System.out.println("[STAT2]: HOST:" + host + "  IP:" + addr
					+ "  CLICK:" + click + "  TYPE:IM_HOME");

			if (click.equalsIgnoreCase(HOME_REDIRECT)) {
				response.sendRedirect(HOME_REDIRECT_URL);
			} else if (click.equalsIgnoreCase(POP0_REDIRECT)) {
				response.sendRedirect(POP0_REDIRECT_URL);
			} else if (click.equalsIgnoreCase(POP5_REDIRECT)) {
				response.sendRedirect(POP5_REDIRECT_URL);
			} else if (click.equalsIgnoreCase(OLLIST1_REDIRECT)) {
				response.sendRedirect(OLLIST1_REDIRECT_URL);
			} else {
				response.sendRedirect(HOME_REDIRECT_URL);
			}
			return null;

		} catch (Exception e) {
			code = 1;
			throw e;
		} finally {
			if(code > 0){
				System.out.println("[STAT_ERR:"+String.valueOf(code)+"] redirect_im_page " + request.getRequestURI());
			}else{
				System.out.println("[STAT_SUCC:"+String.valueOf(code)+"] redirect_im_page "+ request.getRequestURI());
			}
		}
	}
}
