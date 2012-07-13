package mop.hi.oce.adapter.impl;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import mop.hi.oce.adapter.Antispam2Adapter;
import mop.hi.svc.spam.AntispamManager2Prx;
import mop.hi.svc.spam.AntispamManager2PrxHelper;
import mop.hi.svc.spam.CheckAns;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class Antispam2AdapterImpl extends ServiceAdapter implements
		Antispam2Adapter {
	private static final String endpoints = "@Antispam2";
	Map<Character, Character> charSet = new HashMap<Character, Character>();

	private Vector<AntispamManager2Prx> managers = new Vector<AntispamManager2Prx>();
	private Vector<AntispamManager2Prx> managersOneway = new Vector<AntispamManager2Prx>();

	public int check(int fromId, int toid, int type, String text) {
		String tmp = htmlFilter(text);
		return getAntispamManager2(fromId).check(fromId, toid, type, tmp,
				tmp.hashCode());
	}

	public CheckRet check2(int fromId, int toid, int type, String text) {
		String tmp = htmlFilter(text);
		CheckAns ans = getAntispamManager2(fromId).check2(fromId, toid, type,
				tmp, tmp.hashCode());
		CheckRet res = new CheckRet();
		res.type = ans.type;
		res.order = ans.order;
		return res;
	}

	public void addNewDictWord(String word, int type) {
		getAntispamManager2Oneway(type).addDictWord(word, type);
	}

	public void control(Map<String, Integer> props) {
		getAntispamManager2Oneway(0).control(props);
	}

	public void addNewSpam(String text) {
		getAntispamManager2Oneway(0).addNewSpam(text);
	}

	// ---------------------------------------------------
	public String htmlFilter(String text) {
		StringBuffer tmp = new StringBuffer();
		text = text.toLowerCase();
		for (int i = 0; i < text.length(); ++i) {
			Character ch = charSet.get(Character.valueOf(text.charAt(i)));
			if (ch != null) {
				tmp.append(ch.toString());
				continue;
			}
			// if ((text.charAt(i) == ' ') || (text.charAt(i) == '\t')
			// || (text.charAt(i) == '\n') || (text.charAt(i) == '\r')) {
			// continue;
			// }

			if (text.charAt(i) == '&') {
				int index = i + 1;
				if (index >= text.length()) {
					break;
				}
				int pos = -1;
				if (text.charAt(index) == '#') {
					for (int j = index + 1; (j < text.length())
							&& (j < index + 11); ++j) {
						if (text.charAt(j) >= '0' && text.charAt(j) <= '9') {
							pos = j;
						} else
							break;
					}
					if (pos != -1) {
						i = pos;
						++pos;
						if (pos < text.length() && text.charAt(pos) == ';') {
							i++;
						}
						continue;
					}
				} else {
					for (int j = index; (j < text.length()) && (j < index + 10); ++j) {
						if (text.charAt(j) >= 'a' && text.charAt(j) <= 'z') {
							pos = j;
						} else
							break;
					}
					if (pos != -1) {
						i = pos;
						++pos;
						if (pos < text.length() && text.charAt(pos) == ';') {
							i++;
						}
						continue;
					}
				}
			}
			tmp.append(text.charAt(i));
		}
		text = tmp.toString();
		// ------------------------------------------
		StringBuffer buf = new StringBuffer();
		for (int i = 0; i < text.length(); ++i) {
			if (text.charAt(i) == '<') {
				for (int index = i + 1; index < text.length(); ++index) {
					if (text.charAt(index) == '<') {
						buf.append(text.substring(i, index));
						i = index - 1;
						break;
					}
					if (text.charAt(index) == '>') {

						if ((text.charAt(i + 1) == 'a')
								|| text.substring(i + 1, i + 1 + 3).equals(
										"img")) {
							buf.append(text.substring(i, index + 1));
							i = index;
							break;
						} else {
							i = index;
							break;
						}

					}
				}
			} else
				buf.append(text.charAt(i));
		}

		return buf.toString();

	}

	public Antispam2AdapterImpl() {
		super(endpoints, 1, Channel.newOceChannel(endpoints));
		charSet.put(Character.valueOf('１'), Character.valueOf('1'));
		charSet.put(Character.valueOf('２'), Character.valueOf('2'));
		charSet.put(Character.valueOf('３'), Character.valueOf('3'));
		charSet.put(Character.valueOf('４'), Character.valueOf('4'));
		charSet.put(Character.valueOf('５'), Character.valueOf('5'));
		charSet.put(Character.valueOf('６'), Character.valueOf('6'));
		charSet.put(Character.valueOf('７'), Character.valueOf('7'));
		charSet.put(Character.valueOf('８'), Character.valueOf('8'));
		charSet.put(Character.valueOf('９'), Character.valueOf('9'));
		charSet.put(Character.valueOf('０'), Character.valueOf('0'));

		charSet.put(Character.valueOf('ａ'), Character.valueOf('a'));
		charSet.put(Character.valueOf('ｂ'), Character.valueOf('b'));
		charSet.put(Character.valueOf('ｃ'), Character.valueOf('c'));
		charSet.put(Character.valueOf('ｄ'), Character.valueOf('d'));
		charSet.put(Character.valueOf('ｅ'), Character.valueOf('e'));
		charSet.put(Character.valueOf('ｆ'), Character.valueOf('f'));
		charSet.put(Character.valueOf('ｇ'), Character.valueOf('g'));
		charSet.put(Character.valueOf('ｈ'), Character.valueOf('h'));
		charSet.put(Character.valueOf('ｉ'), Character.valueOf('i'));
		charSet.put(Character.valueOf('ｊ'), Character.valueOf('j'));
		charSet.put(Character.valueOf('ｋ'), Character.valueOf('k'));
		charSet.put(Character.valueOf('ｌ'), Character.valueOf('l'));
		charSet.put(Character.valueOf('ｍ'), Character.valueOf('m'));
		charSet.put(Character.valueOf('ｎ'), Character.valueOf('n'));
		charSet.put(Character.valueOf('ｏ'), Character.valueOf('o'));
		charSet.put(Character.valueOf('ｐ'), Character.valueOf('p'));
		charSet.put(Character.valueOf('ｑ'), Character.valueOf('q'));
		charSet.put(Character.valueOf('ｒ'), Character.valueOf('r'));
		charSet.put(Character.valueOf('ｓ'), Character.valueOf('s'));
		charSet.put(Character.valueOf('ｔ'), Character.valueOf('t'));
		charSet.put(Character.valueOf('ｕ'), Character.valueOf('u'));
		charSet.put(Character.valueOf('ｖ'), Character.valueOf('v'));
		charSet.put(Character.valueOf('ｗ'), Character.valueOf('w'));
		charSet.put(Character.valueOf('ｘ'), Character.valueOf('x'));
		charSet.put(Character.valueOf('ｙ'), Character.valueOf('y'));
		charSet.put(Character.valueOf('ｚ'), Character.valueOf('z'));

		charSet.put(Character.valueOf('Ａ'), Character.valueOf('a'));
		charSet.put(Character.valueOf('Ｂ'), Character.valueOf('b'));
		charSet.put(Character.valueOf('Ｃ'), Character.valueOf('c'));
		charSet.put(Character.valueOf('Ｄ'), Character.valueOf('d'));
		charSet.put(Character.valueOf('Ｅ'), Character.valueOf('e'));
		charSet.put(Character.valueOf('Ｆ'), Character.valueOf('f'));
		charSet.put(Character.valueOf('Ｇ'), Character.valueOf('g'));
		charSet.put(Character.valueOf('Ｈ'), Character.valueOf('h'));
		charSet.put(Character.valueOf('Ｉ'), Character.valueOf('i'));
		charSet.put(Character.valueOf('Ｊ'), Character.valueOf('j'));
		charSet.put(Character.valueOf('Ｋ'), Character.valueOf('k'));
		charSet.put(Character.valueOf('Ｌ'), Character.valueOf('l'));
		charSet.put(Character.valueOf('Ｍ'), Character.valueOf('m'));
		charSet.put(Character.valueOf('Ｎ'), Character.valueOf('n'));
		charSet.put(Character.valueOf('Ｏ'), Character.valueOf('o'));
		charSet.put(Character.valueOf('Ｐ'), Character.valueOf('p'));
		charSet.put(Character.valueOf('Ｑ'), Character.valueOf('q'));
		charSet.put(Character.valueOf('Ｒ'), Character.valueOf('r'));
		charSet.put(Character.valueOf('Ｓ'), Character.valueOf('s'));
		charSet.put(Character.valueOf('Ｔ'), Character.valueOf('t'));
		charSet.put(Character.valueOf('Ｕ'), Character.valueOf('u'));
		charSet.put(Character.valueOf('Ｖ'), Character.valueOf('v'));
		charSet.put(Character.valueOf('Ｗ'), Character.valueOf('w'));
		charSet.put(Character.valueOf('Ｘ'), Character.valueOf('x'));
		charSet.put(Character.valueOf('Ｙ'), Character.valueOf('y'));
		charSet.put(Character.valueOf('Ｚ'), Character.valueOf('z'));
	}

	private AntispamManager2Prx getAntispamManager2(int uid) {
		return locateProxy(managers, "M", uid, Channel.Invoke.Twoway,
				AntispamManager2PrxHelper.class, 300);
	}

	private AntispamManager2Prx getAntispamManager2Oneway(int uid) {
		return locateProxy(managersOneway, "M", uid, Channel.Invoke.Oneway,
				AntispamManager2PrxHelper.class, 300);
	}

	public static void main(String[] args) {
		Antispam2AdapterImpl adapter = new Antispam2AdapterImpl();
		System.out.println(adapter.htmlFilter("fdsfdsa&#;&;"));
	}

}
