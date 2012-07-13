package xce.util.tools;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Enumeration;

public class IPAddress {
	public static final String getLocalAddress() {
		try {
			for (Enumeration<NetworkInterface> ni = NetworkInterface
					.getNetworkInterfaces(); ni.hasMoreElements();) {
				NetworkInterface eth = ni.nextElement();
				for (Enumeration<InetAddress> add = eth.getInetAddresses(); add
						.hasMoreElements();) {
					InetAddress i = add.nextElement();
					if (i instanceof Inet4Address) {
						if (i.isSiteLocalAddress()) {
							return i.getHostAddress();
						}
					}
				}
			}
		} catch (SocketException e) {
			e.printStackTrace();
		}
		return "";
	}

	public static final String getLocalHostName() {
		try {
			return Inet4Address.getLocalHost().getHostName();
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
		return "";
	}

	public static final boolean isRenRen() {
		return isXiaonei();
	}
	
	public static final boolean isXiaonei() {
		try {
			InetAddress local = java.net.Inet4Address
					.getByName("XiaoNeiRegistry");
			InetAddress remote = java.net.Inet4Address
					.getByName("XiaoNeiRegistry.d.xiaonei.com");
			return local.equals(remote);
		} catch (Throwable e) {
			
		}
		return false;
	}

	public static final boolean isKaixin() {
		try {
			InetAddress local = java.net.Inet4Address
					.getByName("XiaoNeiRegistry");
			InetAddress remote = java.net.Inet4Address
					.getByName("XiaoNeiRegistry.d.kaixin.com");
			return local.equals(remote);
		} catch (Throwable e) {
			
		}
		return false;
	}

	public static void main(String[] args) {
		System.out.println("isKaixin: " + isKaixin());
		System.out.println("isXiaonei: " + isXiaonei());
	}
}
