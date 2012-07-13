package com.xiaonei.xce.windowsliveimporter;

import java.util.Vector;
import com.xiaonei.xce.log.Oce;
import java.util.concurrent.atomic.AtomicInteger;

import xce.importer.AMI_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationID;
import xce.importer.AMI_WindowsLiveImporterManager_contactsByPassword;
import xce.importer.WindowsLiveContacts;
import xce.importer.WindowsLiveImporterManagerPrx;
import xce.importer.WindowsLiveImporterManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.channel.Channel.Invoke;
import xce.util.service.ServiceAdapter;
import Ice.LocalException;

import com.live.login.WindowsLiveLogin;
import com.live.login.WindowsLiveLogin.ConsentToken;

public final class WindowsLiveImporterNAdapter extends ServiceAdapter {

	public class WindowsLiveException extends Exception {

		private static final long serialVersionUID = -4485117006995219546L;

		public WindowsLiveException(String message) {
			_message = message;
		}

		@Override
		public String toString() {
			return "WindowsLiveException: " + _message;
		}

		private final String _message;
	}

	private class AMI_WindowsLiveImporterManager_contactsByPasswordI extends
			AMI_WindowsLiveImporterManager_contactsByPassword {
		private final ContactsCallback _callback;

		public AMI_WindowsLiveImporterManager_contactsByPasswordI(
				ContactsCallback callback) {
			_callback = callback;
		}

		@Override
		public void ice_exception(LocalException ex) {
			_callback.exception(ex);
		}

		@Override
		public void ice_response(WindowsLiveContacts __ret) {
			_callback.finished(new Contacts(__ret));
		}

	}

	private class AMI_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDI
			extends
			AMI_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationID {
		private final ContactsCallback _callback;

		public AMI_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDI(
				ContactsCallback callback) {
			_callback = callback;
		}

		@Override
		public void ice_exception(LocalException ex) {
			_callback.exception(ex);
		}

		@Override
		public void ice_response(WindowsLiveContacts __ret) {
			_callback.finished(new Contacts(__ret));
		}

	}

	private final WindowsLiveLogin _wll;
	private Vector<WindowsLiveImporterManagerPrx> _managers = new Vector<WindowsLiveImporterManagerPrx>();

	public WindowsLiveImporterNAdapter() {
		super("@WindowsLiveImporter", 10, Channel
				.newImporterChannel("WindowsLiveImporter"));
		_wll = null;
	}

	public WindowsLiveImporterNAdapter(String appId, String secret) {
		super("@WindowsLiveImporter", 10, Channel
				.newImporterChannel("WindowsLiveImporter"));
		_wll = new WindowsLiveLogin(appId, secret);
	}

	/**
	 * @deprecated Use
	 *             {@link #requestByPasswordAsync(int,String,String,ContactsCallback)}
	 *             instead
	 */
	public void requestAsync(int userId, String email, String passwd,
			ContactsCallback callback) {
		requestByPasswordAsync(userId, email, passwd, callback);
	}

	public void requestByPasswordAsync(int userId, String email, String passwd,
			ContactsCallback callback) {
		Oce.getLogger().error(this.getClass().getName() + ".requestByPasswordAsync userId=" + userId + " email="+email);
		getManager(userId)
				.contactsByPassword_async(
						new AMI_WindowsLiveImporterManager_contactsByPasswordI(
								callback), userId, email, passwd);
	}

	public void requestByConsentTokenAsync(int userId, String email,
			String token, ContactsCallback callback)
			throws WindowsLiveException {
		if (_wll == null) {
			throw new WindowsLiveException(
					"WindowsLiveImporterAdapter should be initialized with appid and secret.");
		}
		ConsentToken ct = _wll.processConsentToken(token);
		if (ct == null) {
			throw new WindowsLiveException("Token format error");
		}
		Oce.getLogger().error(this.getClass().getName() + ".requestByConsentTokenAsync userId=" + userId + " email="+email);
		getManager(userId)
				.contactsByDelegationTokenAndLocationID_async(
						new AMI_WindowsLiveImporterManager_contactsByDelegationTokenAndLocationIDI(
								callback), userId, email,
						ct.getDelegationToken(), ct.getLocationID());
	}

	/**
	 * @deprecated Use {@link #requestByPassword(int,String,String)} instead
	 */
	public void request(int userId, String email, String passwd) {
		requestByPassword(userId, email, passwd);
	}

	public void requestByPassword(int userId, String email, String passwd) {
		Oce.getLogger().error(this.getClass().getName() + ".requestByPassword userId=" + userId + " email="+email);
		getManager(userId).requestByPassword(userId, email, passwd);
	}

	public void requestByConsentToken(int userId, String email, String token)
			throws WindowsLiveException {
		if (_wll == null) {
			throw new WindowsLiveException(
					"WindowsLiveImporterAdapter should be initialized with appid and secret.");
		}
		ConsentToken ct = _wll.processConsentToken(token);
		if (ct == null) {
			throw new WindowsLiveException("Token format error");
		}
		Oce.getLogger().error(this.getClass().getName() + ".requestByConsentToken userId=" + userId + " email="+email);
		getManager(userId).requestByDelegationTokenAndLocationID(userId, email,
				ct.getDelegationToken(), ct.getLocationID());
	}

	public WindowsLiveContacts response(int userId, String email) {
		Oce.getLogger().error(this.getClass().getName() + ".response userId=" + userId + " email="+email);
		return getManager(userId).response(userId, email);
	}

	private static AtomicInteger verifyPos = new AtomicInteger(0);

	public boolean verifyAccount(String email, String passwd) {
		Oce.getLogger().error(this.getClass().getName() + ".verifyAccount email="+email);
		return getManager(verifyPos.getAndIncrement()).verifyAccount(email,
				passwd);
	}

	private WindowsLiveImporterManagerPrx getManager(int userId) {
		return locateProxy(_managers, "M", userId, Invoke.Twoway,
				WindowsLiveImporterManagerPrxHelper.class, 30000);
	}
	
	public static void main(String[] args) {
		long begin = System.currentTimeMillis();
		WindowsLiveImporterAdapter adapter = new WindowsLiveImporterAdapter();
		// System.out.println(adapter.verifyAccount(args[0], args[1]));
		// System.out.println(adapter.verifyAccount("han.zhang@opi-corp.com", "saladin"));
		// System.out.println(adapter.verifyAccount("tissy@sina.com", "1qaz2wsx"));
		
		int userId = 222525090;
		String email = "han.zhang@opi-corp.com"; 		String passwd = "saladin";
		// String email = "tissy@sina.com"; 		String passwd = "1qaz2wsx";
		// adapter.request(userId, email, passwd);
		/* try { 
			Thread.sleep(3000);
		} catch (Exception e) {
			e.printStackTrace();
		} */
		WindowsLiveContacts cts = adapter.response(userId, email);
		System.out.println("contacts length : " + cts.contacts.Contacts.length);
		if ( cts.contacts.Contacts.length > 0 ) {
			System.out.println("contacts 0 : " + cts.contacts.Contacts[0].WindowsLiveID);
		}
		System.out.println("contacts state : " + cts.state.name());		
		
		System.out.println(System.currentTimeMillis()-begin);
		System.exit(0);
	}
}
