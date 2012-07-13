package com.xiaonei.xce.feed.reply;

/**
 * 工具类
 * <p>
 * 单例模式
 * 
 * @author 康伟 2008-8-12
 * @version 1.0
 */
public class XmlFilter {
	private static XmlFilter instance = new XmlFilter();

	public static XmlFilter getInstance() {
		return instance;
	}
	public static void main(String[] args){
		String str = "☆晶晶★J&J@42";
		System.out.println(XmlFilter.getInstance().write(str));
	}
	/**
	 * 首先检查是否文本中是否包含非UTF-8集合的字符，如果有非UTF-8字符，将之替换。 然后检查是否包含XML保留字
	 * <code> &lt;</code> <code> &gt;</code><code> '</code><code> " </code><code> &amp;</code>
	 * 如果有将之替换掉
	 * 
	 * @param xml
	 *            需要检查的XML文档
	 * @return 过滤之后的XML文档
	 */
	public String write(String xml) {
		if (xml == null)
			return "";
		xml = replaceInvaldateCharacter(xml);
		xml = escapeAttributeEntities(xml);
		return xml;
	}

	/**
	 * 检查字符是否为合法的xml字符。
	 * <p>
	 * XML规范中规定了允许的字符范围(http://www.w3.org/TR/REC-xml#dt-character):
	 * <p>
	 * Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] |
	 * <p>
	 * [#x10000-#x10FFFF]
	 * <p>
	 * 如果遇见非法字符，将之转换成0x20
	 */
	private String replaceInvaldateCharacter(String text) {
		if (text != null) {
			char[] data = text.toCharArray();
			boolean result = false;
			for (int i = 0; i < data.length; i++) {
				result = isXMLCharacter(data[i]);
				if (!result) {
					data[i] = 0x20;
				}
			}
			return new String(data);
		}
		return text;
	}

	/**
	 * 检查是否为UTF-8字符集内的字符
	 * 
	 * @param c
	 *            带检查字符
	 * @return 是 返回 真 ，不是返回 假
	 */
	private boolean isXMLCharacter(char c) {
	    	if(c == 0x1b){
	    	    return false;
	    	}
		if (c <= 0xD7FF) {
			if (c >= 0x20) {
				return true;
			} else {
				return c == '\n' || c == '\r' || c == '\t';
			}
		}
		return (c >= 0xE000 && c <= 0xFFFD) || (c >= 0x10000 && c <= 0x10FFFF);
	}

	/**
	 * 过滤掉XML的保留字符
	 * 
	 * @param text
	 *            待过滤的XML文档
	 * @return 过滤后的XML文档
	 */
	private String escapeAttributeEntities(String text) {
		StringBuffer buffer = new StringBuffer();
		char[] block = null;
		int i;
		int last = 0;
		int size = text.length();

		for (i = 0; i < size; i++) {
			String entity = null;
			char c = text.charAt(i);

			switch (c) {
			case '<':
				entity = "&lt;";

				break;

			case '>':
				entity = "&gt;";

				break;

			case '\'':
				entity = "&apos;";

				break;

			case '\"':
				entity = "&quot;";

				break;

			case '&':
				entity = "&amp;";

				break;
			}

			if (entity != null) {
				if (block == null) {
					block = text.toCharArray();
				}

				buffer.append(block, last, i - last);
				buffer.append(entity);
				last = i + 1;
			}
		}

		if (last == 0) {
			return text;
		}

		if (last < size) {
			if (block == null) {
				block = text.toCharArray();
			}

			buffer.append(block, last, i - last);
		}

		String answer = buffer.toString();
		buffer.setLength(0);

		return answer;
	}

}