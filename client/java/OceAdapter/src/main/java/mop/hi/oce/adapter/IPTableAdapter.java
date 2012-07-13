package mop.hi.oce.adapter;

import java.util.Map;

import mop.hi.svc.iptab.IPTable;
import mop.hi.svc.iptab.UnivInfo;

public interface IPTableAdapter {
	IPTable getIPTable(int id);

	UnivInfo[] getUnivByIp(String ip);

	int[] getUnivIdByIp(String ip);

	IPTable[] getIPTables(int univid);

	boolean isValid(long ip, int univ);

	void addIPTable(Map<String, String> ipt);

	void addUnivInfo(Map<String, String> univ);

	void remove(int id);
}
