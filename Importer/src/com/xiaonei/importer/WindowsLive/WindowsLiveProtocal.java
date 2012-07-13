package com.xiaonei.importer.WindowsLive;

import java.net.URL;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.httpclient.contrib.ssl.EasySSLProtocolSocketFactory;
import org.apache.commons.httpclient.protocol.Protocol;
import org.apache.commons.httpclient.protocol.ProtocolSocketFactory;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Node;
import org.dom4j.XPath;

import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyInfoStatus;

public class WindowsLiveProtocal {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(WindowsLiveProtocal.class);

	public static final String getAuthEnv(String username, String password) {
		return "<s:Envelope"
				+ "    xmlns:s = \"http://www.w3.org/2003/05/soap-envelope\""
				+ "    xmlns:wsse = \"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\""
				+ "    xmlns:saml = \"urn:oasis:names:tc:SAML:1.0:assertion\""
				+ "    xmlns:wsp = \"http://schemas.xmlsoap.org/ws/2004/09/policy\""
				+ "    xmlns:wsu = \"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\""
				+ "    xmlns:wsa = \"http://www.w3.org/2005/08/addressing\""
				+ "    xmlns:wssc = \"http://schemas.xmlsoap.org/ws/2005/02/sc\""
				+ "    xmlns:wst = \"http://schemas.xmlsoap.org/ws/2005/02/trust\">"
				+ "    <s:Header>"
				+ "        <wlid:ClientInfo xmlns:wlid = \"http://schemas.microsoft.com/wlid\">"
				+ "            <wlid:ApplicationID>your ApplicationID Here</wlid:ApplicationID>"
				+ "        </wlid:ClientInfo>"
				+ "        <wsa:Action s:mustUnderstand = \"1\">http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue</wsa:Action>"
				+ "        <wsa:To s:mustUnderstand = \"1\">https://dev.login.live.com/wstlogin.srf</wsa:To>"
				+ "        <wsse:Security>"
				+ "            <wsse:UsernameToken wsu:Id = \"user\">"
				+ "                <wsse:Username>"
				+ username
				+ "</wsse:Username>"
				+ "                <wsse:Password>"
				+ password
				+ "</wsse:Password>"
				+ "            </wsse:UsernameToken>"
				+ "        </wsse:Security>"
				+ "    </s:Header>"
				+ "    <s:Body>"
				+ "        <wst:RequestSecurityToken Id = \"RST0\">"
				+ "            <wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>"
				+ "            <wsp:AppliesTo>"
				+ "                <wsa:EndpointReference>"
				+ "                    <wsa:Address>http://live.com</wsa:Address>"
				+ "                </wsa:EndpointReference>"
				+ "            </wsp:AppliesTo>"
				+ "            <wsp:PolicyReference URI = \"MBI\"></wsp:PolicyReference>"
				+ "        </wst:RequestSecurityToken>" + "    </s:Body>"
				+ "</s:Envelope>";
	}

	public static final String wstEndpoints = "https://dev.login.live.com/wstlogin.srf";

	public static final String contactsEndpoints = "https://cumulus.services.live.com";

	public static final String contactsHeaderName = "Authorization";

	public static final String getContactHeaderValue(String ticket) {
		return "WLID1.0 t=\"" + ticket + "\"";
	}

	public static final Map<String, String> namespaces = new HashMap<String, String>();
	static {
		namespaces
				.put(
						"wsse",
						"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
		namespaces.put("wst", "http://schemas.xmlsoap.org/ws/2005/02/trust");
		namespaces
				.put(
						"wsu",
						"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd");
	}
	static {
		try {
			URL trusturl = ClassLoader.getSystemResource("livecert");
			// ProtocolSocketFactory factory = new
			// AuthSSLProtocolSocketFactory(trusturl, "changeit", trusturl,
			// "changeit");
			ProtocolSocketFactory factory = new EasySSLProtocolSocketFactory();
			Protocol myhttps = new Protocol("https", factory, 443);
			Protocol.registerProtocol("https", myhttps);
		} catch (Exception e) {
			_logger.warn("Loading certification error.", e);
		}
	}

	public static final WindowsLiveAuthToken getTicket(String response)
			throws DocumentException, ParseException {
		try {
			Document doc = DocumentHelper.parseText(response);

			XPath xpathToken = DocumentHelper
					.createXPath("//wst:RequestedSecurityToken/wsse:BinarySecurityToken/text()");
			xpathToken.setNamespaceURIs(namespaces);
			Node nodeToken = xpathToken.selectSingleNode(doc);
			String ticket = nodeToken.getText();

			XPath xpathCreate = DocumentHelper
					.createXPath("//wst:Lifetime/wsu:Created/text()");
			xpathCreate.setNamespaceURIs(namespaces);
			Node nodeCreate = xpathCreate.selectSingleNode(doc);
			String create = nodeCreate.getText();

			XPath xpathExpire = DocumentHelper
					.createXPath("//wst:Lifetime/wsu:Expires/text()");
			xpathExpire.setNamespaceURIs(namespaces);
			Node nodeExpire = xpathExpire.selectSingleNode(doc);
			String expire = nodeExpire.getText();

			Date createDate = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'")
					.parse(create);
			Date expireDate = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'")
					.parse(expire);

			return new WindowsLiveAuthToken(createDate, expireDate, ticket
					.substring(2));
		} catch (Exception e) {
			return null;
		}
	}

	public static final BuddyInfo[] getBuddInfos(String response)
			throws DocumentException {
		Document doc = DocumentHelper.parseText(response);
		XPath xpathContacts = DocumentHelper.createXPath("Contacts/Contact");
		List<Node> contacts = xpathContacts.selectNodes(doc);
		List<BuddyInfo> result = new ArrayList<BuddyInfo>();
		for (int i = 0; i < contacts.size(); ++i) {
			Node node = contacts.get(i);
			// String firstName = "";
			// String lastName = "";
			String nickName = "";
			String displayName = "";
			String email = "";
			int statusCode = 0;
			String statusMessage = "Contact not got.";
			try {
				email = node.selectSingleNode("Emails/Email/Address").getText();
			} catch (Exception e) {
				statusCode = -1;
				statusMessage = "No Email Address";
			}
			// firstName =
			// node.selectSingleNode("Profiles/Personal/FirstName/text()").getText();
			// lastName =
			// node.selectSingleNode("Profiles/Personal/LastName/text()").getText();
			try {
				nickName = node.selectSingleNode(
						"Profiles/Personal/NickName/text()").getText();
				if (nickName == null) {
					nickName = "";
				}
			} catch (Exception e) {
				// no name is not crictal
			}
			try {
				displayName = node.selectSingleNode(
						"Profiles/Personal/DisplayName/text()").getText();
				if (displayName == null) {
					displayName = "";
				}
			} catch (Exception e) {
				// no name is not crictal
			}
			if (statusCode == 0) {
				result.add(new BuddyInfo(nickName + " " + displayName, email,
						new BuddyInfoStatus(statusCode, statusMessage)));
			}
		}
		return result.toArray(new BuddyInfo[result.size()]);
	}
}
