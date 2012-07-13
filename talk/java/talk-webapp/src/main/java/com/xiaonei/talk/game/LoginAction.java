package com.xiaonei.talk.game;

import java.io.OutputStream;
import java.util.HashMap;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;

import com.xiaonei.xce.passport.TicketWebImpl;
import com.xiaonei.platform.core.model.User;
import com.xiaonei.platform.core.opt.ice.WUserAdapter;

public class LoginAction extends Action {

	private static String head = "<html>"
			+ "<head>"
			+ "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"
			+ "<title>人人网</title>" + "</head>"
			+ "<body id=\"registerpage\" class=\"login\">";
	private static String loginForm = "<form method=\"post\" action=\"http://passport.renren.com/RL.do\" focus=\"email\">"
			+ "<label for=\"email\">帐号:</label>"
			+ "<input type=\"text\" id=\"email\" name=\"email\" tabindex=\"1\" value=\"\" />"
			+ "<p />"
			+ "<label for=\"password\">密码:</label>"
			+ "<input type=\"password\" id=\"password\" name=\"password\" tabindex=\"2\"  value=\"\" />"
			+ "<p />"
			+ "<a tabindex=\"5\" href='http://safe.renren.com/findPass.do'>取回密码</a>"

			+ "<label for=\"autoLogin\"><input type=\"checkbox\" name=\"autoLogin\" id=\"autoLogin\" value=\"true\" tabindex=\"4\" class=\"input-checkbox\" />下次自动登录</label>"
			+ "<input type=\"hidden\" name=\"origURL\" value=\"http://game.talk.renren.com/login.do\" />"
			+ "<input type=\"hidden\" name=\"domain\" value=\"renren.com\" />"
			+ "<input type=\"hidden\" name=\"needNotify\" value=\"false\" />"
			+ "<input type=\"hidden\" name=\"formName\" value=\"\" />"
			+ "<input type=\"hidden\" name=\"method\" value=\"\" />"
			+ "<input type=\"submit\" id=\"login\" tabindex=\"4\" name=\"submit\" class=\"input-submit large\" value=\"登录\" />"
			+ "</form>";

	private static String tail = "</body></html>";

	public ActionForward execute(ActionMapping mapping, ActionForm form,
			HttpServletRequest request, HttpServletResponse response)
			throws Exception {

		String errCode = request.getParameter("errCode");
		if (errCode == null) {

			// String cookieName;
			String ticket = null;
			Cookie[] cookies = request.getCookies();
			if (cookies != null) {
				for (Cookie cookie : cookies) {
					// if (cookie.getName().equalsIgnoreCase("id")) {
					// cookieName = cookie.getValue();
					// }
					if (cookie.getName().equalsIgnoreCase("t")) {
						ticket = cookie.getValue();
						break;
					}

				}
				// boolean success = false;
				int uid = -1;
				if (ticket != null) {
					uid = TicketWebImpl.getInstance().verifyTicket(ticket,
							new HashMap<String, String>());
					// if (uid > 0) {
					// success = true;
					// }
				}

				if (uid > 0) {
					// Êä³öÐÅÏ¢
					System.out.println("uid:" + String.valueOf(uid));
					User u = WUserAdapter.getInstance().get(uid);

					response.setContentType("text/html");
					StringBuffer buf = new StringBuffer();
//					StringBuffer body = new StringBuffer();
					buf.append(head).append("<img id=\"head\" src=\"").append(
							u.getTinyFullUrl()).append(
							"\" />").append("<img id=\"headurl\" src=\"").append(u.getHeadFullUrl()).append("\" />").append("<span id=\"name\">").append(u.getName())
							.append("</span>").append("<span id=\"time\">").append(System.currentTimeMillis()/1000).append("</span>").append(tail);

					OutputStream out = response.getOutputStream();
					out.write(buf.toString().getBytes());
					out.flush();
				} else {
					// Ìø×ª
					response
							.sendRedirect("http://game.talk.renren.com/login.do?errCode=8");
				}
			}
		} else {

			/*
			 * switch(Integer.parseInt(request.getParameter("errorCode"))){ case
			 * 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case
			 * 9: default: response.setContentType("text/html"); PrintWriter out
			 * = response.getWriter(); out.println("Error code 8!");
			 * out.flush(); break; }
			 */
			response.setContentType("text/html");
			StringBuffer buf = new StringBuffer();
			buf.append(head).append("error ").append(errCode).append(loginForm)
					.append(tail);
			OutputStream out = response.getOutputStream();
			out.write(buf.toString().getBytes());
			out.flush();
		}
		return null;

	}
}
