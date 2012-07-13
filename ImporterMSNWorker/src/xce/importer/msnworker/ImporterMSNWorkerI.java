package xce.importer.msnworker;

import java.util.ArrayList;
import java.util.List;

import net.sf.jml.MsnContact;
import net.sf.jml.MsnContactList;
import net.sf.jml.MsnGroup;
import net.sf.jml.MsnMessenger;
import net.sf.jml.event.MsnContactListListener;
import net.sf.jml.event.MsnMessengerListener;
import net.sf.jml.impl.MsnMessengerFactory;
import xce.importer.AMD_ImporterMSNWorker_contacts;
import xce.importer._ImporterMSNWorkerDisp;
import Ice.Current;

import com.live.schema.TContact;
import com.live.schema.TEmail;
import com.live.schema.TLiveContacts;
import com.live.schema.TOwner;
import com.live.schema.TPersonal;
import com.live.schema.TProfessional;
import com.live.schema.TProfiles;
import com.live.schema.TTag;

public class ImporterMSNWorkerI extends _ImporterMSNWorkerDisp {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory.getLog(ImporterMSNWorkerI.class);

	private class WorkThread extends Thread {
		private final String _email;
		private final String _passwd;
		private final AMD_ImporterMSNWorker_contacts _callback;

		public WorkThread(String email, String passwd, AMD_ImporterMSNWorker_contacts callback) {
			_email = email;
			_passwd = passwd;
			_callback = callback;
		}

		@Override
		public void run() {
			MsnMessenger messenger = MsnMessengerFactory.createMsnMessenger(_email, _passwd);

			messenger.setLogIncoming(false);
			messenger.setLogOutgoing(false);
			messenger.addContactListListener(new MsnContactListListener() {

				@Override
				public void contactAddCompleted(MsnMessenger arg0, MsnContact arg1) {
				}

				@Override
				public void contactAddedMe(MsnMessenger arg0, MsnContact arg1) {
				}

				@Override
				public void contactListInitCompleted(MsnMessenger messenger) {
					_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : started");
					List<TContact> contacts = new ArrayList<TContact>();
					MsnContactList list = messenger.getContactList();
					_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : count=" + list.getContacts().length);
					for (MsnContact c : list.getContacts()) {
						try {
							TContact contact = new TContact();
							contact.Emails = new TEmail[] { new TEmail("", "", c.getEmail().getEmailAddress(), "", "") };
							contact.Profiles = new TProfiles(new TPersonal(), new TProfessional());
							contact.Profiles.Personal.DisplayName = c.getDisplayName();
							contact.Profiles.Personal.NickName = c.getFriendlyName();
							contacts.add(contact);
							_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : email=" + c.getEmail().getEmailAddress());
						}catch(Exception e){
							e.printStackTrace(System.out);
						}
					}

					_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : start owner");
					TOwner owner = new TOwner();
					owner.WindowsLiveID = messenger.getOwner().getEmail().getEmailAddress();
					owner.Emails = new TEmail[] { new TEmail("", "", messenger.getOwner().getEmail().getEmailAddress(), "", "") };
					owner.Profiles = new TProfiles(new TPersonal(), new TProfessional());
					owner.Profiles.Personal.DisplayName = messenger.getOwner().getDisplayName();
					owner.Profiles.Personal.NickName = messenger.getOwner().getPersonalMessage();
					TLiveContacts result = new TLiveContacts(owner, contacts.toArray(new TContact[contacts.size()]), new TTag[] {});
					_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : start ice_response");
					_callback.ice_response(result);
					_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : done ice_response, now logout.");
					messenger.logout();
					_logger.debug("contactListInitCompleted " + _email + " " + _passwd + " : all done");
				}

				@Override
				public void contactListSyncCompleted(MsnMessenger messenger) {
					System.out.println("contactListSyncCompleted");
				}

				@Override
				public void contactRemoveCompleted(MsnMessenger arg0, MsnContact arg1) {
				}

				@Override
				public void contactRemovedMe(MsnMessenger arg0, MsnContact arg1) {
				}

				@Override
				public void contactStatusChanged(MsnMessenger arg0, MsnContact arg1) {
				}

				@Override
				public void groupAddCompleted(MsnMessenger arg0, MsnGroup arg1) {
				}

				@Override
				public void groupRemoveCompleted(MsnMessenger arg0, MsnGroup arg1) {
				}

				@Override
				public void ownerStatusChanged(MsnMessenger arg0) {
				}
			});

			messenger.addMessengerListener(new MsnMessengerListener() {

				@Override
				public void exceptionCaught(MsnMessenger arg0, Throwable arg1) {
					// _callback.ice_exception(new
					// Exception("exceptionCaught"));
				}

				@Override
				public void loginCompleted(MsnMessenger arg0) {

				}

				@Override
				public void logout(MsnMessenger arg0) {
					_callback.ice_exception(new Exception("logout"));
				}
			});
			_logger.debug("Login " + _email + " " + _passwd);
			messenger.login();
			System.out.println("Login done.");
		}
	}

	@Override
	public void contacts_async(AMD_ImporterMSNWorker_contacts callback, String email, String passwd, Current __current) {
		Thread t = new WorkThread(email, passwd, callback);
		t.setDaemon(true);
		t.start();
	}
}
