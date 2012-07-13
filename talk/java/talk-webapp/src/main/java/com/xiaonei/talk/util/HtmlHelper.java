package com.xiaonei.talk.util;

import java.util.Stack;

class Symbol {
	public int index;
	public char ch;

	public Symbol(int index, char ch) {
		this.index = index;
		this.ch = ch;
	}
};

public class HtmlHelper {

	 static String cleanHtmlTag(String text) {

		text = text.replaceAll("&nbsp;", " ");

		Stack<Symbol> symbolStack = new Stack<Symbol>();
		StringBuffer buf = new StringBuffer();

		for (int i = 0; i < text.length(); ++i) {
			if (text.charAt(i) == '<') {
				if (symbolStack.empty()) {
					symbolStack.push(new Symbol(i, '<'));
					continue;
				} else {
					return "";
				}
			}

			if (text.charAt(i) == '>') {
				if (symbolStack.empty()) {
					return "";
				} else {
					symbolStack.pop();
					buf.append(" ");
					continue;
				}
			}

			if (symbolStack.empty()) {
				buf.append(text.charAt(i));
			}
		}
		if (symbolStack.empty()) {
			String res = buf.toString().replaceAll("\\s+", " ");
			return res;
		} else
			return "";
	}

	static String htmlDecode(String text) {

		StringBuffer res = new StringBuffer();
		int index = 0;
		for (int i = 0; i < text.length();) {
			index = i;
			if (text.charAt(index) == '&') {
				switch (text.charAt(++index)) {
				case 'l': {
					if (text.charAt(index + 1) == 't'
							&& text.charAt(index + 2) == ';') {
						res.append('<');

						i = index + 3;
						continue;
					}
					res.append(text.substring(i, index + 1));
					i = index + 1;
					continue;
				}
				case 'g': {
					if (text.charAt(index + 1) == 't'
							&& text.charAt(index + 2) == ';') {
						res.append('>');

						i = index + 3;
						continue;
					}
					res.append(text.substring(i, index + 1));
					i = index + 1;
					continue;

				}
				case 'a': {
					if (text.charAt(index + 1) == 'm'
							&& text.charAt(index + 2) == 'p'
							&& text.charAt(index + 3) == ';') {
						res.append('&');

						i = index + 4;
						continue;
					}
					res.append(text.substring(i, index + 1));
					i = index + 1;
					continue;

				}
				case 'q': {
					if (text.charAt(index + 1) == 'u'
							&& text.charAt(index + 2) == 'o'
							&& text.charAt(index + 3) == 't'
							&& text.charAt(index + 4) == ';') {

						res.append('"');
						i = index + 5;
						continue;
					}
					res.append(text.substring(i, index + 1));
					i = index + 1;
					continue;
				}
				case 'n': {
					if (text.charAt(index + 1) == 'b'
							&& text.charAt(index + 2) == 's'
							&& text.charAt(index + 3) == 'p'
							&& text.charAt(index + 4) == ';') {

						res.append(' ');
						i = index + 5;
						continue;
					}
					res.append(text.substring(i, index + 1));
					i = index + 1;
					continue;
				}
				case '#': {
					if (text.charAt(index + 1) == '3'
							&& text.charAt(index + 2) == '5'
							&& text.charAt(index + 3) == ';') {
						res.append('#');
						i = index + 4;
						continue;
					}
					if (text.charAt(index + 1) == '4'
							&& text.charAt(index + 2) == '0'
							&& text.charAt(index + 3) == ';') {
						res.append('(');
						i = index + 4;
						continue;
					}
					if (text.charAt(index + 1) == '4'
							&& text.charAt(index + 2) == '1'
							&& text.charAt(index + 3) == ';') {
						res.append(')');
						i = index + 4;
						continue;
					}
					if (text.charAt(index + 1) == '3'
							&& text.charAt(index + 2) == '4'
							&& text.charAt(index + 3) == ';') {
						res.append('"');
						i = index + 4;
						continue;
					}
					if (text.charAt(index + 1) == '3'
							&& text.charAt(index + 2) == '9'
							&& text.charAt(index + 3) == ';') {
						res.append('\'');
						i = index + 4;
						continue;
					}
					res.append(text.substring(i, index + 1));
					i = index + 1;
					continue;
				}

				}
			}
			res.append(text.charAt(i++));
		}
		return res.toString();
	}

	static String xmlEncode(String text) {

		StringBuffer res = new StringBuffer();
		for (int i = 0; i < text.length(); ++i) {
			if (text.charAt(i) == '<') {
				res.append("&lt;");
				continue;
			}
			if (text.charAt(i) == '>') {
				res.append("&gt;");
				continue;
			}
			if (text.charAt(i) == '&') {
				res.append("&amp;");
				continue;
			}
			res.append(text.charAt(i));
		}
		return res.toString();
	}
	
	public static String dataFormat(String text){
		String tmp = cleanHtmlTag(text);
		tmp = htmlDecode(tmp);
		return xmlEncode(tmp);
	}
	/*
	 * public static String htmlEncode(String text) {
	 * 
	 * StringBuffer buf = new StringBuffer(); for (int i = 0; i < text.length();
	 * ++i) { switch (text.charAt(i)) { case '&': { buf.append("&amp;"); break; }
	 * case '<': { buf.append("&lt;"); break; } case '>': { buf.append("&gt;");
	 * break; } case '\"': { buf.append("&quot;"); break; } case '\'': {
	 * buf.append("&apos;"); break; } default: { buf.append(text.charAt(i)); } } }
	 * 
	 * return buf.toString(); }
	 */
}
