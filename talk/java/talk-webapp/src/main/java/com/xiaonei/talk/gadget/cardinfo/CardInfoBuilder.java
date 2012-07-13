package com.xiaonei.talk.gadget.cardinfo;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;

import javax.servlet.http.HttpServletRequest;

import mop.hi.oce.adapter.BuddyCoreAdapter;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
import mop.hi.oce.adapter.impl.NetworkAdapterImpl;
import mop.hi.oce.domain.buddy.BuddyRelation;
import mop.hi.oce.domain.network.Network;

import com.xiaonei.platform.core.utility.AlbumUtil;
import com.xiaonei.talk.TalkProxyAdapter;
import com.xiaonei.talk.util.HtmlHelper;
import com.xiaonei.xce.XceAdapter;

public class CardInfoBuilder {

	protected static BuddyCoreAdapter buddyCoreAdapter = new BuddyCoreAdapterImpl();
	//protected static PassportAdapterImpl passportAdapter = new PassportAdapterImpl();
	protected static NetworkAdapterImpl networkAdapter = new NetworkAdapterImpl();
	protected static TalkProxyAdapter adapter = TalkProxyAdapter.instance();

	public static String buildXml(HttpServletRequest request) {

		String requestUrl = request.getRequestURL().toString();
//		boolean isRenRen = (requestUrl
//				.indexOf("http://gadget.talk.renren.com") != -1);

		String domain = "renren.com";
//		if(!isRenRen){
//			domain = "xiaonei.com";
//		}
		
		
		
		String host = request.getParameter("h");
		String ticket = request.getParameter("t");
		String guest = request.getParameter("g");
		
//		System.out.println("[DEBUG]: host:"+host+"   ticket:"+ticket+"   guest:"+guest );
		String[] list = ticket.split("\\$");
		
		if(!host.equals(list[0])){
			//System.out.println("ticket is err");
			return "1";
			//return null;
		}
		
		//int hostId = passportAdapter.verifyTicket(t);
		boolean pass = adapter.checkTalkTicket(ticket);
		if(!pass){
			//System.out.println("ticket ->"+ticket+" is wrong");
			return "2";
			//return null;
		}
//		int guestId;
//		if (hostId == -1){
//			System.err.println("hostid is -1");
//			return null;
//		}
//		try {
//			if (hostId != Integer.parseInt(h)){
//				System.err.println("hostid is not self");	
//				return null;
//			}
//			guestId = Integer.parseInt(g);
//		} catch (NumberFormatException e) {
//			e.printStackTrace();
//			return null;
//		}
		int hostId, guestId;
		try{
			hostId = Integer.parseInt(host);
			guestId = Integer.parseInt(guest);
		}catch(NumberFormatException e){
			e.printStackTrace();
			return "3";
			//return null;
		}

		BuddyRelation br = buddyCoreAdapter.getRelation(hostId, guestId);
		//Relation r = relationAdapter.getRelationA(hostId, guestId);
		if (br == null) {
			//System.err.println("there are not friend");
			return "4";
			//return null;
		}
//		if (!((r.getDisc() == Relation.uuFriend) || (r.getDisc() == Relation.uuBlocked))) {
//			System.err.println("host has no right to visit");
//			return null;
//		}
		if(!br.isFriend()){
			//System.err.println("host has no right to visit");
			return "5";
			//return null;
		}

		StringBuffer xml = new StringBuffer();
		xml.append("<?xml version=\"1.0\"?>");
		xml.append("<card xmlns=\"http://talk."+domain+"/cardinfo\" owner=\"")
				.append(guest).append("\">");

		// get network info
		// --------------------------------------

		try {
			List<Network> netlist = networkAdapter.getNetworkList(guestId);
			if (netlist != null) {
				if (netlist.size() > 0) {
					xml.append("<networks>");
					for (int i = 0; i < netlist.size(); ++i) {
						xml.append("<network name=\"").append(
								HtmlHelper.dataFormat(netlist.get(i)
										.getNetworkName())).append("\" url=\"")
								.append("http://"+domain+"/network.do?nk=")
								.append(netlist.get(i).getNetworkId()).append(
										"\" />");
					}
					xml.append("</networks>");
				}else{
					xml.append("<networks/>");
				}
			}else{
				xml.append("<networks/>");
			}
		} catch (Exception e) {
			xml.append("<networks/>");
			e.printStackTrace();
		}

		// get blog info
		// --------------------------------------
		String blogTitle = "";
		String blogId = "";
		String blogContent = "";
		Connection conn = null;
		Statement stat = null;
		try {
			StringBuffer table = new StringBuffer();
			table.append("blog_").append(guestId % 100);
			conn = XceAdapter.getInstance().getReadConnection("blog",table.toString());

			stat = conn.createStatement();
			StringBuffer sql = new StringBuffer();
			sql.append("select id, title, body from ").append(table.toString()).append(" where owner=").append(guest)
					.append(" and blogControl >= 0 order by time desc limit 1");
			ResultSet res = stat.executeQuery(sql.toString());

			while (res.next()) {
				blogTitle = res.getString("title");
				blogId = res.getString("id");
				blogContent = res.getString("body");
			}
			res.close();
			stat.close();
			conn.close();
			
			blogTitle = HtmlHelper.dataFormat(blogTitle);
			if (blogTitle.length() > 50) {
				blogTitle = blogTitle.substring(0, 50);
			}
			//blogTitle = HtmlHelper.dataFormat(blogTitle);

			blogContent = HtmlHelper.dataFormat(blogContent);
			if (blogContent.length() > 100) {
				blogContent = blogContent.substring(0, 100);
			}
			//blogContent = /*HtmlHelper.htmlEncode*/(blogContent);

			if ((!blogTitle.equals("")) && (!blogContent.equals(""))) {
				xml.append("<blog title=\"").append(blogTitle).append(
						"\" href=\"").append(
						"http://blog."+domain+"/GetEntry.do?id=").append(
						blogId).append("&amp;owner=").append(guest).append("\">");
				xml.append("<content>").append(blogContent).append(
						"</content></blog>");
			}

		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			try {
				if (stat != null) {
					stat.close();
				}
				if (conn != null) {
					conn.close();
				}
			} catch (SQLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}

		// get album info
		// -----------------------------------------
		conn = null;
		stat = null;

		StringBuffer table = new StringBuffer();
		table.append("album_").append(guestId % 100);
		try {
			conn = XceAdapter.getInstance().getReadConnection("album",
					table.toString());
			stat = conn.createStatement();
			StringBuffer sql = new StringBuffer();
			sql.append("select * from ").append(table.toString()).append(
					" where albumcontrol >0 and owner = ").append(guest).append(
					" and type != 1 and photo_count > 0 order by id desc limit 5");
			ResultSet res = stat.executeQuery(sql.toString());
			boolean flag = false;
			while (res.next()) {
				if (!flag) {
					flag = true;
					xml.append("<albums>");
				}
				String albumTitle = HtmlHelper.dataFormat(res.getString("name"));
				String albumId = res.getString("id");
				String headUrl = res.getString("headurl");
				xml.append("<album title=\"").append(
						/*HtmlHelper.htmlEncode*/(albumTitle)).append("\" page=\"")
						.append("http://photo."+domain+"/getalbum.do?id=")
						.append(albumId).append("&amp;owner=").append(guest)
						.append("\" img=\"").append(AlbumUtil.getPhotoURL(headUrl))
						.append("\" />");
			}
			if (flag) {
				xml.append("</albums>");
			}
			res.close();
			stat.close();
			conn.close();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

			try {
				
				if (stat != null) {
					stat.close();
				}
				if (conn != null) {
					conn.close();
				}
			} catch (SQLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}

		}

		xml.append("</card>");
		System.out.println("xml:"+xml.toString());
		return xml.toString();
	}



}
