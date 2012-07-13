package com.xiaonei.talk.gadget.app;

import java.io.OutputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.xiaonei.platform.component.application.model.AppBase;
import com.xiaonei.platform.component.application.utils.AppIdCacheManager;
import com.xiaonei.platform.core.menu.UserMenu;
import com.xiaonei.platform.core.menu.UserMenuManager;
import com.xiaonei.platform.core.model.AppMenuItem;
import com.xiaonei.platform.core.opt.base.AppMenuItemCacheMgr;
import com.xiaonei.talk.TalkProxyAdapter;
import com.xiaonei.xce.passport.TicketWebImpl;
public class AppMenuListAction extends Action {
	protected static TalkProxyAdapter adapter = TalkProxyAdapter.instance();
	protected static Set<Integer> blockSet1 = new HashSet<Integer>();
	protected static Set<Integer> blockSet2 = new HashSet<Integer>();
	protected static Set<Integer> blockSet_webgame = new HashSet<Integer>();

	// protected static PassportAdapter passport = new PassportAdapterImpl();
	static {
		blockSet_webgame.add(	117171);
		blockSet_webgame.add(110118);
		blockSet_webgame.add(128926);
		blockSet_webgame.add(150520);
		blockSet_webgame.add(142771);
		blockSet_webgame.add(147798);
		blockSet_webgame.add(57094);
		blockSet_webgame.add(84691);
		blockSet_webgame.add(121130);
		blockSet_webgame.add(126904);
		blockSet_webgame.add(135589);
		blockSet_webgame.add(17);
		blockSet_webgame.add(48);
		blockSet_webgame.add(31623);
		blockSet_webgame.add(49);
		blockSet_webgame.add(39);
		blockSet_webgame.add(104254);
		blockSet_webgame.add(52121);
		blockSet_webgame.add(98905);
		blockSet_webgame.add(109477);
		blockSet_webgame.add(114717);
		blockSet_webgame.add(112106);
		blockSet_webgame.add(114721);
		blockSet_webgame.add(127743);
		blockSet_webgame.add(118253);
		blockSet_webgame.add(119767);
		blockSet_webgame.add(122445);
		blockSet_webgame.add(135571);
		blockSet_webgame.add(130399);
		blockSet_webgame.add(133742);
		blockSet_webgame.add(144825);
		blockSet_webgame.add(141000);
		blockSet_webgame.add(118344);
		blockSet_webgame.add(133743);
		blockSet_webgame.add(119651);
		blockSet_webgame.add(115737);
		blockSet_webgame.add(81224);
		blockSet_webgame.add(83535);
		blockSet_webgame.add(81272);
		blockSet_webgame.add(91327);
		blockSet_webgame.add(81271);
		blockSet_webgame.add(81269);
		blockSet_webgame.add(81270);
		blockSet_webgame.add(51774);
		blockSet_webgame.add(109494);
		blockSet_webgame.add(125666);
		blockSet_webgame.add(11);
		blockSet_webgame.add(141609);
		blockSet_webgame.add(142390);
		blockSet_webgame.add(131709);
		blockSet_webgame.add(98964);
		blockSet_webgame.add(32522);
		blockSet_webgame.add(51);
		blockSet_webgame.add(30);
		blockSet_webgame.add(97559);
		blockSet_webgame.add(123022);
		blockSet_webgame.add(104832);
		blockSet_webgame.add(102751);
		blockSet_webgame.add(97214);
		blockSet_webgame.add(99261);
		blockSet_webgame.add(131669);
		blockSet_webgame.add(132471);
		blockSet_webgame.add(141769);
		blockSet_webgame.add(132354);
		blockSet_webgame.add(132554);
		blockSet_webgame.add(132475);
		blockSet_webgame.add(133682);
		blockSet_webgame.add(143603);
		blockSet_webgame.add(149082);
		blockSet_webgame.add(128938);
		blockSet_webgame.add(114000);
		blockSet_webgame.add(128174);
		blockSet_webgame.add(130419);
		blockSet_webgame.add(124292);
		blockSet_webgame.add(111794);
		blockSet_webgame.add(113884);
		blockSet_webgame.add(143449);
		blockSet_webgame.add(152006);
		blockSet_webgame.add(131827);
		blockSet_webgame.add(94383);
		blockSet_webgame.add(88293);
		blockSet_webgame.add(107401);
		blockSet_webgame.add(88365);
		blockSet_webgame.add(124660);
		blockSet_webgame.add(97873);
		blockSet_webgame.add(105847);
		blockSet_webgame.add(114009);
		blockSet_webgame.add(118212);
		blockSet_webgame.add(86466);
		blockSet_webgame.add(103543);
		blockSet_webgame.add(103063);


		blockSet1.add(45);
		blockSet1.add(42);
		blockSet1.add(33);
		blockSet1.add(29);
		blockSet1.add(26);
		blockSet1.add(21);
		blockSet1.add(20);
		blockSet1.add(19);
		blockSet1.add(15);
		blockSet1.add(14);
		blockSet1.add(13);
		blockSet1.add(10);
		blockSet1.add(9);
		blockSet1.add(8);
		blockSet1.add(7);
		blockSet1.add(6);
		blockSet1.add(4);
		blockSet1.add(3);
		blockSet1.add(2);
		blockSet1.add(115147);
		blockSet1.add(94243);
		blockSet1.add(53209);
		blockSet1.add(51256);
		blockSet1.add(60);
		blockSet1.add(98172);
		blockSet1.add(121160);
		blockSet1.add(98349);

		blockSet2.addAll(blockSet1);
//		blockSet2.add(60);
		blockSet2.add(2);
		blockSet2.add(6);
		blockSet2.add(13);
		blockSet2.add(21);
		blockSet2.add(29);
		blockSet2.add(45);
		blockSet2.add(9999);
		blockSet2.add(10057);
		blockSet2.add(10789);
		blockSet2.add(11494);
		blockSet2.add(12274);
		blockSet2.add(12409);
		blockSet2.add(12603);
		blockSet2.add(13206);
		blockSet2.add(13371);
		blockSet2.add(13388);
		blockSet2.add(13998);
		blockSet2.add(14222);
		blockSet2.add(14645);
		blockSet2.add(15760);
		blockSet2.add(15903);
		blockSet2.add(15991);
		blockSet2.add(16212);
		blockSet2.add(16664);
		blockSet2.add(17960);
		blockSet2.add(18398);
		blockSet2.add(18458);
		blockSet2.add(18690);
		blockSet2.add(18985);
		blockSet2.add(19188);
		blockSet2.add(19246);
		blockSet2.add(19581);
		blockSet2.add(20018);
		blockSet2.add(22162);
		blockSet2.add(22545);
		blockSet2.add(22701);
		blockSet2.add(22986);
		blockSet2.add(23241);
		blockSet2.add(24564);
		blockSet2.add(24766);
		blockSet2.add(24981);
		blockSet2.add(25393);
		blockSet2.add(26769);
		blockSet2.add(27112);
		blockSet2.add(28243);
		blockSet2.add(28508);
		blockSet2.add(29706);
		blockSet2.add(30433);
		blockSet2.add(31409);
		blockSet2.add(32418);
		blockSet2.add(33092);
		blockSet2.add(33160);
		blockSet2.add(36638);
		blockSet2.add(36989);
		blockSet2.add(39179);
		blockSet2.add(51256);
		blockSet2.add(52758);
		blockSet2.add(66756);
		blockSet2.add(79412);
		blockSet2.add(79646);
		blockSet2.add(92896);
		blockSet2.add(95662);
		blockSet2.add(95829);
		blockSet2.add(97509);
		blockSet2.add(99542);
		blockSet2.add(101121);
		blockSet2.add(101362);
		blockSet2.add(101363);
		blockSet2.add(104149);
		blockSet2.add(104693);
		blockSet2.add(112993);
		blockSet2.add(101644);
		blockSet2.add(99542);
		blockSet2.add(99427);
		blockSet2.add(98172);
		blockSet2.add(104149);
		blockSet2.add(73001);
		blockSet2.add(92813);
		blockSet2.add(12530);
		blockSet2.add(11843);
		
		
	}

	@Override
	public ActionForward execute(ActionMapping mapping, ActionForm form,
			HttpServletRequest request, HttpServletResponse response)
			throws Exception {
		int code = 0;
		Set<Integer> blockSet = blockSet1;
		try {
			String queryString = request.getQueryString();
			String requestUrl;
			if (queryString != null) {
				requestUrl = request.getRequestURL().append("?").append(
						request.getQueryString()).toString();
			} else {
				requestUrl = request.getRequestURL().toString();
			}
			boolean isRenRen = (requestUrl .indexOf("http://gadget.talk.renren.com") != -1);
			String ticket = null;
			Cookie[] cookies = request.getCookies();
			if (cookies != null) {
				for (Cookie cookie : cookies) {
					if (cookie.getName().equalsIgnoreCase("t")) {
						ticket = cookie.getValue();
						break;
					}
				}
			}
			boolean hasTicket = false;
			int uid = 0;
			if (ticket != null) {
				uid = TicketWebImpl.getInstance().verifyTicket(ticket,
						new HashMap<String, String>());
				if (uid > 0) {
					hasTicket = true;
				}
			}
			if (!hasTicket) {
				String host = null;
				// if (!hasTicket) {
				host = request.getParameter("h");
				ticket = request.getParameter("t");
				String[] list = ticket.split("\\$");

				if (!host.equals(list[0])) {
					// System.out.println("ticket is err");
					code = 1;
					return null;
				}

				// int hostId = passportAdapter.verifyTicket(t);
				boolean pass = adapter.checkTalkTicket(ticket);
				if (!pass) {
					// System.out.println("ticket ->"+ticket+" is wrong");
					code = 2;
					return null;
				}
				// }

				uid = Integer.parseInt(host);

				

			}
			String selectListFlag = request.getParameter("l");
			boolean isdesktop = true;
			if (selectListFlag != null && selectListFlag.equals("game")) {
				blockSet = blockSet2;
				isdesktop = false;
			}
			UserMenu userMenu = UserMenuManager.getInstance().get(uid);
			int[] appids = userMenu.getRecentAppIds();
			System.out.println("getappmenulist userid:" + String.valueOf(uid) + " appids:" + String.valueOf(appids.length));

			Document document = DocumentHelper.createDocument();
			document.setXMLEncoding("utf-8");

			Element applist = document.addElement("applist").addAttribute( "host", String.valueOf(uid));

			if (!(selectListFlag != null && selectListFlag.equals("game"))) {
				//if(uid == 232626782 || uid == 415345675 || uid == 416269707 || uid == 415565886){
					Element app2 = applist.addElement("app").addAttribute("id","-1001");
					app2.addElement("name").addText("人人桌面周年庆大抽奖");
					app2.addElement("logo").addText("http://im.renren.com/zhuomianlogo.png");
					app2.addElement("logo75").addText("http://im.renren.com/zhuomianlogo.png");
					app2.addElement("url").addText("http://im.renren.com/huodong/zhounianqing.html?subver=18");
			//	}
			}
			if (appids != null) {
				for (int i = 0; i < appids.length; ++i) {
					//System.out.println("appids"+String.valueOf(i)+" = " + String.valueOf(appids[i]));
					AppMenuItem appMenuItem = AppMenuItemCacheMgr.getInstance().get(appids[i]);
					// System.out.println("appID:" + appMenuItem.getAppId() +
					// ", block.size:" + blockSet.size());
					if(appMenuItem == null){
						continue;
						//System.out.println("null item appid:" + String.valueOf(appids[i]));
					}
					if(!isdesktop){
						if(!(blockSet_webgame.contains(appMenuItem.getAppId()))){
							boolean isgame = true;
							try{
								isgame = AppIdCacheManager.getInstance().appTypeMatch(AppIdCacheManager.GAME, String.valueOf(appMenuItem.getAppId()));
							}catch(Exception e){
								System.out.println("call AppIdCacheManager exception appTypeMatch uid:" + String.valueOf(uid));
							}
							//System.out.println("Appid:"+String.valueOf(appMenuItem.getAppId())+" is game:"+String.valueOf(isgame));
							if (!isgame) {
								continue;
							}
						}
					}else{
						if(blockSet.contains(appMenuItem.getAppId())){
							continue;
						}
					}
					String logo75;
					try {
						//System.out.println(appMenuItem.getAppId());
						AppBase appbase = com.xiaonei.platform.component.application.cache.AppBaseCacheMgr.getInstance().get(appMenuItem.getAppId());
						
						logo75 = appbase.getLogo();
					} catch (Exception e) {
						System.out.println("Appid:" + String.valueOf(appMenuItem.getAppId()) + " not found");
						e.printStackTrace();
						continue;
					}

					// System.out.println("app:"+appMenuItem.toString());
					Element app = applist.addElement("app").addAttribute("id", String.valueOf(appMenuItem.getAppId()));
					app.addElement("name").addText(appMenuItem.getAppName());
					app.addElement("logo").addText(appMenuItem.getAppIcon());

					app.addElement("logo75").addText(logo75);
					if (isRenRen) {
						app.addElement("url").addText(appMenuItem.getSideNavUrl());
//						if(appMenuItem.getAppId() == 60) System.out.println("60 appid = "+ appMenuItem.getSideNavUrl());
					} else {
						String url = appMenuItem.getSideNavUrl();
						// url = url.replaceAll("renren.com","xiaonei.com");
						app.addElement("url").addText(url);
					}
				}
			}

			//人人电台
			if (!(selectListFlag != null && selectListFlag.equals("game"))) {
				Element app = applist.addElement("app").addAttribute("id","-1000");
				app.addElement("name").addText("人人电台");
				app.addElement("logo").addText("http://a.xnimg.cn/imgpro/icons/radio.png");
				app.addElement("logo75").addText("http://a.xnimg.cn/imgpro/icons/radio75.png");
				app.addElement("url").addText("http://music.renren.com/fm");
			}
			
			
			String appxml = document.asXML();
			response.setContentType("text/xml");
			OutputStream out = response.getOutputStream();
			out.write(appxml.getBytes());
			out.flush();
			return null;
		} catch (Exception e) {
			code = 3;
			e.printStackTrace();
			throw e;
		} finally {
			if (code > 0) {
				//System.out.println("[STAT_ERR:" + String.valueOf(code) + "] get_app_list " + request.getRequestURI());
			} else {
				//System.out.println("[STAT_SUCC:" + String.valueOf(code) + "] get_app_list " + request.getRequestURI());
			}
		}
	}
}
