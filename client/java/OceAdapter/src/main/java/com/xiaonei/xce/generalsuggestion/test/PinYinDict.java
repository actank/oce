package com.xiaonei.xce.generalsuggestion.test;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import com.xiaonei.xce.XceAdapter;

public class PinYinDict {
	public static PinYinDict instance = new PinYinDict();
	public static PinYinDict getInstance() {
		return instance;
	}
	
	private Map<String, Set<String>> dict;
	private PinYinDict() {
		dict = new HashMap<String, Set<String>>();
	}
	
	public void init() {
		Connection conn = null;
		Statement stat = null;
		String sql = "SELECT hanzi, pinyin FROM pinyin";
		try {
			conn = XceAdapter.getInstance().getReadConnection("friendinput_pinyin");
			stat = conn.createStatement();
			ResultSet rs = stat.executeQuery(sql);
			while(rs.next()) {
				String hanzi = rs.getString("hanzi");
				String pinyin = rs.getString("pinyin");
				Set<String> pinyins = dict.get(hanzi);
				if (pinyins == null) {
					pinyins = new HashSet<String>();
					dict.put(hanzi, pinyins);
				}
				pinyins.add(pinyin);
			}
		} catch (SQLException e) {
			e.printStackTrace();
		} finally {
			if (stat != null){
				try {
					stat.close();
				} catch (SQLException e) {
				}
			}
			if (conn != null) {
				try {
					conn.close();
				} catch (SQLException e) {
				}
			}
		}
	}
	
	public Set<String> getPinyin(String hanzi) {
		return dict.get(hanzi);
	}
}
